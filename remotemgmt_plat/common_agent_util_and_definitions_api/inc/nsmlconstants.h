/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constants for SyncML component
*
*/


#ifndef __NSMLCONSTANTS_H
#define __NSMLCONSTANTS_H

#include <e32base.h>
#include <e32cmn.h> 
#include "nsmlconstantdefs.h"


const TUid KNSmlSOSServerPolicyUID = { 0x101F99FB };
const TUid KNSmlDMHostOnePolicyUID = { 0x101F9A02 };
//DM constants to handle the MO/MT calls in minutes .
//Minimum session timeout value when session gets interrupts due to 
//call is 1 minute and maximum 15 minutes
const TInt KNSmlDMMinSessionTimeout = 1;
const TInt KNSmlDMMaxSessionTimeout = 15;

//Enums for session type 
enum TNsmlSessionTypes
    {
    ESyncMLSessionUnknown,
    ESyncMLDSSession,
    ESyncMLDMSession,
    };
#ifdef SYMBIAN_SECURE_DBMS
// Define SID for databases
_LIT( KNSmlDBMSSecureSOSServerID,   "secure[101F99FB]" );
_LIT( KNSmlDBMSSecureDSHostRWID,    "secure[101F99FD]" );
_LIT( KNSmlDBMSSecureDSHostNULLID,  "secure[101F99FE]" );
_LIT( KNSmlDBMSSecureMDHostOneID,   "secure[101F9A02]" );
#else
// Define SID for databases
_LIT( KNSmlDBMSSecureSOSServerID,   "" );
_LIT( KNSmlDBMSSecureDSHostRWID,    "" );
_LIT( KNSmlDBMSSecureDSHostNULLID,  "" );
_LIT( KNSmlDBMSSecureMDHostOneID,   "" );
// Database location when SYMBIAN_SECURE_DBMS flag is not used
_LIT( KNSmlDatabasesNonSecurePath, "\\" );
#endif 


//Medium type definitions
const TUid KUidNSmlMediumTypeInternet = { 0x101F99F0 };
const TUid KUidNSmlMediumTypeBluetooth = { 0x101F99F1 };
const TUid KUidNSmlMediumTypeUSB = { 0x101F99F2 };
const TUid KUidNSmlMediumTypeIrDA = { 0x101F99F3 };

// Used for deriving the IapId for DM Job(Generic alert) started by FOTA
// Enhancement to an existing DM Job creation api which takes Transport Id
// Transport Id Format is 1022xxxx ,1022 signifies xxxx as Iap Id to use
_LIT( KNSmlDMJobIapPrefix, "1022" );
// To extract 1022 from the Transport Id 1022xxxx
#define KNSmlHalfTransportIdLength 4 
// Limit of both HistoryLog entry types in history log array.
// Maximum entry count is 10.
const TInt KNSmlMaxHistoryLogEntries( 5 );

const TInt KNSmlPanicIndexOutOfBound = 1;
_LIT(KNSmlIndexOutOfBoundStr, "Array index out of bounds error");

// SYNC APP
_LIT(KNSmlMutexName, "SmlSingleInstanceMutex");
const TInt KSmlArrayGranularity = 8;

// SETTINGS
const TInt KNSmlNewObject = -1;
const TInt KMaxDataSyncID = 1000000;

//CLIENT API
const TInt KNSmlNullId = -1;

// central repository uid for MaxMsgSize
const TUid KCRUidNSmlDSEngine       = { 0x2000CF7E };

//Cenrepository that publishes ongoing data sync session details
const TUid KNsmlDsSessionInfoKey = { 0x20029F15 };
enum TDSSessfionInfo
    {
    EDSSessionProfileId,
    EDSSessionProfileName,
    EDSSessionServerId
    };


/**
* CenRep key for storing the Maximum SyncML DS Packet Size.
* 
*
* Possible integer values:
* 
* Default value: 40,000
*
*/
const TUint32 KNSmlMaxMsgSizeKey = 0x00000008;

// SMLSYNCAGENT
// Workspace buffer size used for SyncML messages. Defines also max size for item
// that can be sent to the server, i.e. single item must fit to the
// workspace buffer. Note that used buffer size can be less than the default value if
// SyncML server or WAP gateway requires smaller size. 
const TInt KNSmlDefaultWorkspaceSize( 10000 ); // remote connections
const TInt KNSmlLocalSyncWorkspaceSize( 65535 ); // local connections
// Minimum workspace buffer size for SyncML messages used by the SyncML Client 
// despite server side or WAP gateway request
const TInt KNSmlMininumWorkspaceSize( 3000 );
// Maximum object size. The value is sent to a server.
const TInt KNSmlMaxObjSize( 786432 );
// Maximum size by which maximum object size can be exceeded. 
const TInt KNSmlMaxObjSizeOverflow( 20000 );
// Minimum size of an object, which is sent in chunks from the client 
const TInt KNSmlLargeObjectMinSize( 1000 );

