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
* Description:  Terminal Control DM Adapter 
*
*/



#ifndef __SMLTCADAPTER_H__
#define __SMLTCADAPTER_H__

// ---------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------
// symbian
#include <e32base.h>
#include <smldmadapter.h>
// s60
#include "TARMDmAdapter.h"
#include "TerminalControlClient.h"
#include <PolicyEngineClient.h>

#define TCADAPTER_FILESCAN_EXECUTE_LEAF

const TUint KSmlDMTCAdapterImplUid = 0x10207824;

#ifndef MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64
#define MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64 21
#endif

// The string value for true/false
_LIT8( KTCValueTrue, "true" );
_LIT8( KTCValueFalse, "false" );

// Passcode min/max length
const TInt KTCPassCodeMinLen = 4;
const TInt KTCPassCodeMaxLen = 255;

// the DDF version must be changed if any changes in 
// DDF structure ( built in DDFStructureL() function )
_LIT8( KSmlTCDDFVersion, "1.0" ); 

_LIT8( KSmlTCTextPlain, "text/plain" );
_LIT8( KSmlTCTextXml, "text/xml" );

_LIT8( KTCNodeName, "TerminalSecurity" );
_LIT8( KTCNodeDescription, 
	"This node is the root node for all terminal control functionality" );

_LIT8( KTCFileScanNodeName, "FileScan" );
_LIT8( KTCFileScanNodeDescription, 
	"The file scan feature delivers to admin a list of files or \
	folders found in device" );

#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
_LIT8( KTCFileScanExecuteNodeName, "Execute" );
_LIT8( KTCFileScanExecuteNodeDescription, 
	"This node can be used to execute file scan operation." );
#endif

_LIT8( KTCFileScanResultsNodeName, "Results" );
_LIT8( KTCFileScanResultsNodeDescription, 
	"This node will hold the scan results" );

_LIT8( KTCFileDeleteNodeName, "FileDelete" );
_LIT8( KTCFileDeleteNodeDescription, "Deletes a file or empty folder" );

_LIT8( KTCDeviceLockNodeName, "DeviceLock" );
_LIT8( KTCDeviceLockNodeDescription, 
	"The parent node for all DeviceLock related nodes" );

_LIT8( KTCAutoLockPeriodNodeName, "AutoLockPeriod" );
_LIT8( KTCAutoLockPeriodNodeDescription, 
	"AutoLockPeriod is an integer that tells after how many minutes the device \
	will be automatically locked if not used" );

_LIT8( KTCMaxAutoLockPeriodNodeName, "MaxAutoLockPeriod" );
_LIT8( KTCMaxAutoLockPeriodNodeDescription, 
	"MaxAutoLockPeriod is an integer that tells maximum value of AutoLockPeriod" );

_LIT8( KTCLockLevelNodeName, "LockLevel" );
_LIT8( KTCLockLevelNodeDescription, 
	"Lock level tells whether lock is enabled, and what type of lock it is" );

_LIT8( KTCLockCodeNodeName, "LockCode" );
_LIT8( KTCLockCodeNodeDescription, 
	"LockCode is the character sequence that is needed to open locked device" );

//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
// -------- Enhanced features BEGIN
	
_LIT8( KTCLockCodeMinLengthNodeName, "LockCodeMinLength" );
_LIT8( KTCLockCodeMinLengthNodeDescription, 
	"LockCode minimum length, 4-256 characters" );

_LIT8( KTCLockCodeMaxLengthNodeName, "LockCodeMaxLength" );
_LIT8( KTCLockCodeMaxLengthNodeDescription, 
	"LockCode maximum length, 4-256 characters" );

_LIT8( KTCLockCodeRequireUpperAndLowerNodeName, 
	"LockCodeRequireUpperAndLower" );
_LIT8( KTCLockCodeRequireUpperAndLowerNodeDescription, 
	"If enabled the LockCode must contain both upper and lower case letters" );

_LIT8( KTCLockCodeRequireCharsAndNumbersNodeName, 
	"LockCodeRequireCharsAndNumbers" );
_LIT8( KTCLockCodeRequireCharsAndNumbersNodeDescription, 
	"If enabled the LockCode must contain both chars and numbers" );

_LIT8( KTCLockCodeMaxRepeatedCharactersNodeName, 
	"LockCodeMaxRepeatedCharacters" );
_LIT8( KTCLockCodeMaxRepeatedCharactersNodeDescription, 
	"Tells how many times a single character can be used in a password. \
	Values between 0 and 4, 0 = no restriction" );

