/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   stores update package to filesystem
*
*/



// INCLUDE FILES
#include <ecom.h>
#include <implementationproxy.h>
#include <centralrepository.h>
#include <sysutil.h> 
#include "fotadiskstoragePrivateCRKeys.h"
#include "fotaDiskStorage.h"
#include "fotadebug.h"

// CONSTANTS
// System Critical Level (128KB) plus 5KB for fota operations.
const TInt KSystemCriticalWorkingspace = 136192;

// MACROS
#ifdef __EABI__
#ifndef IMPLEMENTATION_PROXY_ENTRY
typedef TAny*   TProxyNewLPtr;
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr) {{aUid},(TProxyNewLPtr)(aFuncPtr)}
#endif
#endif

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CFotaDiskStorage::CFotaDiskStorage()
// ---------------------------------------------------------------------------
//
CFotaDiskStorage::CFotaDiskStorage ()
    {
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::NewL()
// ---------------------------------------------------------------------------
//
CFotaDiskStorage* CFotaDiskStorage::NewL()
    {
    FLOG(_L("CFotaDiskStorage::NewL()"));
    CFotaDiskStorage* self = new ( ELeave ) CFotaDiskStorage;
    FLOG(_L("  CFotaDiskStorage created at %X "), self);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::ConstructL() 
// ---------------------------------------------------------------------------
//
void CFotaDiskStorage::ConstructL() 
    {
    FLOG(_L("CFotaDiskStorage::ConstructL() >>"));
    TInt err;
    User::LeaveIfError ( iFs.Connect() );

    // Ensures that fotaserver private dir exists
    err = iFs.MkDirAll(KDummyFilePath);
    if ( err!=KErrAlreadyExists && err != KErrNone )
        {
        FLOG(_L(" created priv dir err %d"),err);
        User::Leave ( err ) ;
        }
    FLOG(_L("CFotaDiskStorage::ConstructL()  sessionpath to %S")
                        , &KDummyFilePath,err);
    User::LeaveIfError ( iFs.SetSessionPath ( KDummyFilePath ) );

    // newdummy remains should not exist now. safety.
    err = iFs.Delete ( _L("newdummy") ) ;
    if ( err!=KErrNone && err!= KErrNotFound ) User::Leave (err);

    // Get write limit 
    TInt writelimit( KFileReservationDefaultSize );
    TInt chunksize ( iChunkSize );
    CRepository* centrep( NULL);
    TRAP(err, centrep = CRepository::NewL( KCRUidFotaDiskStorage ) );
    if ( centrep ) 
        {
        err = centrep->Get( KFotaDiskSpaceReservationKey, writelimit );
        err = centrep->Get( KFotaDiskSpaceReservationChunkKey, chunksize );
        }
    iDummySize = writelimit;
    iChunkSize = chunksize;
    delete centrep;

    AdjustDummyStorageL();
    FLOG(_L("CFotaDiskStorage::ConstructL() <<"));
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::~CFotaDiskStorage ()
// ---------------------------------------------------------------------------
//
CFotaDiskStorage::~CFotaDiskStorage ()
    {
    FLOG(_L("CFotaDiskStorage::~CFotaDiskStorage ()"));
    iFileWriteStream.Close();
    iFs.Close();
    }


// ---------------------------------------------------------------------------
// CFotaDiskStorage::IsPackageStoreSizeAvailableL
// does the pkg fit to reservation or filesystem
// ---------------------------------------------------------------------------
//
CFotaStorage::TFreeSpace CFotaDiskStorage::IsPackageStoreSizeAvailableL(TInt& aSize)
    {
    CFotaStorage::TFreeSpace isavailable;
    TInt            swupdSize(0);
    TInt            dummySize(0);
    SpaceAllocatedBySWUPDFilesL( swupdSize, dummySize);

    if ( aSize <= dummySize ) 
        {        
        // fits to reservation
        isavailable = CFotaStorage::EFitsToReservation;
        }
    else
        {
        // doesnt fit to reservation, does it fit to filesystem?
        TInt sizeNeededFromFS = aSize - dummySize ;
        if ( sizeNeededFromFS < 0 ) sizeNeededFromFS = 0;
        TBool critical = SysUtil::FFSSpaceBelowCriticalLevelL( &iFs, sizeNeededFromFS );
        if ( critical )
        	{
					// how much space would be needed
					TVolumeInfo vi;
        	iFs.Volume(vi,EDriveC);
        	
					TInt neededspace = sizeNeededFromFS - vi.iFree + KSystemCriticalWorkingspace;
					FLOG(_L("neededspace = %d vi.iFree = %d "), neededspace , vi.iFree);		
					FLOG(_L(" neededspace = sizeNeededFromFS - vi.iFree + KSystemCriticalWorkingspace;") );
        	//= aSize - vi.iFree;
        	aSize = neededspace;
        	isavailable = CFotaStorage::EDoesntFitToFileSystem;
        	}
        else
        	{
					isavailable = CFotaStorage::EFitsToFileSystem;
        	}
        }
    FLOG(_L("CFotaDiskStorage::IsPackageStoreSizeAvailableL %d<%d (sz vs dummy) => fitstodummy:%d fitstoFS:%d DoesntFitToFS:%d")
        ,aSize,dummySize,isavailable==CFotaStorage::EFitsToReservation?1:0,isavailable==CFotaStorage::EFitsToFileSystem?1:0,isavailable==CFotaStorage::EDoesntFitToFileSystem?1:0
       );

    return isavailable;
    }


// ---------------------------------------------------------------------------
// CFotaDiskStorage::AdjustDummyStorageL()
// Ensure that total of iDummySize bytes are reserved by .swupd files and 
// dummy file.
// ---------------------------------------------------------------------------
//
TInt CFotaDiskStorage::AdjustDummyStorageL  ( )
    {
    FLOG(_L("CFotaDiskStorage::AdjustDummyStorageL >>"));
    // Count size reserved by .swupd files
    // CDir*       list;
    TInt        err;
    TInt        swupdSize(0);
    TInt        dummySize(0);
    RFile       dummy;

    TRAP(err,SpaceAllocatedBySWUPDFilesL( swupdSize, dummySize));

    // Calculate space for dummy file
    TInt targetsize = iDummySize - swupdSize;
    if ( targetsize<0 ) 
        {
        targetsize=0;
        }

    // Reduce dummy file size 
    if ( dummySize != targetsize || dummySize ==0 ) 
        {
        FLOG(_L("   dummy new size %d (old %d)"), targetsize,dummySize);

        err = dummy.Open(iFs, KDummyFileName, EFileWrite|EFileShareExclusive);

        if (err == KErrNotFound ) 
            {
            User::LeaveIfError(dummy.Replace(iFs, KDummyFileName
                                , EFileWrite|EFileShareExclusive)); 
            }
        else
            if ( err!=KErrNone) User::LeaveIfError (err);

        CleanupClosePushL(dummy);
        TInt err= dummy.SetSize (targetsize);
        if (err!=KErrNone)
        	{
        	FLOG(_L("Error while creating reserved space:  %d "),err );
        	}
        CleanupStack::PopAndDestroy(1); // dummy
        }
    FLOG(_L("CFotaDiskStorage::AdjustDummyStorageL, reserved file size = %d <<"),targetsize);
    return 0;
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::OpenUpdatePackageStore
// Open upd pkg store for writing (writes to dummy file).
// ---------------------------------------------------------------------------
TInt CFotaDiskStorage::OpenUpdatePackageStoreL(const TInt aPkgId,TInt aNewDummySize
                                                , RWriteStream*& aPkgStore)
	{
    FLOG(_L("CFotaDiskStorage::OpenUpdatePackageStore"));
    TInt        err (KErrNone);

    // Remove reserved memory
    RFile tmp;
    err = tmp.Open(iFs, KDummyFileName, EFileWrite|EFileShareExclusive);
    CleanupClosePushL(tmp);
    if ( !err )
    	{
        FLOG(_L("Removing the reserved memory as download has started"), iDummySize);
        tmp.SetSize( KErrNone );
        }
    CleanupStack::PopAndDestroy( &tmp ); 
	// flexible mem handling: increase dummy size to receive over sized package
    if (iDummySize < aNewDummySize)
    	iDummySize = aNewDummySize;
    FLOG(_L("Newer dummy size = %d"),iDummySize);

    TBuf<KMaxFileName>    swupd;

    swupd.AppendNum(aPkgId);
    swupd.Append(_L(".swupd"));
    err = iFileWriteStream.Open(iFs, swupd, EFileWrite) ;
    if (err == KErrNotFound ) 
        {
        User::LeaveIfError(iFileWriteStream.Replace(iFs, swupd
                                                            , EFileWrite));
        }
    else
        if ( err!=KErrNone) User::LeaveIfError (err);

    //Positioning the seek if the file is already present (in case of resume)
	TEntry entry;
	TInt size (KErrNone);
	err = iFs.Entry(swupd,entry);
	if (!err)
		size = entry.iSize; 
	if (size)
		{
	    MStreamBuf* x = iFileWriteStream.Sink();
	    TStreamPos pos(0);
	    TRAPD(err2, pos = x->TellL(MStreamBuf::EWrite));
	    
	    pos+=size;
	    TRAP(err2, x->SeekL(MStreamBuf::EWrite,pos ));
		}
    aPkgStore = &iFileWriteStream;
    return err;
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::GetDownloadedUpdatePackageSizeL
// Gets the downloaded update package size in bytes
// ---------------------------------------------------------------------------

void CFotaDiskStorage::GetDownloadedUpdatePackageSizeL(const TInt aPkgId, TInt& aSize)
	{
	FLOG(_L("CFotaDiskStorage::GetDownloadedUpdatePackageSizeL >>"));
	aSize = 0;

    TBuf<KMaxFileName>    swupd;

    swupd.AppendNum(aPkgId);
    swupd.Append(_L(".swupd"));

	TInt err(KErrNone);
	TEntry entry;
	err = iFs.Entry(swupd,entry);
	if (!err)
	aSize = entry.iSize; 
	FLOG(_L("CFotaDiskStorage::GetDownloadedUpdatePackageSizeL,err = %d, aSize = %d <<"),err, aSize);
	}


// ---------------------------------------------------------------------------
// CFotaDiskStorage::UpdatePackageDownloadComplete
// closes the stream and frees resources
// ---------------------------------------------------------------------------
void CFotaDiskStorage::UpdatePackageDownloadCompleteL(const TInt aPkgId)
    {
    FLOG(_L("CFotaDiskStorage::UpdatePackageDownloadComplete(const TInt aPkgId)"));
    RFile       fswupd;
    TBuf<KMaxFileName>    fn;
    TInt        err;
    iFileWriteStream.Close();
    if(iBytesWritten<1) 
        {
        FLOG(_L("  no bytes received!"));
//        User::Leave(KErrNotFound);
        return;
        }
    TBuf<KMaxFileName>    swupd;

    swupd.AppendNum(aPkgId);
    swupd.Append(_L(".swupd"));

    // open swupd file for reading
    err = fswupd.Open(iFs, swupd, EFileWrite|EFileShareExclusive);
    FLOG(_L("  open err %d"),err);
    if (err == KErrNotFound ) 
        {
        FLOG(_L("swupd not found, creaeting"));
        User::LeaveIfError(fswupd.Replace(iFs, swupd
                                        ,EFileWrite|EFileShareExclusive)); 
        }
    else
        if ( err!=KErrNone) User::LeaveIfError (err);
    CleanupClosePushL(fswupd);

    RFile       ND;
    User::LeaveIfError ( ND.Replace ( iFs, KDummyFileName, EFileWrite ) );
    CleanupClosePushL  ( ND);
    CleanupStack::PopAndDestroy(2); // dummies
    AdjustDummyStorageL();
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::GetUpdatePackageLocation
// Gets update package location, that is , path.
// ---------------------------------------------------------------------------
void CFotaDiskStorage::GetUpdatePackageLocationL(const TInt aPkgId
                                                        , TDes8& aPath )
{
    TBuf8<20> fn;
    fn.AppendNum(aPkgId);
    fn.Append(_L8(".swupd"));
    TInt pathlength = ((TDesC16)KDummyFilePath).Length();
    HBufC8* path = HBufC8::NewLC( pathlength );
    path->Des().Copy( KDummyFilePath );

    aPath.Append( path->Des() );
    aPath.Append(fn);
    CleanupStack::PopAndDestroy( path );
}

// ---------------------------------------------------------------------------
// CFotaDiskStorage::GetUpdatePackageIds
// getupdate package ids
// ---------------------------------------------------------------------------
void CFotaDiskStorage::GetUpdatePackageIdsL(TDes16& aPackageIdList)
    {
    FLOG(_L("CFotaDiskStorage::GetUpdatePackageIds"));
    // Read all .swupd files and parse pkg ids from filenames
    TInt err;
    CDir*   list;
    err=iFs.GetDir (_L("*.swupd"), KEntryAttNormal ,ESortByName, list  );
    for(int i=0; i<list->Count() ;++i )
        {
        TEntry t = (*list)[i];
        TParse  p;
        TInt16  pkgid;
        p.Set(t.iName,NULL,NULL);
        TLex    lex(p.Name());
        err = lex.Val(pkgid);
        FLOG(_L("   %S"),&t.iName);
        if(err==KErrNone)
            {
            TPtrC  filename(p.Name());
            FLOG(_L("  pkig found: %d"), pkgid);
            TDateTime d = t.iModified.DateTime();
            aPackageIdList.Append (pkgid);   
            }
        }
    delete list;
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::DeleteUpdatePackageL 
// ---------------------------------------------------------------------------
void  CFotaDiskStorage::DeleteUpdatePackageL (const TInt aPkgId)
    {
    FLOG(_L("CFotaDiskStorage::DeleteUpdatePackageL %d >>"),aPkgId);
    RFile       dummy;
    TInt err=    dummy.Open(iFs, KDummyFileName , EFileWrite|EFileShareExclusive);
    FLOG(_L("Error opening the reserved file...%d"),err);
    CleanupClosePushL ( dummy );
    TRAP(err, DoDeleteUpdatePackageL ( dummy, aPkgId, 0 ));
    CleanupStack::PopAndDestroy(1);
    AdjustDummyStorageL();

    FLOG(_L("CFotaDiskStorage::DeleteUpdatePackageL %d, err = %d <<"),aPkgId, err);
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::DoDeleteUpdatePackageL 
// Delete swupd by chunking data to dummy (param) file. Will grow dummy 
// independently, but takes already allocated bytes into account.
// ---------------------------------------------------------------------------
void  CFotaDiskStorage::DoDeleteUpdatePackageL ( RFile& dummy, TInt aPkgId
                                                    , TInt aAlreadyAllocated)
    {
    // Open swupd file 
    TInt      err;
    TBuf8<KMaxFileName> swupdpath;
    TBuf<KMaxFileName>  swupdpath16;
    GetUpdatePackageLocationL ( aPkgId, swupdpath) ;
    swupdpath16.Copy ( swupdpath );
    RFile       swupd;
    err = swupd.Open( iFs, swupdpath16, EFileWrite );
    if ( err == KErrNotFound )      return; // no need to delete
    if ( err != KErrNone )          User::Leave ( err );
    CleanupClosePushL ( swupd );

    // Reduce dummy size
    TInt dummytargetsize = iDummySize - aAlreadyAllocated;
    for ( TInt p=0; p<10000 ; ++p )
        {
        TInt        dsize;
        TInt        swupdsize(0);
        User::LeaveIfError( dummy.Size( dsize ) );
        User::LeaveIfError( swupd.Size( swupdsize) );
        TInt        chunk =  swupdsize > iChunkSize ? iChunkSize : swupdsize;

        // Ensure that dummy dosnt get oversized
        if ( dsize <= dummytargetsize  && dsize + chunk >= dummytargetsize ) 
                    chunk = dummytargetsize - dsize;

        // Safety
        if ( dsize >= dummytargetsize ) break;

        FLOG(_L("  deleting swupd:  dummy %d\t  swupd %d\t chunk%d"),dsize
                                                ,swupdsize,chunk);
        if (chunk>0)
            {
            User::LeaveIfError( dummy.SetSize( dsize + chunk ) );
            User::LeaveIfError( swupd.SetSize( swupdsize - chunk ) );
            }
        else 
            break;
        }
    CleanupStack::PopAndDestroy(1); // swupd 

    // Delete swupd  (dummy file is big enough now)
    err = iFs.Delete ( swupdpath16 ) ;
    FLOG(_L("CFotaDiskStorage::DoDeleteUpdatePackageL deleted ,err %d"),err);
    if ( err != KErrNone && err != KErrNotFound ) 
        {
        User::Leave ( err );
        }
    }

// ---------------------------------------------------------------------------
// CFotaDiskStorage::SpaceAllocatedBySWUPDFilesL
// Counts space allocated by swupd files
// ---------------------------------------------------------------------------
void CFotaDiskStorage::SpaceAllocatedBySWUPDFilesL( TInt& aSwupdSize, TInt& aDummySize )
    {
    CDir*       list;
    TInt        err;
    // TInt        (0);
    aSwupdSize = 0;
    aDummySize = 0;
    err = iFs.GetDir (_L("*.swupd"), KEntryAttNormal ,ESortByName, list );
    User::LeaveIfError(err);
    CleanupStack::PushL ( list );
    
    // get sizes of swupd files
    for(int i=0; i<list->Count() ;++i )
        {
        TEntry t = (*list)[i];
        TParse  p;
        TInt16  pkgid;
        
        p.Set(t.iName,NULL,NULL);
        TLex    lex(p.Name());
        err = lex.Val(pkgid);
        if(err==KErrNone)
            {
            TPtrC  filename(p.Name());
            aSwupdSize += t.iSize;
            }
        }
    CleanupStack::PopAndDestroy( list );

    // get size of dummyfile
    err = iFs.GetDir (KDummyFileName, KEntryAttNormal ,ESortByName, list );
    User::LeaveIfError(err);
    CleanupStack::PushL ( list );
    if ( list->Count() >0 )
        {
        TEntry t = (*list)[0];
        aDummySize = t.iSize;
        }
    CleanupStack::PopAndDestroy( list );
    FLOG(_L("CFotaDiskStorage::SpaceAllocatedBySWUPDFilesL dummy:%d swupd:%d")
                        ,aDummySize,aSwupdSize);
    }        

// ---------------------------------------------------------------------------
// Global implementation uid array
// Define the Implementation UIDs for JP2K decoder.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
{
    // implementation_uid
    IMPLEMENTATION_PROXY_ENTRY( 0x10207385 , CFotaDiskStorage::NewL )
};

// ========================== OTHER EXPORTED FUNCTIONS =========================

// ---------------------------------------------------------------------------
// ImplementationGroupProxy Implements proxy interface for ECom
// Exported proxy for instantiation method resolution.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(  //lint !e714 Used by ECom
    TInt& aTableCount ) // Number of tables 
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( 
                                                        TImplementationProxy );
    return ImplementationTable;
    }