_LIT( KNSmlPhoneModelIdDefault, "Unknown" );

// NSMLTRANSPORT
// set value for shutdowntimer in microseconds
 const TInt KNSmlXptShutdownInterval = 2100000000;

//RD_AUTO_RESTART		
 const TInt KNSmlDSXptShutdownInterval = 120000000;
//RD_AUTO_RESTART		 

/*
	KNSmlConNumberOfRetries 
	This constant is used for retrying the Rconnection::start  
	in 2G phone when call is active
*/

const TUint KNSmlConNumberOfRetries = 35;
/*
	KNSmlConRetryInterval
	Interval in Micro seconds	
	1 minute
*/

const TInt KNSmlConRetryInterval = 60000000;

// Template class that takes care of CArrayPtr<T> class free in
// leave situations
template <class T> class _NSmlDummy
	{
public:
	static void _DoFree( TAny* ptr )
		{
		CArrayPtr<T>* arr = STATIC_CAST(CArrayPtr<T>*, ptr);
		arr->ResetAndDestroy();
		delete arr;
		}
	};

#define PtrArrCleanupItem(TYPE, PTR) TCleanupItem(&_NSmlDummy<TYPE>::_DoFree, PTR)

template <class T> class _NSmlDummyRArr
	{
public:
	static void _DoFree( TAny* ptr )
		{
		RPointerArray<T>* arr = STATIC_CAST(RPointerArray<T>*, ptr);
		arr->ResetAndDestroy();
		}
	};

#define PtrArrCleanupItemRArr(TYPE, PTR) TCleanupItem(&_NSmlDummyRArr<TYPE>::_DoFree, PTR)

// CArrayPtr cleanup pusher
template <class T>
class CleanupPtrArrayDelete
	{
public:
	inline static void PushL(T* aPtr);
private:
	static void PtrArrayDelete(TAny *aPtr);
	};

template <class T>
inline void CleanupPtrArrayDelete<T>::PushL(T* aPtr)
	{
	CleanupStack::PushL(TCleanupItem(&PtrArrayDelete,aPtr));
	}

template <class T>
void CleanupPtrArrayDelete<T>::PtrArrayDelete(TAny *aPtr)
	{
	T* ptr = STATIC_CAST(T*,aPtr);
	ptr->ResetAndDestroy();
	delete ptr;
	}

template <class T>
inline void CleanupPtrArrayPushL(T* aPtr)
	{
	CleanupPtrArrayDelete<T>::PushL(aPtr);
	}


// RPointerArray cleanup pusher
template <class T>
class CleanupRPtrArrayDelete
	{
public:
	inline static void PushL(T* aPtr);
	inline static void PushL(T& aRef);
private:
	static void RPtrArrayDelete(TAny *aPtr);
	static void LocalRPtrArrayDelete(TAny *aPtr);
	};

template <class T>
inline void CleanupRPtrArrayDelete<T>::PushL(T* aPtr)
	{
	CleanupStack::PushL(TCleanupItem(&RPtrArrayDelete,aPtr));
	}

template <class T>
inline void CleanupRPtrArrayDelete<T>::PushL(T& aRef)
    {
    CleanupStack::PushL(TCleanupItem(&LocalRPtrArrayDelete,&aRef));
    }

template <class T>
void CleanupRPtrArrayDelete<T>::RPtrArrayDelete(TAny *aPtr)
	{
	T* ptr = STATIC_CAST(T*,aPtr);
	ptr->ResetAndDestroy();
	delete ptr;
	}

template <class T>
void CleanupRPtrArrayDelete<T>::LocalRPtrArrayDelete(TAny *aPtr)
    {
    T* ptr = STATIC_CAST(T*,aPtr);
    ptr->ResetAndDestroy();
    }

template <class T>
inline void CleanupRPtrArrayPushL(T* aPtr)
	{
	CleanupRPtrArrayDelete<T>::PushL(aPtr);
	}

template <class T>
inline void CleanupRPtrArrayPushL(T& aRef)
    {
    CleanupRPtrArrayDelete<T>::PushL(aRef);
    }

// Table names
_LIT( KNSmlTableVersion, "Version" );

// Column names
_LIT( KNSmlVersionColumnMajor, "VerMajor");
_LIT( KNSmlVersionColumnMinor, "VerMinor");

// Settings
const TInt KNSmlSettingsCurrentVersionMajor = 1;
const TInt KNSmlSettingsCurrentVersionMinor = 1;

// Agentlog
const TInt KNSmlAgentLogCurrentVersionMajor = 1;
const TInt KNSmlAgentLogCurrentVersionMinor = 1;

_LIT_SECURE_ID( KNSmlSOSServerSecureID, 0x101F99FB );
_LIT_SECURE_ID( KNSmlDSHostRWServerSecureID, 0x101F99FD );
_LIT_SECURE_ID( KNSmlDSHostNULLServerSecureID, 0x101F99FE );


#endif // __NSMLCONSTANTS_H