_LIT8( KTCLockCodeHistoryBufferNodeName, "LockCodeHistoryBuffer" );
_LIT8( KTCLockCodeHistoryBufferNodeDescription, 
	"History buffer for previously used lock codes. 0 = no restriction, \
	1-X = the lock code can not match the previous X lock codes" );

_LIT8( KTCLockCodeExpirationNodeName, "LockCodeExpiration" );
_LIT8( KTCLockCodeExpirationNodeDescription, 
	"The password expires after X days, 0 = no restriction, 1-365" );

_LIT8( KTCLockCodeMinChangeToleranceNodeName, "LockCodeMinChangeTolerance" );
_LIT8( KTCLockCodeMinChangeToleranceNodeDescription, 
	"The user can change the password only X times before 'minimum change \
	interval' hours has passed" );

_LIT8( KTCLockCodeMinChangeIntervalNodeName, "LockCodeMinChangeInterval" );
_LIT8( KTCLockCodeMinChangeIntervalNodeDescription, 
	"The user cannot change the password more than 'minimum change tolerance' \
	times before X hours have passed since the previous change" );

_LIT8( KTCLockCodeCheckSpecificStringsNodeName,"LockCodeCheckSpecificStrings" );
_LIT8( KTCLockCodeCheckSpecificStringsNodeDescription, 
	"0 = No restriction, 1 = The password cannot match a string in the set of \
	strings given by the next three calls" );

_LIT8( KTCLockCodeDisallowSpecificNodeName, "LockCodeDisallowSpecific" );
_LIT8( KTCLockCodeDisallowSpecificNodeDescription, 
	"Disallow the specific string/strings given. Individual strings can be \
	separated with ';'" );

_LIT8( KTCLockCodeAllowSpecificNodeName, "LockCodeAllowSpecific" );
_LIT8( KTCLockCodeAllowSpecificNodeDescription, 
	"Re-allow the specific string/strings given. \
	Individual strings can be separated with ';'" );

_LIT8( KTCLockCodeClearSpecificStringsNodeName, "LockCodeClearSpecificStrings" );
_LIT8( KTCLockCodeClearSpecificStringsNodeDescription, 
	"Clear the specific string -buffer" );

_LIT8( KTCLockCodeMaxAttemptsNodeName, "LockCodeMaxAttempts" );
_LIT8( KTCLockCodeMaxAttemptsNodeDescription, 
	"0 = No restriction, 3-100 = The device is 'hard' reset after the user has \
	consecutively failed X times to answer the password query" );

_LIT8( KTCLockCodeConsecutiveNumbersNodeName, "LockCodeConsecutiveNumbers" );
_LIT8( KTCLockCodeConsecutiveNumbersNodeDescription, 
 "0 = No restriction, 1 = The password cannot contain two consecutive numbers" );

_LIT8( KTCLockCodeMinSpecialCharactersNodeName, 
    "LockCodeMinSpecialCharacters" );
_LIT8( KTCLockCodeMinSpecialCharactersNodeDescription, 
    "Tells the least number of special characters that must be present in the lock code. \
    Values between 0 and 255, 0 = no restriction" );

_LIT8( KTCLockCodeDisallowSimpleNodeName, "LockCodeDisallowSimple" );
_LIT8( KTCLockCodeDisallowSimpleNodeDescription, 
 "0 = No restriction, 1 = The password cannot be a simple string" );



// -------- Enhanced features END
//#endif

_LIT8( KTCDeviceWipeNodeName, "DeviceWipe" );
_LIT8( KTCDeviceWipeNodeDescription, 
	"This node is the root node for all terminal control functionality" );

_LIT8( KTCLocalOperationsNodeName, "LocalOperations" );
_LIT8( KTCLocalOperationsNodeDescription, 
	"This node is common parent for all local operations" );

_LIT8( KTCStartNodeName, "Start" );
_LIT8( KTCStartNodeDescription, 
	"Exec command causes device to start an application" );

_LIT8( KTCStopNodeName, "Stop" );
_LIT8( KTCStopNodeDescription, 
	"Exec command causes device to stop an application or process" );

_LIT8( KTCProcessesNodeName, "Processes" );
_LIT8( KTCProcessesNodeDescription, 
	"TThis is root for all process related information that is \
	available for the admin" );

_LIT8( KTCProcessesDynaNodeDescription, 
	"This node is placeholder for an identity of a process" );

_LIT8( KTCIDNodeName, "ID" );
_LIT8( KTCIDNodeDescription, 
	"This leaf node holds the unique id of the process" );

_LIT8( KTCFromRAMNodeName, "FromRAM" );
_LIT8( KTCFromRAMNodeDescription, 
	"This leaf holds knowledge whether the process has been loaded from \
	RAM or ROM. " );

