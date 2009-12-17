/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles storing of update pkg
*
*/


#ifndef __FOTASTORAGE_H_
#define __FOTASTORAGE_H_

#include <s32strm.h>


/*
* Handles all package storing and space reservation functionality.
* This class is used by fotaserver and rfs.
*
*/
class CFotaStorage : public CBase
{

    public:

    enum TFreeSpace
        {
        EFitsToReservation,     // fits to space reservation
        EFitsToFileSystem,      // doesn't fit to reservation,fits to FS
        EDoesntFitToFileSystem  // doesn't fit to fs
        };

        // This func is used to reserve storage space for future packages
        virtual TInt AdjustDummyStorageL() = 0;

        // This function is used to get the filestream pointer for writing the package.
        virtual TInt OpenUpdatePackageStoreL(const TInt aPkgId,TInt aNewDummySize
                                            , RWriteStream*& aPkgStore) = 0;

		// This function is used to get the downloaded update package size.
		virtual	void GetDownloadedUpdatePackageSizeL (const TInt aPkgId, TInt& aSize ) = 0;
        virtual void UpdatePackageDownloadCompleteL(const TInt aPkgId) = 0;

        // Get update package location. 
        virtual void GetUpdatePackageLocationL(const TInt aPkgId
                                                        , TDes8& aPath ) = 0;

        virtual void GetUpdatePackageIdsL(TDes16& aPackageIdList) = 0;

        // does the pkg fit to storage?
        virtual TFreeSpace IsPackageStoreSizeAvailableL (TInt& aSize) = 0;

        virtual void  DeleteUpdatePackageL ( const TInt aPkgId ) = 0;

    protected:

        
    public:

        // Bytes written to store, client must update
        TInt    iBytesWritten;

        TUid    iDtor_ID_Key;

};
#endif //__FOTASTORAGE_H_
