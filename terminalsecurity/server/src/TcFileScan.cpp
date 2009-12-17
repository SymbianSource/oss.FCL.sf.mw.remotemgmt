/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Implementation of terminalsecurity components
*
*/


// INCLUDES
#include <e32base.h>
#include "TcFileScan.h"
#include "TerminalControlClient.h" // For TFileName8
#include "debug.h"

// MACROS
_LIT8(KFormatFolderEmpty, "<folder name=\"%S\"/>\r\n");
_LIT8(KFormatFolderOpen,  "<folder name=\"%S\">\r\n");
_LIT8(KFormatFolderClose, "</folder>\r\n");
_LIT8(KFormatFileOpen,    "<file name=\"%S\"/>\r\n");
_LIT8(KFormatDriveName,   "<drive name=\"%S\"/>\r\n");
_LIT8(KFormatListingOpen, "<folder-listing version=\"1.1\">\r\n");
_LIT8(KFormatListingClose,"</folder-listing>\r\n");

// METHODS


CTcFileScan* CTcFileScan::NewL()
	{
	return new (ELeave) CTcFileScan();	
	}


// ----------------------------------------------------------------------------------------
// CTcFileScan::CTcFileScan
// ----------------------------------------------------------------------------------------
CTcFileScan::CTcFileScan()
: iFileScanResults(0)
    {
	RDEBUG("CTcFileScan::CTcFileScan");

    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::~CTcFileScan
// ----------------------------------------------------------------------------------------
CTcFileScan::~CTcFileScan()
    {
	RDEBUG("CTcFileScan::~CTcFileScan");

    delete iFileScanResults;
    }    

// ----------------------------------------------------------------------------------------
// CTcFileScan::FileScanL
// ----------------------------------------------------------------------------------------
void CTcFileScan::FileScanL( const TDesC8 &aFileName, TBool aRecursive )
    {
		RDEBUG_2( "CTcFileScan::FileScanL: %S", &aFileName );
		RDEBUG_2( "CTcFileScan::FileScanL: Recursive: %d", aRecursive );

    RFs fileSession;
    User::LeaveIfError(fileSession.Connect());
    CleanupClosePushL(fileSession);

    TBuf<sizeof(TFileName8)+2> startDirBuf;
    startDirBuf.Copy(aFileName.Left(sizeof(TFileName8)));
    startDirBuf.TrimAll();

    //
    // Add asterisk if path is specified
    //
    if(startDirBuf.Length() > 0)
        {
        if(startDirBuf.Right(1) != _L("\\"))
            {
            startDirBuf.Append(TChar('\\'));
            }
            
        startDirBuf.Append(TChar('*'));
        }        
    
    if( iFileScanResults != 0 )
        {
        delete iFileScanResults;
        iFileScanResults = 0;
        }

    iFileScanResults = CBufFlat::NewL(128);

    // Every scan result begins with <folder-listing>
    //
    iFileScanResults->InsertL(0, KFormatListingOpen);
    iRecurseLevel = 0;
    
    if( startDirBuf.Length() > 0 )
        {        
        ScanDirectoryL( fileSession, startDirBuf, aRecursive );
        }
    else
        {
        GetDriveListL();
        }

    // Every scan result ends with </folder-listing>
    //
    iFileScanResults->InsertL(iFileScanResults->Size(), KFormatListingClose);

    CleanupStack::PopAndDestroy( &fileSession );
    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::ScanDirectoryL
// ----------------------------------------------------------------------------------------
void CTcFileScan::ScanDirectoryL( RFs& aFileSession,
                                  const TDesC &aDirectory,
                                  TBool aRecursive )
    {
		RDEBUG_2( "CTcFileScan::ScanDirectoryL: %S", &aDirectory);

    // Note: this function is used recursively
    // Stack use is minimized to avoid overflow as long as possible
    //
    iRecurseLevel ++;
    TFindFile* fileFinder1 = new (ELeave) TFindFile( aFileSession );
    CDir*      fileList1;

    TInt err = fileFinder1->FindWildByPath(aDirectory, &KNullDesC(), fileList1);

    while( err == KErrNone )
        {
        TInt i;

        for (i=0; i<fileList1->Count(); i++)
            {
            const TEntry & entry = (*fileList1)[i];
            TInt iFilesInDirectory = 0;

            //
            // Is there any files in directory?
            //
            if(aRecursive && entry.IsDir())
                {
                HBufC *fullPathName1 = CreateScanPathNameL(aDirectory, entry.iName, _L("*"));
                CleanupStack::PushL( fullPathName1 );
                
                iFilesInDirectory = FilesInDirectoryL( aFileSession, *fullPathName1 );
                
                CleanupStack::PopAndDestroy( fullPathName1 );    
                }

            //
            // Add file information into result buffer
            //
            BeginAddFileToResultsL( TPtrC ( entry.iName ), entry.IsDir(), iFilesInDirectory );

            //
            // Scan subdirectory
            //
            if(aRecursive && entry.IsDir() && (iRecurseLevel < 200))
                {
                HBufC *fullPathName2 = CreateScanPathNameL(aDirectory, entry.iName, _L("*"));                
                CleanupStack::PushL( fullPathName2 );
                
                ScanDirectoryL( aFileSession, fullPathName2->Des(), ETrue );
                                
                CleanupStack::PopAndDestroy( fullPathName2 );
                }
            
            //
            // More file information into result buffer
            //
            EndAddFileToResultsL( entry.IsDir(), iFilesInDirectory );
            }

        delete fileList1;

        err = fileFinder1->FindWild( fileList1 );
        }

    delete fileFinder1;
    
    iRecurseLevel --;
    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::BeginAddFileToResults
// ----------------------------------------------------------------------------------------
void CTcFileScan::BeginAddFileToResultsL( const TDesC &aFileName,
                                         TBool aIsDir,
                                         TInt aFiles )
    {    
	RDEBUG("CTcFileScan::BeginAddFileToResultsL");

    iTabString.Zero();
    if( iRecurseLevel > 0)
        {
        TInt max = (iRecurseLevel>sizeof(iTabString)?sizeof(iTabString):iRecurseLevel);
        for(TInt i=0; i<max; i++)
            {
            iTabString.Append(TChar('\t'));
            }
        }
        
    TFileName8 filename; filename.Copy( aFileName.Left( sizeof( TFileName8 ) ) );
    
    if(aIsDir)
        {
        if(aFiles == 0)
            {
            TInt appendPosition = iFileScanResults->Size();
            TInt appendSize = filename.Length() + KFormatFolderEmpty().Length() + iTabString.Length();

            HBufC8* appendString = HBufC8::NewLC(appendSize);
            appendString->Des().Format(KFormatFolderEmpty, &filename);
            appendString->Des().Insert(0, iTabString);

            iFileScanResults->InsertL(appendPosition, *appendString);
            CleanupStack::PopAndDestroy(appendString);
            }
        else
            {
            TInt appendPosition = iFileScanResults->Size();
            TInt appendSize = filename.Length() + KFormatFolderOpen().Length() + iTabString.Length();
            
            HBufC8* appendString = HBufC8::NewLC(appendSize);
            appendString->Des().Format(KFormatFolderOpen, &filename);
            appendString->Des().Insert(0, iTabString);

            iFileScanResults->InsertL(appendPosition, *appendString);
            CleanupStack::PopAndDestroy(appendString);            
            }
        }
    else
        {
        TInt appendPosition = iFileScanResults->Size();
        TInt appendSize = filename.Length() + KFormatFileOpen().Length() + iTabString.Length();
        
        HBufC8* appendString = HBufC8::NewLC(appendSize);
        appendString->Des().Format(KFormatFileOpen, &filename);
        appendString->Des().Insert(0, iTabString);

        iFileScanResults->InsertL(appendPosition, *appendString);
        CleanupStack::PopAndDestroy(appendString);                        
        }
    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::EndAddFileToResults
// ----------------------------------------------------------------------------------------
void CTcFileScan::EndAddFileToResultsL( TBool aIsDir,
                                        TInt aFiles )
    {   
	RDEBUG("CTcFileScan::EndAddFileToResultsL");

    iTabString.Zero();
    if( iRecurseLevel > 0)
        {
        TInt max = (iRecurseLevel>sizeof(iTabString)?sizeof(iTabString):iRecurseLevel);
        for(TInt i=0; i<max; i++)
            {
            iTabString.Append(TChar('\t'));
            }
        }

    if(aIsDir)
        {
        if(aFiles > 0)
            {
            TInt appendPosition = iFileScanResults->Size();
            TInt appendSize = KFormatFolderClose().Length() + iTabString.Length();
            
            HBufC8* appendString = HBufC8::NewLC(appendSize);
            appendString->Des().Copy(KFormatFolderClose);
            appendString->Des().Insert(0, iTabString);

            iFileScanResults->InsertL(appendPosition, *appendString);
            CleanupStack::PopAndDestroy(appendString);
            }
        }
    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::AddDriveToResults
// ----------------------------------------------------------------------------------------
void CTcFileScan::AddDriveToResultsL( const TDesC& aDriveName )
    {
	RDEBUG("CTcFileScan::AddDriveToResultsL");

    iTabString.Zero();
    if( iRecurseLevel > 0)
        {
        TInt max = (iRecurseLevel>sizeof(iTabString)?sizeof(iTabString):iRecurseLevel);
        for(TInt i=0; i<max; i++)
            {
            iTabString.Append(TChar('\t'));
            }
        }

    TFileName8 filename; filename.Copy( aDriveName.Left( sizeof( TFileName8 ) ) );

    TInt appendPosition = iFileScanResults->Size();
    TInt appendSize = filename.Length() + KFormatDriveName().Length() + iTabString.Length();

    HBufC8* appendString = HBufC8::NewLC(appendSize);
    appendString->Des().Format(KFormatDriveName, &filename);
    appendString->Des().Insert(0, iTabString);

    iFileScanResults->InsertL(appendPosition, *appendString);
    CleanupStack::PopAndDestroy(appendString);
    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::FilesInDirectoryL
// ----------------------------------------------------------------------------------------
TInt CTcFileScan::FilesInDirectoryL(RFs& aFs, TDesC &aDirectory)
    {
	RDEBUG("CTcFileScan::FilesInDirectoryL");

    TInt retVal = 0;

    TFindFile* fileFinder = new (ELeave) TFindFile( aFs );

    CDir* fileList = NULL;
    if(KErrNone == fileFinder->FindWildByPath(aDirectory, &KNullDesC(), fileList))
        {
            retVal = fileList->Count();
            delete fileList;
        }

    delete fileFinder;
    
    return retVal;
    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::CreateScanPathNameL
// @return String "aDirectory\aSubDirectory\*"
// @note aDirectory may contain one extra '*' in the end
// ----------------------------------------------------------------------------------------
HBufC* CTcFileScan::CreateScanPathNameL ( const TDesC &aDirectory, const TDesC &aSubDirectory, const TDesC& aAsterisk )
    {
	RDEBUG("CTcFileScan::CreateScanPathNameL");

    TInt fullLength = aDirectory.Length() + aSubDirectory.Length() + aAsterisk.Length() + 2;

    HBufC *fullPathName = HBufC::NewL(fullLength);

    fullPathName->Des().Append(aDirectory);

    // Remove last star character '*'
    if(( fullPathName->Des().Length() > 0 ) && ( fullPathName->Des().Right(1) == _L("*") ))
        {
        fullPathName->Des().SetLength(fullPathName->Des().Length()-1);
        }

    // Add last backslash character '\'
    if(( fullPathName->Des().Length() > 0 ) && ( fullPathName->Des().Right(1) != _L("\\") ))
        {
        fullPathName->Des().Append(TChar('\\'));
        }

    fullPathName->Des().Append(aSubDirectory);

    // Add last backslash character '\'
    if(( fullPathName->Des().Length() > 0 ) && ( fullPathName->Des().Right(1) != _L("\\") ))
        {
        fullPathName->Des().Append(TChar('\\'));
        }

    fullPathName->Des().Append(aAsterisk);

    return fullPathName;
    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::GetCopyOfResults
// ----------------------------------------------------------------------------------------
HBufC8* CTcFileScan::GetCopyOfResultsL()
    {
	RDEBUG("CTcFileScan::GetCopyOfResultsL");

    if(iFileScanResults == 0)
        return 0;
    
    HBufC8* results = HBufC8::NewL( iFileScanResults->Size() );
    results->Des().Append(iFileScanResults->Ptr(0));
    return results;
    }

// ----------------------------------------------------------------------------------------
// CTcFileScan::GetDriveListL
// This method will list available local drives (letters from 'a' to 'i', including 'z')
// Drive letters from 'j' to 'y' are substed or redirected and NOT included in results
// ----------------------------------------------------------------------------------------
void CTcFileScan::GetDriveListL()
    {
	RDEBUG("CTcFileScan::GetDriveListL");

    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    TDriveList list;
    fs.DriveList( list );

    iRecurseLevel = 0;
    TInt   i;
    TUint8 c = 'a';
        
    for(i= 0; i<sizeof(TDriveList) && c<'i'; i++, c++)
        {
        if( list[i] != 0 )
            {
            TBuf<10> driveName;
            driveName.Append(TChar(c));
            AddDriveToResultsL( driveName );
            }
        }
        
    // Look for drive 'z'
    if( list['z' - 'a'] != 0 )
        {
        TBuf<10> driveName;
        driveName.Append(TChar('z'));
        AddDriveToResultsL( driveName );
        }
        
    CleanupStack::PopAndDestroy( &fs );       
    }

// ----------------------------------------------------------------------------------------
// End of file
