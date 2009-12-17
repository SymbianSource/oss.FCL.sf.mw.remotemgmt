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


#ifndef __TERMINALCONTROL_SERVER_H__
#define __TERMINALCONTROL_SERVER_H__


// INCLUDES

#include <e32base.h>
#include "TerminalControlClient.h"
#include <SCPClient.h>


// CONSTANTS

// ----------------------------------------------------------------------------------------
// TerminalControl server panic codes
// ----------------------------------------------------------------------------------------
enum TTerminalControlPanic
	{
	EPanicGeneral,
	EBadSubsessionHandle,
	EPanicIllegalFunction,
	EBadDescriptor
	};


// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES

void PanicClient(const RMessagePtr2& aMessage,TTerminalControlPanic aPanic);

// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  CTerminalControlServer
*  Description.
*/

class CTerminalControlServer : public CPolicyServer
	{
	friend class CTerminalControlSession;

public:
    class TTcProcessInfo
        {
        public:
            TFullName           iProcessName;
            TFileName           iFileName;
            TInt                iHandle;
            TProcessId          iId;
            TModuleMemoryInfo   iMemoryInfo;
            TBool               iCodeInRom;
            TSecureId           iSecureId;
            TBool               iProtected;
        };

public:
	~CTerminalControlServer();	

	static CServer2*    NewLC                   ( );
	void                AddSession              ( );
	void                DropSession             ( );
	CObjectCon*         NewContainerL           ( );	
	void                Panic                   ( TInt aPanicCode );

private:
	CTerminalControlServer();

	void                ConstructL              ();
	CSession2*          NewSessionL             (const TVersion& aVersion,
	                                             const RMessage2& aMessage) const;

	//From CPolicyServer
	CPolicyServer::TCustomResult
	                    CustomSecurityCheckL    (const RMessage2& aMsg,
	                                             TInt& aAction,
	                                             TSecurityInfo& aMissing);

    TTcProcessInfo      FindLocalProcessInfoL   ( const TDesC8 &aProcessName );

protected:
    //
    // Services
    //
	void                DeleteFileL             ( const TDesC8 &aFileName );
    void                WipeDeviceL             ( );
    void                DeepFactoryResetL       ( );
    void                RebootDeviceL            ( );
    CBufFlat*           GetRunningProcessesL    ( );
	CBufFlat*           GetProcessDataL         ( const TDesC8 &aProcessName, TDesC8 &aDataName );	
	TInt                GetLockLevelL           ( );
	TInt                GetLockTimeoutL         ( );
	TInt                GetLockMaxTimeoutL      ( );
	void                SetLockLevelL           ( TInt aValue );
	void                SetLockTimeoutL         ( TInt aValue );
	void                SetLockMaxTimeoutL      ( TInt aValue );
	void                SetLockCodeL            ( TDesC8& aCode );
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
	void 				SetPasscodeMinLengthL	( TInt aMinLength );
	TInt				GetPasscodeMinLengthL	();
	void 				SetPasscodeMaxLengthL	( TInt aMaxLength );
	TInt				GetPasscodeMaxLengthL	();	
	void 				SetPasscodeRequireUpperAndLowerL	( TInt aRequire );
	TInt				GetPasscodeRequireUpperAndLowerL	();	
	void 				SetPasscodeRequireCharsAndNumbersL	( TInt aRequire );
	TInt				GetPasscodeRequireCharsAndNumbersL	();
	void 				SetPasscodeMaxRepeatedCharactersL	( TInt aMax );
	TInt				GetPasscodeMaxRepeatedCharactersL	();
	void 				SetPasscodeHistoryBufferL	( TInt aBufferLength );
	TInt				GetPasscodeHistoryBufferL	();
	void 				SetPasscodeExpirationL	( TInt aExpiration );
	TInt				GetPasscodeExpirationL	();
	void 				SetPasscodeMinChangeToleranceL	( TInt aTolerance );
	TInt				GetPasscodeMinChangeToleranceL	();
	void 				SetPasscodeMinChangeIntervalL	( TInt aInterval );
	TInt				GetPasscodeMinChangeIntervalL	();
	void 				SetPasscodeCheckSpecificStringsL	( TInt aChoice );
	TInt				GetPasscodeCheckSpecificStringsL	();
	void				DisallowSpecificPasscodesL( const TDesC8& aString );
	void				AllowSpecificPasscodesL( const TDesC8& aString );
	void 				ClearSpecificPasscodeStringsL();
	void 				SetPasscodeMaxAttemptsL	( TInt aAttempts );
	TInt				GetPasscodeMaxAttemptsL	();
	void 				SetConsecutiveNumbersL	( TInt aConsecutiveNumbers );
	TInt				GetConsecutiveNumbersL	();
    void                SetPasscodeMinSpecialCharactersL   ( TInt aMin );
    TInt                GetPasscodeMinSpecialCharactersL   ();
    void                SetDisallowSimpleL  ( TInt aDisallowSimple );
    TInt                GetDisallowSimpleL  ();
	
// ----- enhanced features END ----------------
//#endif
	void                StartProcessByUidL      ( const TUid& aUID );
	void                StartProcessByUidL      ( const TDesC8& aUID );
	void                StartProcessByFullNameL ( const TDesC8& aName );
	void                StopProcessByUidL       ( const TUid& aUID );
    void                StopProcessByUidL       ( const TDesC8 &aUID );
	void                StopProcessByFullNameL  ( const TDesC8& aName );
    TBool               GetPasscodePolicyL      ( );
    void                SetPasscodePolicyL      ( TBool aIsSet );

private:
	void 				SetIntValueL( TInt avalue, TInt aSCPCommandEnum );
	TInt				GetIntValueL( TInt aSCPCommandEnum );
private:
	TInt            iSessionCount;
	CObjectConIx    *iContainerIndex;

	// Server policy
	#define TC_NUMBER_OF_POLICIES 3
	static const CPolicyServer::TPolicy         iTcConnectionPolicy;
    static const TInt                           iTcRanges         [TC_NUMBER_OF_POLICIES];
    static const TUint8                         iTcElementsIndex  [TC_NUMBER_OF_POLICIES];
    static const CPolicyServer::TPolicyElement  iTcElements[TC_NUMBER_OF_POLICIES];

    CArrayFix<TTcProcessInfo>*                  iProcessInfoArray;

    RSCPClient iSCPClient;
	};

#endif //__TERMINALCONTROL_SERVER_H__
