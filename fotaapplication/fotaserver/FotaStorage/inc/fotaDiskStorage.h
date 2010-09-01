/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of fotaserver component
* 	This is part of fotaapplication.
*
*/


#ifndef __FOTADISKSTORAGE_H_
#define __FOTADISKSTORAGE_H_

#include <s32file.h>


#include "fotastorage.h"


// CONSTANTTS
// const TInt KDummyFileSize(2000000);   // total of 2MB should be reserved for fota pkgs

#ifndef _FOTA_UNIT_TESTER
_LIT( KDummyFilePath, "c:\\private\\102072C4\\" );
#else
_LIT( KDummyFilePath, "c:\\private\\10009cf4\\" );
#endif 
_LIT( KDummyFileName, "reserved");  // dummy file name 


const TInt KFileReservationDefaultSize ( 5000000);
const TInt KFileChunkDefaultSize ( 120000 );

/*
* Implementation of fotastorage. This implementation
* directly stores the update package to user disk. 
* Private directory of the calling process will be used
* for storage.
*/

class CFotaDiskStorage : public CFotaStorage
{
    public:

    /**
     * NewL
     * @since   S60   v3.1
     * @param   none
     * @return  New instance
     */
      static CFotaDiskStorage* NewL();


    /**
     * Two phase Constructor
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
		void ConstructL();
     
    // From base classes

    /**
     * Two phase Constructor
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
		TInt AdjustDummyStorageL();

    /**
     * Opens stream to package file
     * @since   S60   v3.1
     * @param   pkgid	Package id
     * @param   aPkgStore	Stream to the file
     * @return  err
     */
    TInt OpenUpdatePackageStoreL(const TInt aPkgId,TInt aNewDummySize
                                  , RWriteStream*& aPkgStore);

    /**
     * Gets the update package size
     * @since   S60   v3.2
     * @param   aPkgid	Package id
     * @param   aSize	Size in bytes
     * @return  None
     */

     void GetDownloadedUpdatePackageSizeL(const TInt aPkgid, TInt& aSize);


    /**
     * Closes stream to package file
     * @since   S60   v3.1
     * @param   pkgid	Package id
     * @return  none
     */
		void UpdatePackageDownloadCompleteL(const TInt aPkgId);

    /**
     * Gets path to package
     * @since   S60   v3.1
     * @param   pkgid	Package id
     * @param   aPath	path to file
     * @return  none
     */
		void GetUpdatePackageLocationL(const TInt aPkgId, TDes8& aPath );


    /**
     * Gets present packages
     * @since   S60   v3.1
     * @param   aPackageIdList	List of package ids
     * @return  none
     */
		void GetUpdatePackageIdsL(TDes16& aPackageIdList);


    /**
     * Checks if there is space for pkg. If not (doesnt fit to filesystem)
     * returns needed space in aSize.
     * @since   S60   v3.1
     * @param   aSize		Space of pkg. On return, needed space.
     * @return  Whether there's free space
     */
		TFreeSpace IsPackageStoreSizeAvailableL(TInt& aSize);


    /**
     * Deletes update package
     * returns needed space in aSize.
     * @since   S60   v3.1
     * @param   apPkgid Package id
     * @return  none
     */
		void  DeleteUpdatePackageL (const TInt aPkgId);



    /**
     * Destructor
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
		virtual ~CFotaDiskStorage ();
		
public:


    /**
     * Stream to pkg file
     */
		RFileWriteStream    iFileWriteStream;


    /**
     * File server session
     */
		RFs                 iFs;

    /**
     * Size of reservation file. Configured via cenrep.
     */
		TInt                iDummySize;



    /**
     * Size of chunk used to separate update package file content from
     * reservation file.
     */
		TInt                iChunkSize;

private: // new functions


    /**
     * Constructor
     * @since   S60   v3.1
     * @param   none
     * @return  none
     */
    CFotaDiskStorage ();


    /**
     * Deletes the update package so that total space reservation file
     * stays the same (ie 5MB). 
     * @since   S60   v3.1
     * @param 	dummy Handle to reservation file
     * @param   aPkgId package file to be deleted
     * @param   aAlreadyAllocated Already allocated bytes by other pkg files
     * @return  none
     */
    void DoDeleteUpdatePackageL ( RFile& dummy, TInt aPkgId
                                      , TInt aAlreadyAllocated=0);


    /**
     * Checks how much space is allocated by package files
     * @since   S60   v3.1
     * @param 	aSwupdSize Size allocated by package files
     * @param   aDummySize Size allocated by reservation file
     * @return  none
     */
     void SpaceAllocatedBySWUPDFilesL( TInt& aSwupdSize, TInt& aDummySize);

};


#endif //__FOTADISKSTORAGE_H_
