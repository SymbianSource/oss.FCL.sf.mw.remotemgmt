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


#ifndef _TC_FILESCAN_HEADER_
#define _TC_FILESCAN_HEADER_

// INCLUDES
#include <e32base.h>
#include <f32file.h>


class CTcFileScan : public CBase
{
public: 
		
		static CTcFileScan* NewL();
		~CTcFileScan();
   

	void                FileScanL                   ( const TDesC8 &aFileName,
	                                                  TBool aRecursive );

    HBufC8*             GetCopyOfResultsL           ( );

private:
	CTcFileScan();

private:
	void                ScanDirectoryL              ( RFs& aFileSession,
	                                                  const TDesC &aDirectory,
	                                                  TBool aRecursive );
	                                                  	                                                  
	void                BeginAddFileToResultsL      ( const TDesC &aFileName, TBool aIsDir, TInt aFiles );
	
	void                EndAddFileToResultsL        ( TBool aIsDir, TInt aFiles );
	
    void                AddDriveToResultsL          ( const TDesC& aDriveName );
	
    TInt                FilesInDirectoryL           ( RFs& aFs, TDesC &aDirectory );
    
    HBufC*              CreateScanPathNameL         ( const TDesC &aDirectory,
                                                      const TDesC &aSubDirectory,
                                                      const TDesC& aAsterisk );
   
    void                GetDriveListL               () ;

private:
    CBufFlat*           iFileScanResults;
    TInt                iRecurseLevel;
	TBuf8<20>           iTabString;
};

#endif //_TC_FILESCAN_HEADER_