_LIT8( KTCMemoryInfoNodeName, "MemoryInfo" );
_LIT8( KTCMemoryInfoNodeDescription, 
	"This node holds some extra information about memory usage of the process." );

_LIT8( KTCProcessFilenameNodeName, "Filename" );
_LIT8( KTCProcessFilenameNodeDescription, 
	"This node holds the filename of the process" );

_LIT8( KTCRebootNodeName, "Reboot" );
_LIT8( KTCRebootNodeDescription, 
	"This node is the root node for all terminal control functionality" );

_LIT8( KTCDynamicNodeName, "" );
_LIT(  KTCSeparator16, "/" );

_LIT8( KTCSeparator8, "/" );

// -------------------------------------------------------------------
// CSmlDmEmailAdapter 
// -------------------------------------------------------------------
class CTcAdapter : public CTARMDmAdapter//, public MMsvSessionObserver
    {
private:
    enum TTcNodeIdentifier
        {
        ETcNodeTerminalControl = 0,
        ETcNodeFileScan,
#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
        ETcNodeFileScanExecute,
#endif
        ETcNodeFileScanResults,
        ETcNodeFileDelete,
        ETcNodeDeviceLock,
        ETcNodeAutoLockPeriod,
        ETcNodeMaxAutoLockPeriod,
        ETcNodeLockLevel,
        ETcNodeLockCode,						
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
        ETcNodeLockCodeMinLength,
        ETcNodeLockCodeMaxLength,
        ETcNodeLockCodeRequireUpperAndLower,
        ETcNodeLockCodeRequireCharsAndNumbers,
        ETcNodeLockCodeMaxRepeatedCharacters,
        ETcNodeLockCodeHistoryBuffer,
        ETcNodeLockCodeExpiration,
        ETcNodeLockCodeMinChangeTolerance,
        ETcNodeLockCodeMinChangeInterval,
        ETcNodeLockCodeCheckSpecificStrings,
        ETcNodeLockCodeDisallowSpecific,
        ETcNodeLockCodeAllowSpecific,
        ETcNodeLockCodeClearSpecificStrings,
        ETcNodeLockCodeMaxAttempts,
        ETcNodeLockConsecutiveNumbers,
        ETcNodeLockCodeMinSpecialCharacters,
        ETcNodeLockDisallowSimple,        
//#endif
// --------- Enhanced features END --------------------
        ETcNodeDeviceWipe,
        ETcNodeProcesses,
        ETcNodeProcessesX,
        ETcNodeID,
        ETcNodeFromRAM,
        ETcNodeMemoryInfo,
        ETcNodeFilename,
        ETcNodeReboot,
        ETcNodeLocalOperations,
        ETcNodeStart,
        ETcNodeStop,
        ETcNodeNotUsedAndAlwaysLast
        };

    enum TTcNodeAccessType
        {
        ENoAccess = 0,
        EExecute  = 1,
        EGet      = 2,
        EReplace  = 4,
        };

    static const TInt iAccessTypeList[ETcNodeNotUsedAndAlwaysLast+1];
    
    TInt iAccessHistory[ETcNodeNotUsedAndAlwaysLast+1];

public:
    CTcAdapter(TAny* aEcomArguments);
    
    static CTcAdapter* NewL(MSmlDmCallback* aDmCallback );
    static CTcAdapter* NewLC(MSmlDmCallback* aDmCallback );

    virtual ~CTcAdapter();

    // Pure virtual methods
    void DDFVersionL( CBufBase& aVersion );
    
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    
    CSmlDmAdapter::TError FetchLeafObjectL( const TDesC8& aURI, 
    	const TDesC8& aLUID, const TDesC8& aType, 
    	CBufFlat* aObject, CBufFlat* aRetMime );

    // From CTARMDmAdapter
    void _UpdateLeafObjectL( const TDesC8& aURI, 
    	const TDesC8& aLUID, const TDesC8& aObject, 
    	const TDesC8& aType, TInt aStatusRef );
    void _UpdateLeafObjectL( const TDesC8& aURI, 
    	const TDesC8& aLUID, RWriteStream*& aStream, 
    	const TDesC8& aType, TInt aStatusRef );
    void _DeleteObjectL( const TDesC8& aURI, 
    	const TDesC8& aLUID, TInt aStatusRef );
    void _FetchLeafObjectL( const TDesC8& aURI, 
    	const TDesC8& aLUID, const TDesC8& aType, 
    	TInt aResultsRef, TInt aStatusRef );
    void _FetchLeafObjectSizeL( const TDesC8& aURI, 
    	const TDesC8& aLUID, const TDesC8& aType, 
    	TInt aResultsRef, TInt aStatusRef );
    void _ChildURIListL( const TDesC8& aURI, 
    	const TDesC8& aLUID, 
    	const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
    	TInt aResultsRef, TInt aStatusRef );
    void _AddNodeObjectL( const TDesC8& aURI, 
    	const TDesC8& aParentLUID, TInt aStatusRef );
    void _ExecuteCommandL( const TDesC8& aURI, 
    	const TDesC8& aLUID, const TDesC8& aArgument, 
    	const TDesC8& aType, TInt aStatusRef );
    void _ExecuteCommandL( const TDesC8& aURI, 
    	const TDesC8& aLUID, RWriteStream*& aStream, 
    	const TDesC8& aType, TInt aStatusRef );
    void _CopyCommandL( const TDesC8& aTargetURI, 
    	const TDesC8& aTargetLUID, const TDesC8& aSourceURI, 
    	const TDesC8& aSourceLUID, const TDesC8& aType, 
    	TInt aStatusRef );
    
    void StartAtomicL();   
    void CommitAtomicL();
    void RollbackAtomicL();
    TBool StreamingSupport( TInt& aItemSize );
#ifdef __TARM_SYMBIAN_CONVERGENCY	
		virtual void StreamCommittedL( RWriteStream& aStream );
#else
		virtual void StreamCommittedL();
#endif	
    void CompleteOutstandingCmdsL();

    TInt StartProcessL(const TDesC8& aURI);
    TInt StopProcessL(const TDesC8& aURI);

private:
    void ConstructL(MSmlDmCallback* aDmCallback);

    void FillNodeInfoL(MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
            MSmlDmDDFObject::TOccurence aOccurrence,
            MSmlDmDDFObject::TScope aScope,
            MSmlDmDDFObject::TDFFormat aFormat,
            const TDesC8& aDescription,
            const TDesC8& aMimeType);

    void CopyAndTrimMimeType(TDes8& aNewMime, const TDesC8& aType);
    void CopyAndTrimURI(TDes8& aNewURI, const TDesC8& aURI);
    void ParseStartPathL(const TDesC8 &aArguments, 
    	TDes8 &aStartPath, TBool &aRecursive);

    TTcNodeIdentifier GetNodeIdentifier(const TDesC8& aURI);
    TInt NewTerminalControlSession();
    TInt CheckTerminalControlSession();
    TPtrC8 PolicyRequestResourceL( const TDesC8& aURI );

	void _UpdateLeafObjectToBeTrappedL( const TDesC8& aURI, 
														 const TDesC8& aLUID,
                             const TDesC8& aObject, const TDesC8& aType,
                             const TInt aStatusRef);
                             
	void FetchLeafObjectToBeTrappedL( const TDesC8& aURI, 
																			const TDesC8& aLUID,
                                      const TDesC8& aType, 
                                      CBufFlat* aObject,
                                      CBufFlat* aRetMime );
                                      
	void _ExecuteCommandToBeTrappedL( const TDesC8& aURI, 
																			const TDesC8& aLUID,
                                  	  const TDesC8& aArgument, 
                                  	  const TDesC8& aType,
                                  	  TInt aStatusRef );                                      
                                                                                 
private:

    RTerminalControl iTC;
    RTerminalControlSession iTCSession;
    TBool iConnected;

    // Rollback information
    TInt32 iAutoLockPeriod;
    TInt32 iLockLevel;
    TInt32 iMaxAutoLockPeriod;
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
    TInt32  iLockCodeMinLength;
    TInt32  iLockCodeMaxLength;
    TBool   iLockCodeRequireUpperAndLower;
    TInt32  iLockCodeRequireUpperAndLowerInt;
    TBool   iLockCodeRequireCharsAndNumbers;
    TInt32  iLockCodeRequireCharsAndNumbersInt;
    TInt32  iLockCodeMaxRepeatedCharacters;
    TInt32  iLockCodeHistoryBuffer;
    TInt32  iLockCodeExpiration;
    TInt32  iLockCodeMinChangeTolerance;
    TInt32  iLockCodeMinChangeInterval;
    TBool   iLockCodeCheckSpecificStrings;
    TInt32  iLockCodeCheckSpecificStringsInt;
    TInt32  iLockCodeMaxAttempts;
    TBool   iLockCodeConsecutiveNumbers;
    TInt32  iLockCodeConsecutiveNumbersInt;
    TInt32  iLockCodeMinSpecialCharacters;
    TBool   iLockCodeDisallowSimple;
    TInt32  iLockCodeDisallowSimpleInt;

//#endif
// --------- Enhanced features END --------------------

};

#endif // __SMLTCADAPTER_H__
