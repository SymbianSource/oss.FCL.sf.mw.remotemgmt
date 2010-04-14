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


/*

./Terminal Security -+--> FileScan (E) -+--> Execute (E)
                                        |
                                        |
                                        |--> Results (G)


./Terminal Security -+--> FileDelete (E)


./Terminal Security -+--> DeviceWipe (E)


./Terminal Security -+--> Reboot (E)


./Terminal Security -+--> DeviceLock (G) -+--> Autolock Period : int (GR)
               |                      				
               |
               |--> LockLevel : int (GR)
               |
               |
               |--> LockCode : int (R)
               |
               |
               |--> Max Autolock Period : int (GR)
               |
               |--> Lock Code min length : int (GR)
               |
               |--> Lock Code max length : int (GR)
               |
               |--> Lock Code require upper and lower case letters : bool (GR)
               |
               |--> Lock Code require characters and numbers : bool (GR)
               |
               |--> Lock Code max repeated chars : int (GR)
               |
               |--> Lock Code history buffer : int (GR)
               |
               |--> Lock Code expiration : int (GR)
               |
               |--> Lock Code min change tolerance : int (GR)
               |
               |--> Lock Code min change interval : int (GR)
               |
               |--> Lock Code check specific strings : bool (GR)
               |
               |--> Lock Code disallow specific strings : chr (R)
               |
               |--> Lock Code allow specific strings : chr (R)
               |
               |--> Lock Code clear specific strings : null (E)
               |
               |--> Lock Code max attempts : int (GR)       
               |
               |--> Lock Code ConsecutiveNumbers : bool (GR)  
               |
               |--> Lock Code min special chars : int (GR)
               |
               |--> Lock code disallow simple : bool (GR)
                                                  


./Terminal Security -+--> Local Operations (G) -+--> Start (E)
                                               |
                                               |
                                               |--> Stop (E)
                                         

./Terminal Security -+--> Processes (G) -+--> <X> * (G) -+--> ID (G)
                                                        |
                                                        |
                                                        |--> FromRAM (G)
                                                        |
                                                        |
                                                        |--> MemoryInfo (G)
                                                        |
                                                        |
                                                        |--> Filename (G)

*/

#ifdef __WINS__
#define __STIF_EMULATOR_TESTING__
#endif
// ---------------------------------------------------------------------------
#include "tcadapter.h"

// symbian
#ifndef __WINS_DEBUG_TESTING__
    #include <implementationproxy.h> // For TImplementationProxy definition
#endif
#include <fbs.h>
#include <f32file.h>
// s60
// tarm
#include <DMCert.h>
#include <SettingEnforcementInfo.h>
#include <PolicyEngineXACML.h>
#include "nsmldmuri.h"
#include "TPtrC8I.h"

#ifdef __Sml_DEBUG__
#pragma message("Sml Debugging is ON!")
#endif

#include "debug.h"
#include <featmgr.h>
const TInt KFlatBufSize = 128;
const TInt KFlatMimeBufSize = 20;

#if defined( __WINS_DEBUG_TESTING__ ) || defined ( __STIF_EMULATOR_TESTING__ )
_LIT( KDebugFileName, "C:\\tcadapter.log" );
#endif


const TInt CTcAdapter::iAccessTypeList[ETcNodeNotUsedAndAlwaysLast+1] =
    {
    /*ETcNodeTerminalControl*/     CTcAdapter::EGet
    ,/*ETcNodeFileScan*/            CTcAdapter::EExecute
#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
    ,/*ETcNodeFileScanExecute*/     CTcAdapter::EExecute
#endif
    ,/*ETcNodeFileScanResults*/     CTcAdapter::EGet
    ,/*ETcNodeFileDelete*/          CTcAdapter::EExecute
    ,/*ETcNodeDeviceLock*/          CTcAdapter::EGet
    ,/*ETcNodeAutoLockPeriod*/      CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeMaxAutoLockPeriod*/   CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockLevel*/           CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCode*/            CTcAdapter::EReplace
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
    ,/*ETcNodeLockCodeMinLength*/    CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeMaxLength*/    CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeRequireUpperAndLower*/ 	
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeRequireCharsAndNumbers*/ 	
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeMaxRepeatedCharacters*/  	
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeHistoryBuffer*/           
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeExpiration*/           	
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeMinChangeTolerance*/    	
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeMinChangeInterval*/      	
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeCheckSpecificStrings*/  	
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeDisallowSpecific*/       	
    	CTcAdapter::EReplace
    ,/*ETcNodeLockCodeAllowSpecific*/           
    	CTcAdapter::EReplace
    ,/*ETcNodeLockCodeClearSpecificStrings*/  	
    	CTcAdapter::EExecute
    ,/*ETcNodeLockCodeMaxAttempts*/           	
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockConsecutiveNumbers*/          
    	CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockCodeMinSpecialCharacters*/   
        CTcAdapter::EGet | CTcAdapter::EReplace
    ,/*ETcNodeLockDisallowSimple*/          
        CTcAdapter::EGet | CTcAdapter::EReplace


//#endif
// --------- Enhanced features END --------------------
    ,/*ETcNodeDeviceWipe*/          CTcAdapter::EExecute
    ,/*ETcNodeProcesses*/           CTcAdapter::EGet
    ,/*ETcNodeProcessesX*/          CTcAdapter::EGet
    ,/*ETcNodeID*/                  CTcAdapter::EGet
    ,/*ETcNodeFromRAM*/             CTcAdapter::EGet
    ,/*ETcNodeMemoryInfo*/          CTcAdapter::EGet
    ,/*ETcNodeFilename*/            CTcAdapter::EGet
    ,/*ETcNodeReboot*/              CTcAdapter::EExecute
    ,/*ETcNodeLocalOperations*/     CTcAdapter::EGet
    ,/*ETcNodeStart*/               CTcAdapter::EExecute
    ,/*ETcNodeStop*/                CTcAdapter::EExecute
    ,/**/                           CTcAdapter::ENoAccess
    };

// --------------------------------------------------------------------
// CTcAdapter* CTcAdapter::NewL( )
// --------------------------------------------------------------------
CTcAdapter* CTcAdapter::NewL( MSmlDmCallback* aDmCallback )
    {
    RDEBUG("CTcAdapter::NewL()");

    CTcAdapter* self = NewLC( aDmCallback );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------
// CTcAdapter* CTcAdapter::NewLC( )
// ---------------------------------------------------------------------
CTcAdapter* CTcAdapter::NewLC( MSmlDmCallback* aDmCallback )
    {
    RDEBUG("CTcAdapter::NewLC()");

    CTcAdapter* self = new( ELeave ) CTcAdapter(aDmCallback);
    CleanupStack::PushL( self );
    self->ConstructL( aDmCallback );
    return self;
    }

// --------------------------------------------------------------------
// CTcAdapter::CTcAdapter()
// --------------------------------------------------------------------
CTcAdapter::CTcAdapter(TAny* aEcomArguments)
: CTARMDmAdapter((MSmlDmCallback*)aEcomArguments)
, iConnected(EFalse)
, iAutoLockPeriod(-1)
, iLockLevel(-1)
, iMaxAutoLockPeriod(-1)
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
, iLockCodeMinLength( -1 )
, iLockCodeMaxLength( -1 )
, iLockCodeRequireUpperAndLowerInt( -1 )
, iLockCodeRequireCharsAndNumbersInt( -1 )
, iLockCodeMaxRepeatedCharacters( -1 )
, iLockCodeHistoryBuffer( -1 )
, iLockCodeExpiration( -1 )
, iLockCodeMinChangeTolerance( -1 )
, iLockCodeMinChangeInterval( -1 )
, iLockCodeCheckSpecificStringsInt( -1 )
, iLockCodeMaxAttempts( -1 )
, iLockCodeConsecutiveNumbersInt( -1 )
, iLockCodeMinSpecialCharacters( -1 )
, iLockCodeDisallowSimpleInt( -1 )
//#endif
// --------- Enhanced features END --------------------

    {
    memset(&iAccessHistory[0], CTcAdapter::ENoAccess, sizeof(iAccessHistory));
    }

// -------------------------------------------------------------------------
// CTcAdapter::~CTcAdapter()
// -------------------------------------------------------------------------
CTcAdapter::~CTcAdapter()
    {
    RDEBUG("CTcAdapter::~CTcAdapter()");

    iTCSession.Close();
    iTC.Close();
    }

// -------------------------------------------------------------------------
//  CTcAdapter::DDFVersionL()
// -------------------------------------------------------------------------
void CTcAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    RDEBUG("CTcAdapter::DDFVersionL()");

    aDDFVersion.InsertL( 0, KSmlTCDDFVersion );
    }

// -------------------------------------------------------------------------
//  CTcAdapter::DDFStructureL()
//
// -------------------------------------------------------------------------
void CTcAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    RDEBUG("CTcAdapter::DDFStructureL()" );

    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
    TSmlDmAccessTypes accessTypeExecute;
    accessTypeExecute.SetExec();
#endif

    /*
    Node: ./TerminalSecurity
    This node is the root node for all terminal control functionality.
·    Support: Mandatory
·    Occurs: One
·    Format: Node
·    Access Types: Get
·    Values: N/A

    */
    MSmlDmDDFObject& tc = aDDF.AddChildObjectL( KTCNodeName );
    FillNodeInfoL(tc, accessTypesGet, MSmlDmDDFObject::EOne, 
    	MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode, KTCNodeDescription, KNullDesC8() );

    TSmlDmAccessTypes accessTypesExec;
    accessTypesExec.SetExec();

    /*
    Node: ./TerminalSecurity/FileScan
    The file scan feature delivers to admin a list 
    of files or folders found in device.
·    Support: Mandatory
·    Occurs: One
·    Format: Node
·    Access Types: Exec
·    Values: N/A

    */
    MSmlDmDDFObject& fs = tc.AddChildObjectL( KTCFileScanNodeName );
    FillNodeInfoL( fs, accessTypesExec, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode, KTCFileScanNodeDescription, KNullDesC8() );    

#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
    /*
    Node: ./TerminalSecurity/FileScan/Execute
    This node can be used to execute file scan operation.
·    Support: Mandatory
·    Occurs: One
·    Format: NULL
·    Access Types: Exec
·    Values: N/A
    */
    MSmlDmDDFObject& fse = fs.AddChildObjectL( KTCFileScanExecuteNodeName );
    FillNodeInfoL( fse, accessTypeExecute, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENull, 
        KTCFileScanExecuteNodeDescription, KNullDesC8() );
#endif

    /*
    Node: ./TerminalSecurity/FileScan/Results
    This node will hold the scan results
·    Support: Mandatory
·    Occurs: One
·    Format: Xml
·    Access Types: Get
·    Values: extended Obex folder listing objects
    */
    MSmlDmDDFObject& fsr = fs.AddChildObjectL( KTCFileScanResultsNodeName );
    FillNodeInfoL( fsr, accessTypesGet, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EXml, KTCFileScanResultsNodeDescription, 
        KSmlTCTextXml() );

    /*
    Node: ./TerminalSecurity/FileDelete
    Deletes a file or empty folder
·    Support: Mandatory
·    Occurs: One
·    Format: N/A
·    Access Types: Exec
·    Values: N/A
    */
    MSmlDmDDFObject& fd = tc.AddChildObjectL( KTCFileDeleteNodeName );
    FillNodeInfoL( fd, accessTypesExec, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode, KTCFileDeleteNodeDescription, KNullDesC8() );

    /*
    Node: ./TerminalSecurity/DeviceLock
    The parent node for all DeviceLock related nodes
·    Support: Mandatory
·    Occurs: One
·    Format: Node
·    Access Types: Get
·    Values: N/A
    */
    MSmlDmDDFObject& dl = tc.AddChildObjectL( KTCDeviceLockNodeName );
    FillNodeInfoL( dl, accessTypesGet, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode, KTCDeviceLockNodeDescription, 
        KNullDesC8() );

    TSmlDmAccessTypes accessTypesGetReplace;
    accessTypesGetReplace.SetGet();
    accessTypesGetReplace.SetReplace();

    /*
    Node: ./TerminalSecurity/DeviceLock/AutoLockPeriod
    AutoLockPeriod is an integer that tells after how many 
    minutes the device will be automatically locked if not used
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Any integer in range device accepts.

    */
    MSmlDmDDFObject& alp = dl.AddChildObjectL( KTCAutoLockPeriodNodeName );
    FillNodeInfoL( alp, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCAutoLockPeriodNodeDescription, 
        KSmlTCTextPlain() );

    /*
    Node: ./TerminalSecurity/DeviceLock/MaxAutoLockPeriod
    MaxAutoLockPeriod is an integer that tells maximum value of AutoLockPeriod
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Any integer in range device accepts.

    */
    MSmlDmDDFObject& malp = dl.AddChildObjectL( KTCMaxAutoLockPeriodNodeName );
    FillNodeInfoL( malp, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCMaxAutoLockPeriodNodeDescription, 
        KSmlTCTextPlain() );

    /*
    Node: ./TerminalSecurity/DeviceLock/LockLevel
    Lock level tells whether lock is enabled, and what type of lock it is
·    Support: Mandatory
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: See examples below

    Example Values:
    0 - indicates lock is disabled
    1 - Indicates user lock is active. 
    		User can open lock by User secure code.
    2 - indicates admin lock is active. 
    		Lock can be opened only by Admin secure code.
    */
    MSmlDmDDFObject& ll = dl.AddChildObjectL( KTCLockLevelNodeName );
    FillNodeInfoL( ll, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockLevelNodeDescription, KSmlTCTextPlain() );

    TSmlDmAccessTypes accessTypesReplace;
    accessTypesReplace.SetReplace();

    /*
    Node: ./TerminalSecurity/DeviceLock/LockCode
    LockCode is the character sequence that is needed to open locked device
·    Status: Optional
·    Occurs: One
·    Format: chr
·    Access Types: Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lc = dl.AddChildObjectL( KTCLockCodeNodeName );
    FillNodeInfoL( lc, accessTypesReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr, KTCLockCodeNodeDescription, KSmlTCTextPlain() );










//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
// -------- Enhanced features BEGIN
	
	/*
    Node: ./TerminalSecurity/DeviceLock/LockCodeMinLength
    LockCode minimum length, 4-256 characters
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcmin = dl.AddChildObjectL( KTCLockCodeMinLengthNodeName );
    FillNodeInfoL( lcmin, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeMinLengthNodeDescription, 
        KSmlTCTextPlain() );
	
	
	/*
    Node: ./TerminalSecurity/DeviceLock/LockCodeMaxLength
    LockCode maximum length, 4-256 characters
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcmax = dl.AddChildObjectL( KTCLockCodeMaxLengthNodeName );
    FillNodeInfoL( lcmax, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeMaxLengthNodeDescription, 
        KSmlTCTextPlain() );
	
	
	/*
    Node: ./TerminalSecurity/DeviceLock/LockCodeRequireUpperAndLower
    If enabled the LockCode must contain both upper and lower case letters
·    Status: Optional
·    Occurs: One
·    Format: bool
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcrequl = 
    	dl.AddChildObjectL( KTCLockCodeRequireUpperAndLowerNodeName );
    FillNodeInfoL( lcrequl, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBool, KTCLockCodeRequireUpperAndLowerNodeDescription, 
        KSmlTCTextPlain() );
	
	
	/*
    Node: ./TerminalSecurity/DeviceLock/LockCodeRequireCharsAndNumbers
    If enabled the LockCode must contain both chars and numbers
·    Status: Optional
·    Occurs: One
·    Format: bool
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcreqcn = 
    	dl.AddChildObjectL( KTCLockCodeRequireCharsAndNumbersNodeName );
    FillNodeInfoL( lcreqcn, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBool, 
        KTCLockCodeRequireCharsAndNumbersNodeDescription, 
        KSmlTCTextPlain() );
        
        
    /*
    Node: ./TerminalSecurity/DeviceLock/LockCodeMaxRepeatedCharacters
    Tells how many times a single character can be used in a password. 
    Values between 0 and 4, 0 = no restriction
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcmaxrc = 
    	dl.AddChildObjectL( KTCLockCodeMaxRepeatedCharactersNodeName );
    FillNodeInfoL( lcmaxrc, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeMaxRepeatedCharactersNodeDescription, 
        KSmlTCTextPlain() );
             
        
    /*
    Node: ./TerminalSecurity/DeviceLock/LockCodeHistoryBuffer
    History buffer for previously used lock codes. 0 = no restriction, 
    1-X = the lock code can not match the previous X lock codes
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lchb = 
    	dl.AddChildObjectL( KTCLockCodeHistoryBufferNodeName );
    FillNodeInfoL( lchb, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeHistoryBufferNodeDescription, 
        KSmlTCTextPlain() );
        
        
    /*
    Node: ./TerminalSecurity/DeviceLock/LockCodeExpiration
    The password expires after X days, 0 = no restriction, 1-365
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lce = dl.AddChildObjectL( KTCLockCodeExpirationNodeName );
    FillNodeInfoL( lce, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeExpirationNodeDescription, 
        KSmlTCTextPlain() );
        
        
    /*
    Node: ./TerminalSecurity/DeviceLock/LockCodeMinChangeTolerance
    The user can change the password only X times before 
    'minimum change interval' hours has passed
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcmct = 
    	dl.AddChildObjectL( KTCLockCodeMinChangeToleranceNodeName );
    FillNodeInfoL( lcmct, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeMinChangeToleranceNodeDescription, 
        KSmlTCTextPlain() );    
        
        
    /*
    Node: ./TerminalSecurity/DeviceLock/LockCodeMinChangeInterval
    The user cannot change the password more than 'minimum change 
    tolerance' times before X hours have passed since the previous change
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcmci = 
    	dl.AddChildObjectL( KTCLockCodeMinChangeIntervalNodeName );
    FillNodeInfoL( lcmci, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeMinChangeIntervalNodeDescription, 
        KSmlTCTextPlain() );       
        
        
    /*
    Node: ./TerminalSecurity/DeviceLock/LockCodeCheckSpecificStrings
    0 = No restriction, 1 = The password cannot match a string in the 
    set of strings given by the next three calls
·    Status: Optional
·    Occurs: One
·    Format: bool
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lccss = dl.AddChildObjectL( 
    	KTCLockCodeCheckSpecificStringsNodeName );
    FillNodeInfoL( lccss, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBool, 
        KTCLockCodeCheckSpecificStringsNodeDescription, 
        KSmlTCTextPlain() );  
            
            
    /*
    Node: ./TerminalSecurity/DeviceLock/LockCodeDisallowSpecific
    Disallow the specific string/strings given. 
    Individual strings can be separated with ';'
·    Status: Optional
·    Occurs: One
·    Format: chr
·    Access Types: Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcds = dl.AddChildObjectL( 
    	KTCLockCodeDisallowSpecificNodeName );
    FillNodeInfoL( lcds, accessTypesReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr, KTCLockCodeDisallowSpecificNodeDescription, 
        KSmlTCTextPlain() ); 
        
	/*
	Node: ./TerminalSecurity/DeviceLock/LockCodeAllowSpecific
    Allow the specific string/strings given. 
    Individual strings can be separated with ';'
·    Status: Optional
·    Occurs: One
·    Format: chr
·    Access Types: Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcas = dl.AddChildObjectL( 
    	KTCLockCodeAllowSpecificNodeName );
    FillNodeInfoL( lcas, accessTypesReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr, KTCLockCodeAllowSpecificNodeDescription, 
        KSmlTCTextPlain() ); 
           
           
    /*
	Node: ./TerminalSecurity/DeviceLock/LockCodeClearSpecificStrings
    Clear the specific string -buffer
·    Status: Optional
·    Occurs: One
·    Format: null
·    Access Types: Exec
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcclss = dl.AddChildObjectL( 
    	KTCLockCodeClearSpecificStringsNodeName );
    FillNodeInfoL( lcclss, accessTypesExec, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENull, KTCLockCodeClearSpecificStringsNodeDescription, 
        KSmlTCTextPlain() ); 
                     
                     
    /*
	Node: ./TerminalSecurity/DeviceLock/LockCodeMaxAttempts
    0 = No restriction, 3-100 = The device is "hard" reset after the 
    user has consecutively failed X times to answer the password query
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcma = dl.AddChildObjectL( 
    	KTCLockCodeMaxAttemptsNodeName );
    FillNodeInfoL( lcma, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeMaxAttemptsNodeDescription, 
        KSmlTCTextPlain() );  
        
    
    /*
	Node: ./TerminalSecurity/DeviceLock/ConsecutiveNumbers
    0 = No restriction, 3-100 = The device is "hard" reset after the 
    user has consecutively failed X times to answer the password query
·    Status: Optional
·    Occurs: One
·    Format: bool
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lccon = dl.AddChildObjectL( 
    	KTCLockCodeConsecutiveNumbersNodeName );
    FillNodeInfoL( lccon, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeConsecutiveNumbersNodeDescription, 
        KSmlTCTextPlain() );      
        
    /*
    Node: ./TerminalSecurity/DeviceLock/LockCodeMinSpecialCharacters
    Tells the least number of special characters the lock code must contain. 
    Values between 0 and 255, 0 = no restriction
·    Status: Optional
·    Occurs: One
·    Format: integer
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcminsc = 
        dl.AddChildObjectL( KTCLockCodeMinSpecialCharactersNodeName );
    FillNodeInfoL( lcminsc, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
            MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeMinSpecialCharactersNodeDescription, 
        KSmlTCTextPlain() );
             
        
    /*
    Node: ./TerminalSecurity/DeviceLock/DisallowSimple
    0 = No restriction, 1 = Password cannot be a simple string.
·    Status: Optional
·    Occurs: One
·    Format: bool
·    Access Types: Get, Replace
·    Values: Depends on device.
    */
    MSmlDmDDFObject& lcdissim = dl.AddChildObjectL( 
        KTCLockCodeDisallowSimpleNodeName );
    FillNodeInfoL( lcdissim, accessTypesGetReplace, MSmlDmDDFObject::EOne, 
            MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EInt, KTCLockCodeDisallowSimpleNodeDescription, 
        KSmlTCTextPlain() );

        
// -------- Enhanced features END
//#endif










    /*
    Node: ./TerminalSecurity/DeviceWipe
    Executing this node will wipe device data, after which the device is 
    completely resetted so that all non-permanent data will be lost
·    Support: Mandatory
·    Occurs: One
·    Format: chr
·    Access Types: Exec
·    Values: If none, performs complete wipe, if "RFS", 
		 then only settings are resetted

    */
    MSmlDmDDFObject& dw = tc.AddChildObjectL( KTCDeviceWipeNodeName );
    FillNodeInfoL( dw, accessTypesExec, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode, KTCDeviceWipeNodeDescription, 
        KNullDesC8() );

    /*
    Node: ./TerminalSecurity/LocalOperations
    This node is common parent for all local operations
·    Support: Mandatory
·    Occurs: One
·    Format: Node
·    Access Types: Get
·    Values: N/A
    */
    MSmlDmDDFObject& lo = tc.AddChildObjectL( KTCLocalOperationsNodeName );
    FillNodeInfoL( lo, accessTypesGet, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode, KTCLocalOperationsNodeDescription, 
        KNullDesC8() );

    /*
    Node: ./TerminalSecurity/LocalOperations/Start
    Exec command causes device to start an application
·    Support: Optional
·    Occurs: One
·    Format: Chr
·    Access Types: Exec
·    Values: URI to Application management inventory
    */
    MSmlDmDDFObject& sta = lo.AddChildObjectL( KTCStartNodeName );
    FillNodeInfoL( sta, accessTypesExec, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr, KTCStartNodeDescription, KNullDesC8() );

    /*
    Node: ./TerminalSecurity/LocalOperations/Stop
    Exec command causes device to stop an application or process
·    Support: Optional
·    Occurs: One
·    Format: Chr
·    Access Types: Exec
·    Values: URI to Application management or process inventory
    */
    MSmlDmDDFObject& sto = lo.AddChildObjectL( KTCStopNodeName );
    FillNodeInfoL( sto, accessTypesExec, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr, KTCStopNodeDescription, KNullDesC8() );

    /*
    Node: ./TerminalSecurity/Processes
    This is root for all process related information 
    that is available for the admin
·    Status: Optional 
·    Occurs: One
·    Format: Node
·    Access Types: Get
·    Values: N/A
    */
    MSmlDmDDFObject& pr = tc.AddChildObjectL( KTCProcessesNodeName );
    FillNodeInfoL( pr, accessTypesGet, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode, KTCProcessesNodeDescription, KNullDesC8() );

    /*
    Node: ./TerminalSecurity/Processes/<X>
    This node is placeholder for an identity of a process
·    Status: Optional 
·    Occurs: ZeroOrMore
·    Format: Node
·    Access Types: Get
    Values: N/A
    */
    MSmlDmDDFObject& dyna = pr.AddChildObjectGroupL( );
    FillNodeInfoL( dyna, accessTypesGet, MSmlDmDDFObject::EZeroOrMore, 
    		MSmlDmDDFObject::EDynamic,
        MSmlDmDDFObject::ENode, KTCProcessesDynaNodeDescription, 
        	KNullDesC8() );

    /*
    Node: ./TerminalSecurity/Processes/<X>/ID
    This leaf node holds the unique id of the process
·    Status: Optional 
·    Occurs: One
·    Format: Chr
·    Access Types: Get
·    Values: Depend on device
    */
    MSmlDmDDFObject& id = dyna.AddChildObjectL( KTCIDNodeName );
    FillNodeInfoL( id, accessTypesGet, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr, KTCIDNodeDescription, KSmlTCTextPlain() );

    /*
    Node: ./TerminalSecurity/Processes/<X>/FromRAM
    This leaf holds knowledge whether the process has been 
    loaded from RAM or ROM. 
·    Status: Optional 
·    Occurs: One
·    Format: bool
·    Access Types: Get
·    Values: true, if process is loaded from dynamic memory, false otherwise
    */
    MSmlDmDDFObject& ram = dyna.AddChildObjectL( KTCFromRAMNodeName );
    FillNodeInfoL( ram, accessTypesGet, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EBool, KTCFromRAMNodeDescription, KSmlTCTextPlain() );

    /*
    Node: ./TerminalSecurity/Processes/<X>/MemoryInfo
    This node holds some extra information about memory usage of the process
·    Status: Optional 
·    Occurs: One
·    Format: Xml
·    Access Types: Get
·    Values: N/A
    */
    MSmlDmDDFObject& mi = dyna.AddChildObjectL( KTCMemoryInfoNodeName );
    FillNodeInfoL( mi, accessTypesGet, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EXml, 
        KTCMemoryInfoNodeDescription, KSmlTCTextPlain() );

    /*
    Node: ./TerminalSecurity/Processes/<X>/Filename
    This node holds the filename of the process. 
    
·    Status: Optional 
·    Occurs: One
·    Format: Chr
·    Access Types: Get
·    Values: Filename without path
    */
    MSmlDmDDFObject& fn = dyna.AddChildObjectL( KTCProcessFilenameNodeName );
    FillNodeInfoL( fn, accessTypesGet, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::EChr, KTCProcessFilenameNodeDescription, 
        KSmlTCTextPlain() );        

    /*
    Node: ./TerminalSecurity/Reboot
    Exec command causes device to be restarted after the end of 
    current management command sequence. 
·    Status: Optional
·    Occurs: One
·    Format: chr
·    Access Types: Exec
·    Values: N/A

    */
    MSmlDmDDFObject& reb = tc.AddChildObjectL( KTCRebootNodeName );
    FillNodeInfoL( reb, accessTypesExec, MSmlDmDDFObject::EOne, 
    		MSmlDmDDFObject::EPermanent,
        MSmlDmDDFObject::ENode, KTCRebootNodeDescription, KNullDesC8() );            
    }

// ---------------------------------------------------------------------------
//  CTcAdapter::_UpdateLeafObjectL()
// --------------------------------------------------------------------------
void CTcAdapter::_UpdateLeafObjectL( const TDesC8& aURI, 
																		 const TDesC8& aLUID,
                                    const TDesC8& aObject, 
                                    const TDesC8& aType,
                                    const TInt aStatusRef)
    {
    RDEBUG("CTcAdapter::_UpdateLeafObjectL()");
#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
		TRAPD( err, _UpdateLeafObjectToBeTrappedL( uriPtrc, aLUID, aObject, aType, aStatusRef ) )
#else    
    TRAPD( err, _UpdateLeafObjectToBeTrappedL( aURI, aLUID, aObject, aType, aStatusRef ) )
#endif
    
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    
    if( err != KErrNone )
    	{
    	status = CSmlDmAdapter::EError;
    	}

    Callback().SetStatusL( aStatusRef, status );
    }




void CTcAdapter::_UpdateLeafObjectToBeTrappedL( const TDesC8& aURI, 
																		const TDesC8& /*aLUID*/,
                                    const TDesC8& aObject, 
                                    const TDesC8& /*aType*/,
                                    const TInt /*aStatusRef*/)
    {
    RDEBUG("CTcAdapter::_UpdateLeafObjectToBeTrappedL()");
    
    TLex8 lex( aObject );
    TInt value;
    
    // Check the aObject-string for truth-values true/false
    TInt boolValue = -1; 
    if ( aObject.CompareF( KTCValueTrue ) == 0 )
        {
        boolValue = 1;
        }
    else if ( aObject.CompareF( KTCValueFalse ) == 0 )
        {
        boolValue = 0;
        }

    User::LeaveIfError( CheckTerminalControlSession() );

    CTcAdapter::TTcNodeIdentifier identifier = GetNodeIdentifier(aURI);

    if( ( iAccessTypeList[identifier] & CTcAdapter::EReplace) 
    	== CTcAdapter::EReplace )
        {
        iAccessHistory[identifier] |= CTcAdapter::EReplace;
        
        switch( identifier )
            {
            case ETcNodeAutoLockPeriod:
        		{
        		User::LeaveIfError( lex.Val( value ) );
        		if( -1 == iAutoLockPeriod )
        			{
        			TInt err = iTCSession.GetAutolockPeriod( iAutoLockPeriod );
        			if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
        			}
        		User::LeaveIfError( iTCSession.SetAutolockPeriod( value ) );
        		}
            break;

            case ETcNodeMaxAutoLockPeriod:
            	{
            	User::LeaveIfError( lex.Val( value ) );
                if( -1 == iMaxAutoLockPeriod )
                    {
                    TInt err = 
                    	iTCSession.GetMaxAutolockPeriod( iMaxAutoLockPeriod );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( iTCSession.SetMaxAutolockPeriod( value ) );
                }
            break;

            case ETcNodeLockLevel:
            	{
				// Truncate any White Space before or after the value.
				
				if(!lex.Eos())
            	    {
            	    lex.SkipSpace();
            	    User::LeaveIfError( lex.Val( value ) );
            	    lex.SkipSpaceAndMark();
					// Validate if there is a white space separated string.
            	    if (lex.Peek()!=0)
            	        User::Leave( KErrNotSupported );  
            	    }
            	else
                    User::Leave( KErrNotSupported ); 

                if( -1 == iLockLevel )
                    {                        
                    TInt err = iTCSession.GetDeviceLockLevel( iLockLevel );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( iTCSession.SetDeviceLockLevel( value ) );
                }
            break;

            case ETcNodeLockCode:
            	{            	
                User::LeaveIfError( iTCSession.SetDeviceLockCode( aObject ) );
                }
            break;

// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

			case ETcNodeLockCodeMinLength:
				{
				User::LeaveIfError( lex.Val( value ) );
                if( -1 == iLockCodeMinLength )
                    {
                    TInt err = 
                    	iTCSession.GetPasscodeMinLength( iLockCodeMinLength );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( iTCSession.SetPasscodeMinLength( value ) );
				}
			break;

			case ETcNodeLockCodeMaxLength:
				{
				User::LeaveIfError( lex.Val( value ) );
                if( -1 == iLockCodeMaxLength )
                    {
                    TInt err = 
                    	iTCSession.GetPasscodeMaxLength( iLockCodeMaxLength );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( iTCSession.SetPasscodeMaxLength( value ) );	
				}
			break;
			
			case ETcNodeLockCodeRequireUpperAndLower:
				{
				if ( boolValue == -1 )
				    {
				    User::LeaveIfError( lex.Val( value ) );
				    }
				else
				    {
				    value = boolValue;
				    }
				
                if( -1 == iLockCodeRequireUpperAndLowerInt )
                    {
                    TInt err = 
                    	iTCSession.GetPasscodeRequireUpperAndLower( 
                    		iLockCodeRequireUpperAndLower );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    iLockCodeRequireUpperAndLowerInt = 0;
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeRequireUpperAndLower( value ) );	
				}
			break;
			
			case ETcNodeLockCodeRequireCharsAndNumbers:
				{
				if ( boolValue == -1 )
				    {
				    User::LeaveIfError( lex.Val( value ) );
				    }
				else
				    {
				    value = boolValue;
				    }
				    
                if( -1 == iLockCodeRequireCharsAndNumbersInt )
                    {
                    TInt err = iTCSession.GetPasscodeRequireCharsAndNumbers( 
                    	iLockCodeRequireCharsAndNumbers );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    iLockCodeRequireCharsAndNumbersInt = 0;
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeRequireCharsAndNumbers( value ) );
				}
			break;
			
			case ETcNodeLockCodeMaxRepeatedCharacters:
				{
				User::LeaveIfError( lex.Val( value ) );
                if( -1 == iLockCodeMaxRepeatedCharacters )
                    {
                    TInt err = iTCSession.GetPasscodeMaxRepeatedCharacters( 
                    	iLockCodeMaxRepeatedCharacters );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeMaxRepeatedCharacters( value ) );
				}
			break;
			
			case ETcNodeLockCodeHistoryBuffer:
				{
				User::LeaveIfError( lex.Val( value ) );
                if( -1 == iLockCodeHistoryBuffer )
                    {
                    TInt err = iTCSession.GetPasscodeHistoryBuffer( 
                    	iLockCodeHistoryBuffer );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeHistoryBuffer( value ) );
				}
			break;
			
			case ETcNodeLockCodeExpiration:
				{
				User::LeaveIfError( lex.Val( value ) );
                if( -1 == iLockCodeExpiration )
                    {
                    TInt err = 
                    	iTCSession.GetPasscodeExpiration( iLockCodeExpiration );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( iTCSession.SetPasscodeExpiration( value ) );
				}
			break;
			
			case ETcNodeLockCodeMinChangeTolerance:
				{
				User::LeaveIfError( lex.Val( value ) );
                if( -1 == iLockCodeMinChangeTolerance )
                    {
                    TInt err = iTCSession.GetPasscodeMinChangeTolerance( 
                    	iLockCodeMinChangeTolerance );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeMinChangeTolerance( value ) );	
				}
			break;
			
			case ETcNodeLockCodeMinChangeInterval:
				{
				User::LeaveIfError( lex.Val( value ) );
                if( -1 == iLockCodeMinChangeInterval )
                    {
                    TInt err = iTCSession.GetPasscodeMinChangeInterval( 
                    	iLockCodeMinChangeInterval );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeMinChangeInterval( value ) );
				}
			break;
			
			case ETcNodeLockCodeCheckSpecificStrings:
				{
				if ( boolValue == -1 )
				    {
				    User::LeaveIfError( lex.Val( value ) );
				    }
				else
				    {
				    value = boolValue;
				    }
				    
                if( -1 == iLockCodeCheckSpecificStringsInt )
                    {
                    TInt err = iTCSession.GetPasscodeCheckSpecificStrings( 
                    	iLockCodeCheckSpecificStrings );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    iLockCodeCheckSpecificStringsInt = 0;
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeCheckSpecificStrings( value ) );	
				}
			break;
			
			case ETcNodeLockCodeDisallowSpecific:
				{
				// no rollback possibility
                TInt err = iTCSession.DisallowSpecificPasscodeString( aObject );
                if( err != KErrNone )
    				{
    				User::Leave( err );
    				}
				}
			break;
			
			case ETcNodeLockCodeAllowSpecific:
				{
				// no rollback possibility
                TInt err = iTCSession.AllowSpecificPasscodeString( aObject );
                if( err != KErrNone )
    				{
    				User::Leave( err );
    				}
				}
			break;
			
			case ETcNodeLockCodeClearSpecificStrings:
				{
				TInt err = iTCSession.ClearSpecificPasscodeStrings();
				if( err != KErrNone )
    				{
    				User::Leave( err );
    				}
				}
			break;
			
			case ETcNodeLockCodeMaxAttempts:
				{
				User::LeaveIfError( lex.Val( value ) );
                if( -1 == iLockCodeMaxAttempts )
                    {
                    TInt err = 
                     iTCSession.GetPasscodeMaxAttempts( iLockCodeMaxAttempts );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeMaxAttempts( value ) );	
				}
			break;
			
			case ETcNodeLockConsecutiveNumbers:
				{
				if ( boolValue == -1 )
				    {
				    User::LeaveIfError( lex.Val( value ) );
				    }
				else
				    {
				    value = boolValue;
				    }
				    
                if( -1 == iLockCodeConsecutiveNumbersInt )
                    {
                    TInt err = 
                    	iTCSession.GetPasscodeConsecutiveNumbers( 
                    		iLockCodeConsecutiveNumbers );
                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
        				{
        				User::Leave( err );
        				}
        			iLockCodeConsecutiveNumbersInt = 0;
                    }
                User::LeaveIfError( 
                	iTCSession.SetPasscodeConsecutiveNumbers( value ) );	
				}
			break;

	         case ETcNodeLockCodeMinSpecialCharacters:
	                {
	                User::LeaveIfError( lex.Val( value ) );
	                if( -1 == iLockCodeMinSpecialCharacters )
	                    {
	                    TInt err = iTCSession.GetPasscodeMinSpecialCharacters( 
	                        iLockCodeMinSpecialCharacters );
	                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
	                        {
	                        User::Leave( err );
	                        }
	                    }
	                User::LeaveIfError( 
	                    iTCSession.SetPasscodeMinSpecialCharacters( value ) );
	                }
	            break;

	            case ETcNodeLockDisallowSimple:
	                {
	                if ( boolValue == -1 )
	                    {
	                    User::LeaveIfError( lex.Val( value ) );
	                    }
	                else
	                    {
	                    value = boolValue;
	                    }
	                if( -1 == iLockCodeDisallowSimpleInt )
	                    {
	                    TInt err = 
	                        iTCSession.GetPasscodeDisallowSimple( 
	                            iLockCodeDisallowSimple );
	                    if( ( err != KErrNone ) && ( err != KErrNotFound ) )
	                        {
	                        User::Leave( err );
	                        }
	                    iLockCodeDisallowSimpleInt = 0;
	                    }
	                User::LeaveIfError( 
	                    iTCSession.SetPasscodeDisallowSimple( value ) );    
	                }
	            break;
//#endif
// --------- Enhanced features END --------------------

            default:
            	{
            	User::Leave( KErrGeneral );
            	}
            break;
            }
        }
        else
		    {
		    User::Leave( KErrGeneral );
		    }
    }

// --------------------------------------------------------------------------
//  CTcAdapter::_UpdateLeafObjectL()
// -------------------------------------------------------------------------
void CTcAdapter::_UpdateLeafObjectL( const TDesC8& /*aURI*/, 
																		 const TDesC8& /*aLUID*/,
                                    RWriteStream*& /*aStream*/, 
                                    const TDesC8& /*aType*/,
                                    TInt aStatusRef )
    {
    RDEBUG("CTcAdapter::_UpdateLeafObjectL() (stream)");
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }

// -------------------------------------------------------------------
// CTcAdapter::DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID )
// -------------------------------------------------------------------
void CTcAdapter::_DeleteObjectL( const TDesC8& /*aURI*/, 
															   const TDesC8& /*aLUID*/, 
															   const TInt aStatusRef)
    {
    RDEBUG("CTcAdapter::_DeleteObjectL()");
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }

// ------------------------------------------------------------------
//  CTcAdapter::FetchLeafObjectL
// ------------------------------------------------------------------



CSmlDmAdapter::TError CTcAdapter::FetchLeafObjectL( const TDesC8& aURI, 
																									  const TDesC8& aLUID,
                                                    const TDesC8& aType, 
                                                    CBufFlat* aObject,
                                                    CBufFlat* aRetMime )
    {
    RDEBUG("CTcAdapter::FetchLeafObjectL()");
#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
		TRAPD( err, FetchLeafObjectToBeTrappedL( uriPtrc, aLUID, aType, aObject, aRetMime ) );
#else    
    TRAPD( err, FetchLeafObjectToBeTrappedL( aURI, aLUID, aType, aObject, aRetMime ) );
#endif

	CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
	
	if( err == KErrNotFound )
		{
		status = CSmlDmAdapter::ENotFound;
		}
	else if( err != KErrNone )
		{
		status = CSmlDmAdapter::EError;
		}
		
	return status;
    }


void CTcAdapter::FetchLeafObjectToBeTrappedL( const TDesC8& aURI, 
																							const TDesC8& /*aLUID*/,
                                              const TDesC8& /*aType*/, 
                                              CBufFlat* aObject,
                                              CBufFlat* aRetMime )
    {
    RDEBUG("CTcAdapter::FetchLeafObjectToBeTrappedL()");
    
    TInt32 retVal;
    TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;    
    TPtrC8 segment(0, 0);

    User::LeaveIfError( CheckTerminalControlSession() );

    CTcAdapter::TTcNodeIdentifier identifier = GetNodeIdentifier(aURI);

    if((iAccessTypeList[identifier] & CTcAdapter::EGet) == CTcAdapter::EGet)
        {
            iAccessHistory[identifier] |= CTcAdapter::EGet;

            switch(identifier)
                {
                case ETcNodeTerminalControl:
                break;

                case ETcNodeFileScanResults:
                	{
	                iTCSession.FileScanResultsL( aObject );
	                aRetMime->InsertL( 0, KSmlTCTextXml() );

#if defined( __WINS_DEBUG_TESTING__ ) || defined ( __STIF_EMULATOR_TESTING__ )
	                    {
	                    RFs fs;
	                    User::LeaveIfError( fs.Connect() );
	                    RFile f;
	                    User::LeaveIfError( f.Replace(fs, 
	                    	KDebugFileName, EFileShareExclusive|EFileWrite) );
	                    User::LeaveIfError( 
	                    	f.Write(aObject->Ptr(0),aObject->Size()) );
	                    f.Close();
	                    fs.Close();
	                    }
#endif
                	}
                break;

                case ETcNodeDeviceLock:
                break;

                case ETcNodeAutoLockPeriod:
                	{
	                iTCSession.GetAutolockPeriod( retVal );
	                numBuf.Num( retVal ); 
	                aObject->InsertL(0, numBuf);
	                aRetMime->InsertL( 0, KSmlTCTextPlain() );
                	}
                break;

                case ETcNodeMaxAutoLockPeriod:
                	{
	                iTCSession.GetMaxAutolockPeriod( retVal );
	                numBuf.Num( retVal ); 
	                aObject->InsertL(0, numBuf);
	                aRetMime->InsertL( 0, KSmlTCTextPlain() );
                	}
                break;

                case ETcNodeLockLevel:
                	{
	                iTCSession.GetDeviceLockLevel( retVal );
	                numBuf.Num( retVal ); 
	                aObject->InsertL(0, numBuf);
	                aRetMime->InsertL( 0, KSmlTCTextPlain() );
                	}
                break;

// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
				
            	case ETcNodeLockCodeMinLength:
            		{
            		TInt err = iTCSession.GetPasscodeMinLength( retVal );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    retVal = KTCPassCodeMinLen;
            		    }
                	numBuf.Num( retVal ); 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeMaxLength:
            		{
            		TInt err = iTCSession.GetPasscodeMaxLength( retVal );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    retVal = KTCPassCodeMaxLen;
            		    }
                	numBuf.Num( retVal ); 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeRequireUpperAndLower:
            		{
            		TBool require( EFalse );
            		
            		TInt err = 
            			iTCSession.GetPasscodeRequireUpperAndLower( require );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    require = EFalse;
            		    }  
            		    
                	if ( !require )
                	    {
                	    numBuf.Copy( KTCValueFalse );
                	    }
                    else
                        {
                        numBuf.Copy( KTCValueTrue );
                        }
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeRequireCharsAndNumbers:
            		{
            		TBool require( EFalse );
            		TInt err = 
            			iTCSession.GetPasscodeRequireCharsAndNumbers( require );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    require = EFalse;
            		    }  
            		    
                	if ( !require )
                	    {
                	    numBuf.Copy( KTCValueFalse );
                	    }
                    else
                        {
                        numBuf.Copy( KTCValueTrue );
                        }
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeMaxRepeatedCharacters:
            		{
            		TInt err = 
            			iTCSession.GetPasscodeMaxRepeatedCharacters( retVal );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    retVal = 0;
            		    }  

                	numBuf.Num( retVal ); 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeHistoryBuffer:
            		{
            		TInt err = iTCSession.GetPasscodeHistoryBuffer( retVal );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    retVal = 0;
            		    }  

                	numBuf.Num( retVal ); 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeExpiration:
            		{
            		TInt err = iTCSession.GetPasscodeExpiration( retVal );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    retVal = 0;
            		    }  

                	numBuf.Num( retVal ); 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeMinChangeTolerance:
            		{
            		TInt err = iTCSession.GetPasscodeMinChangeTolerance( retVal );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    retVal = 0;
            		    }  

                	numBuf.Num( retVal ); 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeMinChangeInterval:
            		{
            		TInt err = iTCSession.GetPasscodeMinChangeInterval( retVal );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    retVal = 0;
            		    }  
            		       
                	numBuf.Num( retVal ); 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeCheckSpecificStrings:
            		{
            		TBool require( EFalse );
            		TInt err = 
            			iTCSession.GetPasscodeCheckSpecificStrings( require );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    require = EFalse;
            		    }              		

                	if ( !require )
                	    {
                	    numBuf.Copy( KTCValueFalse );
                	    }
                    else
                        {
                        numBuf.Copy( KTCValueTrue );
                        } 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockCodeDisallowSpecific:
            		{
            		// no implementation needed
            		}
            	break;
            	
            	case ETcNodeLockCodeAllowSpecific:
            		{
            		// no implementation needed
            		}
            	break;
            	
            	case ETcNodeLockCodeClearSpecificStrings:
            		{
            		// no implementation needed
            		}
            	break;
            	
            	case ETcNodeLockCodeMaxAttempts:
            		{            		
            		TInt err = iTCSession.GetPasscodeMaxAttempts( retVal );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    retVal = 0;
            		    }  
            		    
                	numBuf.Num( retVal ); 
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;
            	
            	case ETcNodeLockConsecutiveNumbers:
            		{
            		TBool require( EFalse );
            		TInt err = iTCSession.GetPasscodeConsecutiveNumbers( require );
            		if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
            		    {
            		    User::Leave( err );
            		    }
            		else if ( err == KErrNotFound ) // Param not set
            		    {
            		    require = EFalse;
            		    }  

                	if ( !require )
                	    {
                	    numBuf.Copy( KTCValueFalse );
                	    }
                    else
                        {
                        numBuf.Copy( KTCValueTrue );
                        }                	
                	aObject->InsertL( 0, numBuf );
                	aRetMime->InsertL( 0, KSmlTCTextPlain() );
            		}
            	break;

                case ETcNodeLockCodeMinSpecialCharacters:
                     {
                     TInt err = 
                         iTCSession.GetPasscodeMinSpecialCharacters( retVal );
                     if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
                         {
                         User::Leave( err );
                         }
                     else if ( err == KErrNotFound ) // Param not set
                         {
                         retVal = 0;
                         }  

                     numBuf.Num( retVal ); 
                     aObject->InsertL( 0, numBuf );
                     aRetMime->InsertL( 0, KSmlTCTextPlain() );
                     }
                 break;

                case ETcNodeLockDisallowSimple:
                    {
                    TBool require( EFalse );
                    TInt err = iTCSession.GetPasscodeDisallowSimple( require );
                    if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
                        {
                        User::Leave( err );
                        }
                    else if ( err == KErrNotFound ) // Param not set
                        {
                        require = EFalse;
                        }  
                    if ( !require )
                        {
                        numBuf.Copy( KTCValueFalse );
                        }
                    else
                        {
                        numBuf.Copy( KTCValueTrue );
                        }                   
                    aObject->InsertL( 0, numBuf );
                    aRetMime->InsertL( 0, KSmlTCTextPlain() );
                    }
                break;
//#endif
// --------- Enhanced features END --------------------
				
                case ETcNodeProcesses:
                break;

                case ETcNodeProcessesX:
                break;

                case ETcNodeMemoryInfo:
                	{
	                segment.Set(NSmlDmURI::RemoveLastSeg( aURI ));
	                iTCSession.GetProcessDataL( aObject, 
	                	NSmlDmURI::LastURISeg( segment ), 
	                		NSmlDmURI::LastURISeg( aURI ) );
	                aRetMime->InsertL( 0, KSmlTCTextXml() );
                	}
                break;

                case ETcNodeID:
                case ETcNodeFromRAM:
                case ETcNodeFilename:
                	{
	                segment.Set(NSmlDmURI::RemoveLastSeg( aURI ));
	                iTCSession.GetProcessDataL( aObject, 
	                	NSmlDmURI::LastURISeg( segment ), 
	                		NSmlDmURI::LastURISeg( aURI ) );
	                aRetMime->InsertL( 0, KSmlTCTextPlain() );
                	}
                break;

                case ETcNodeLocalOperations:
                break;

                default:
                	{
                	User::Leave( KErrNotSupported );
                	}
                break;
            }
        }
        else
	        {
	        User::Leave( KErrNotSupported );
	        }
    }

// ------------------------------------------------------------------------
//  CTcAdapter::_FetchLeafObjectL
// ------------------------------------------------------------------------
void CTcAdapter::_FetchLeafObjectL( const TDesC8& aURI, 
																	  const TDesC8& aLUID,
                                   const TDesC8& aType, 
                                   const TInt aResultsRef,
                                   const TInt aStatusRef )
    {
    RDEBUG("CTcAdapter::_FetchLeafObjectL()");

    CBufFlat *object = CBufFlat::NewL( KFlatBufSize );
    CleanupStack::PushL( object );

    CBufFlat *newMime = CBufFlat::NewL( KFlatMimeBufSize );
    CleanupStack::PushL( newMime );
    
#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
		CSmlDmAdapter::TError status = 
    	FetchLeafObjectL( uriPtrc, aLUID, aType, object, newMime );
#else    
    CSmlDmAdapter::TError status = 
    	FetchLeafObjectL( aURI, aLUID, aType, object, newMime );
#endif

    Callback().SetStatusL( aStatusRef, status );

    if( status == CSmlDmAdapter::EOk)
        {
        object->Compress();
        Callback().SetResultsL( aResultsRef,*object, newMime->Ptr( 0 ) );
        }

    CleanupStack::PopAndDestroy( newMime );
    CleanupStack::PopAndDestroy( object );
    }

// ----------------------------------------------------------------------
//  CTcAdapter::_FetchLeafObjectSizeL
// ----------------------------------------------------------------------
void CTcAdapter::_FetchLeafObjectSizeL( const TDesC8& aURI, 
																				const TDesC8& aLUID,
                                        const TDesC8& aType, 
                                        TInt aResultsRef,
                                        TInt aStatusRef )
    {
    RDEBUG("CTcAdapter::_FetchLeafObjectSizeL()");

    CBufFlat *object  = CBufFlat::NewL( KFlatBufSize );
    CleanupStack::PushL( object );

    CBufFlat *newMime = CBufFlat::NewL( KFlatMimeBufSize );
    CleanupStack::PushL( newMime );

#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
    CSmlDmAdapter::TError status = 
    	FetchLeafObjectL( uriPtrc, aLUID, aType, object, newMime );
#else    
    CSmlDmAdapter::TError status = 
    	FetchLeafObjectL( aURI, aLUID, aType, object, newMime );
#endif

    Callback().SetStatusL( aStatusRef, status );
    
    if( status == CSmlDmAdapter::EOk)
        {
        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> numBuf;
        object->Compress();
        
        CBufFlat *object2 = CBufFlat::NewL( KFlatBufSize );
        CleanupStack::PushL( object2 );
        
        numBuf.Num(object->Size());
        object2->InsertL(0, numBuf);
        object->Compress();
        Callback().SetResultsL( aResultsRef,*object2, KSmlTCTextPlain() );
        
        CleanupStack::PopAndDestroy( object2 );
        }

    CleanupStack::PopAndDestroy( newMime );
    CleanupStack::PopAndDestroy( object );
    }

// -------------------------------------------------------------------
//  CTcAdapter::_ChildURIListL( const TDesC8& aURI, 
//     const TDesC8& aParentLUID, const CArrayFix<TSmlDmMappingInfo>& 
// 			aPreviousURISegmentList, CArrayFix<TPtrC>& aCurrentURISegmentList )
// -------------------------------------------------------------------
void CTcAdapter::_ChildURIListL( const TDesC8& aURI, 
				 const TDesC8& /*aParentLUID*/,
         const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/,
         const TInt aResultsRef, const TInt aStatusRef  )
    {
    RDEBUG("CTcAdapter::_ChildURIListL()");

    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    CBufFlat *currentList = CBufFlat::NewL( KFlatBufSize );
    CleanupStack::PushL( currentList );

    User::LeaveIfError( CheckTerminalControlSession() );

#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
    CTcAdapter::TTcNodeIdentifier identifier = GetNodeIdentifier(uriPtrc);
#else    
    CTcAdapter::TTcNodeIdentifier identifier = GetNodeIdentifier(aURI);
#endif

    switch(identifier)
        {
            case ETcNodeTerminalControl:
            currentList->InsertL(currentList->Size(), KTCFileScanNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCFileDeleteNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCDeviceLockNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCDeviceWipeNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCProcessesNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCRebootNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), 
            	KTCLocalOperationsNodeName());
            break;

            case ETcNodeFileScan:
#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
            currentList->InsertL(currentList->Size(), 
            	KTCFileScanExecuteNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
#endif
            currentList->InsertL(currentList->Size(), 
            	KTCFileScanResultsNodeName());
            break;

#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
            case ETcNodeFileScanExecute:
            // nothing
            break;
#endif

            case ETcNodeFileScanResults:
            // nothing
            break;

            case ETcNodeFileDelete:
            // nothing
            break;

            case ETcNodeDeviceLock:
            currentList->InsertL(currentList->Size(), 
            	KTCAutoLockPeriodNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCLockLevelNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCLockCodeNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), 
            	KTCMaxAutoLockPeriodNodeName());
            break;

            case ETcNodeAutoLockPeriod:
            case ETcNodeMaxAutoLockPeriod:
            case ETcNodeLockLevel:
            case ETcNodeLockCode:
            // nothing
            break;
            
            case ETcNodeDeviceWipe:
            // nothing
            break;            

// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

			case ETcNodeLockCodeMinLength:
        	case ETcNodeLockCodeMaxLength:
        	case ETcNodeLockCodeRequireUpperAndLower:
        	case ETcNodeLockCodeRequireCharsAndNumbers:
        	case ETcNodeLockCodeMaxRepeatedCharacters:
        	case ETcNodeLockCodeHistoryBuffer:
        	case ETcNodeLockCodeExpiration:
        	case ETcNodeLockCodeMinChangeTolerance:
        	case ETcNodeLockCodeMinChangeInterval:
        	case ETcNodeLockCodeCheckSpecificStrings:
        	case ETcNodeLockCodeDisallowSpecific:
        	case ETcNodeLockCodeAllowSpecific:
        	case ETcNodeLockCodeClearSpecificStrings:
        	case ETcNodeLockCodeMaxAttempts:
        	case ETcNodeLockConsecutiveNumbers:
            case ETcNodeLockCodeMinSpecialCharacters:
            case ETcNodeLockDisallowSimple:                
			// nothing
        	break;

//#endif
// --------- Enhanced features END --------------------
            case ETcNodeProcesses:
            iTCSession.GetProcessListL( currentList );
            break;

            case ETcNodeProcessesX:
            currentList->InsertL(currentList->Size(), KTCIDNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCFromRAMNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCMemoryInfoNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), 
            	KTCProcessFilenameNodeName());
            break;

            case ETcNodeID:
            case ETcNodeFromRAM:
            case ETcNodeMemoryInfo:
            case ETcNodeFilename:
            // nothing
            break;

            case ETcNodeReboot:
            // nothing
            break;

            case ETcNodeLocalOperations:
            currentList->InsertL(currentList->Size(), KTCStartNodeName());
            currentList->InsertL(currentList->Size(), KTCSeparator8());
            currentList->InsertL(currentList->Size(), KTCStopNodeName());
            break;

            case ETcNodeStart:
            case ETcNodeStop:
            // nothing
            break;

            default:
            if( aURI.Length() == 0)
                {
                currentList->InsertL(currentList->Size(), KTCNodeName());                
                }
            else
                {
                status = CSmlDmAdapter::EError;
                }
            break;
        }

    Callback().SetStatusL( aStatusRef, status );

    if( status == CSmlDmAdapter::EOk )
        {
        currentList->Compress();
        Callback().SetResultsL( aResultsRef, *currentList, KNullDesC8 );
        }

    CleanupStack::PopAndDestroy( currentList );

    }

// ----------------------------------------------------------------
// CTcAdapter::_AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID )
// ----------------------------------------------------------------
void CTcAdapter::_AddNodeObjectL( const TDesC8& /*aURI*/, 
																	const TDesC8& /*aParentLUID*/,
                                  const TInt aStatusRef )
    {
    RDEBUG("CTcAdapter::_AddNodeObjectL()");

    CSmlDmAdapter::TError ret = CSmlDmAdapter::EError;
    Callback().SetStatusL( aStatusRef, ret );
    }

// ---------------------------------------------------------------
// CTcAdapter::_ExecuteCommandL
// ---------------------------------------------------------------
void CTcAdapter::_ExecuteCommandL( const TDesC8& aURI, 
																	 const TDesC8& aLUID,
                                   const TDesC8& aArgument, 
                                   const TDesC8& aType,
                                   TInt aStatusRef )
    {
    RDEBUG("CTcAdapter::_ExecuteCommandL()");

#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
    TRAPD( err, _ExecuteCommandToBeTrappedL( uriPtrc, aLUID, aArgument, aType, aStatusRef ) );
#else    
    TRAPD( err, _ExecuteCommandToBeTrappedL( aURI, aLUID, aArgument, aType, aStatusRef ) );
#endif

	CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
	
	if( err != KErrNone )
		{
		status = CSmlDmAdapter::EError;
		}
		
	Callback().SetStatusL( aStatusRef, status );
    }

void CTcAdapter::_ExecuteCommandToBeTrappedL( const TDesC8& aURI, 
																						const TDesC8& /*aLUID*/,
                                  			  	const TDesC8& aArgument, 
                                  			  	const TDesC8& /*aType*/,
                                    		  	TInt /*aStatusRef*/ )
    {
    RDEBUG("CTcAdapter::_ExecuteCommandToBeTrappedL()");

    TFileName8 startPath;
    TBool recursiveScan;

    User::LeaveIfError( CheckTerminalControlSession() );
    CTcAdapter::TTcNodeIdentifier identifier = GetNodeIdentifier(aURI);

    if((iAccessTypeList[identifier] & CTcAdapter::EExecute) == 
    	CTcAdapter::EExecute)
        {
        iAccessHistory[identifier] |= CTcAdapter::EExecute;

        switch(identifier)
            {
                case ETcNodeFileScan:
#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
                case ETcNodeFileScanExecute:
#endif
                ParseStartPathL(aArgument, startPath, recursiveScan);
                User::LeaveIfError( 
                	iTCSession.FileScan(startPath, recursiveScan) );
                break;
                
                case ETcNodeFileDelete:
                User::LeaveIfError( iTCSession.DeleteFile(aArgument) );
                break;

                case ETcNodeDeviceWipe:
                User::LeaveIfError( iTCSession.DeviceWipe() );
                break;

// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

            	case ETcNodeLockCodeClearSpecificStrings:
            		{
            		User::LeaveIfError( 
            			iTCSession.ClearSpecificPasscodeStrings() );
            		}
            	break;

//#endif
// --------- Enhanced features END --------------------

                case ETcNodeReboot:
                User::LeaveIfError( iTCSession.RebootDevice() );
                break;

                case ETcNodeStart:
                StartProcessL(aArgument);
                break;

                case ETcNodeStop:
                StopProcessL(aArgument);
                break;

                default:
                	{
                	User::Leave( KErrNotSupported );
                	}
                break;
            }
        }
        else
            {
            User::Leave( KErrNotSupported );
            }      
    }

// ------------------------------------------------------------------
// CTcAdapter::_ExecuteCommandL
// ------------------------------------------------------------------
void CTcAdapter::_ExecuteCommandL( const TDesC8& /*aURI*/, 
																	 const TDesC8& /*aLUID*/,
                                   RWriteStream*& /*aStream*/, 
                                   const TDesC8& /*aType*/,
                                   TInt aStatusRef )
    {
    RDEBUG("CTcAdapter::_ExecuteCommandL()");

    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }

// ----------------------------------------------------------------
// CTcAdapter::_CopyCommandL
// ----------------------------------------------------------------
void CTcAdapter::_CopyCommandL( const TDesC8& /*aTargetURI*/, 
																const TDesC8& /*aTargetLUID*/,
                               	const TDesC8& /*aSourceURI*/, 
                               	const TDesC8& /*aSourceLUID*/,
                               	const TDesC8& /*aType*/, 
                               	TInt aStatusRef )
    {
    RDEBUG("CTcAdapter::_CopyCommandL()");

    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    }

// ----------------------------------------------------------------
// CTcAdapter::StartAtomicL
// ----------------------------------------------------------------
void CTcAdapter::StartAtomicL()
    {
    RDEBUG("CTcAdapter::StartAtomicL()");

    memset(&iAccessHistory[0], CTcAdapter::ENoAccess, sizeof(iAccessHistory));    
    iAutoLockPeriod    = -1;
    iLockLevel         = -1;
    iMaxAutoLockPeriod = -1;
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
	iLockCodeMinLength					= -1;
	iLockCodeMaxLength					= -1;
	iLockCodeRequireUpperAndLowerInt	= -1;
	iLockCodeRequireCharsAndNumbersInt 	= -1;
	iLockCodeMaxRepeatedCharacters  	= -1;
	iLockCodeHistoryBuffer				= -1;
	iLockCodeExpiration					= -1;
	iLockCodeMinChangeTolerance			= -1;
	iLockCodeMinChangeInterval			= -1;
	iLockCodeCheckSpecificStringsInt	= -1;
	iLockCodeMaxAttempts					= -1;
	iLockCodeConsecutiveNumbersInt		= -1;
	iLockCodeMinSpecialCharacters      = -1;
    iLockCodeDisallowSimpleInt          = -1;
//#endif
// --------- Enhanced features END --------------------    
    }

// ---------------------------------------------------------------------
// CTcAdapter::CommitAtomicL
// ---------------------------------------------------------------------
void CTcAdapter::CommitAtomicL()
    {
    RDEBUG("CTcAdapter::CommitAtomicL()");

    memset(&iAccessHistory[0], CTcAdapter::ENoAccess, sizeof(iAccessHistory));    
    iAutoLockPeriod    = -1;
    iLockLevel         = -1;
    iMaxAutoLockPeriod = -1;
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS
	iLockCodeMinLength					= -1;
	iLockCodeMaxLength					= -1;
	iLockCodeRequireUpperAndLowerInt	= -1;
	iLockCodeRequireCharsAndNumbersInt 	= -1;
	iLockCodeMaxRepeatedCharacters  	= -1;
	iLockCodeHistoryBuffer				= -1;
	iLockCodeExpiration					= -1;
	iLockCodeMinChangeTolerance			= -1;
	iLockCodeMinChangeInterval			= -1;
	iLockCodeCheckSpecificStringsInt	= -1;
	iLockCodeMaxAttempts					= -1;
	iLockCodeConsecutiveNumbersInt		= -1;
    iLockCodeMinSpecialCharacters      = -1;
    iLockCodeDisallowSimpleInt          = -1;    
//#endif
// --------- Enhanced features END --------------------  
    
    // Note: possible reboot operation remains waiting in 
    // server until session is closed
    // - CommitAtomic() will not directly cause restart of device
    // - Restart is actually done when session is closed
    // - It is up to DM client to close session by destroying adapter
    }

// -------------------------------------------------------------------
// CTcAdapter::RollbackAtomicL
// -------------------------------------------------------------------
void CTcAdapter::RollbackAtomicL()
    {
    RDEBUG("CTcAdapter::RollbackAtomicL()");

    TInt i;

    // Not needed User::LeaveIfError( CheckTerminalControlSession() );

    if( iAccessHistory[ETcNodeReboot] & CTcAdapter::EExecute )
        {
        if( KErrNone == iTCSession.RebootDevice( EFalse ) )
            {
            iAccessHistory[ETcNodeReboot] &= (~CTcAdapter::EExecute);
            }
        }
    // NOw
    // Rollback should be done is certain order to 
    // prevent momentary illegal values
    // It is assumed that setting always succeeds in this order
    // iMaxAutoLockPeriod, iAutoLockPeriod, iLockLevel
    if( (iAccessHistory[ETcNodeMaxAutoLockPeriod] & CTcAdapter::EReplace) &&
        (-1 != iMaxAutoLockPeriod) )
        {
        if( KErrNone == iTCSession.SetMaxAutolockPeriod( iMaxAutoLockPeriod ))
            {
            iAccessHistory[ETcNodeMaxAutoLockPeriod] &= (~CTcAdapter::EReplace);
            }
        }
    if( (iAccessHistory[ETcNodeAutoLockPeriod] & CTcAdapter::EReplace) &&
        (-1 != iAutoLockPeriod) )
        {
        if( KErrNone == iTCSession.SetAutolockPeriod( iAutoLockPeriod ) )
            {
            iAccessHistory[ETcNodeAutoLockPeriod] &= (~CTcAdapter::EReplace);
            }
        }
    if( (iAccessHistory[ETcNodeLockLevel] & CTcAdapter::EReplace ) &&
        (-1 != iLockLevel) )
        {
        if( KErrNone == iTCSession.SetDeviceLockLevel( iLockLevel ) )
            {
            iAccessHistory[ETcNodeLockLevel] &= (~CTcAdapter::EReplace);
            }
        }

// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS

	if( ( iAccessHistory[ ETcNodeLockCodeMinLength ] & CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeMinLength ) )
        {
        if( KErrNone == iTCSession.SetPasscodeMinLength( iLockCodeMinLength ) )
            {
            iAccessHistory[ ETcNodeLockCodeMinLength ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }

	if( ( iAccessHistory[ ETcNodeLockCodeMaxLength ] & CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeMaxLength ) )
        {
        if( KErrNone == iTCSession.SetPasscodeMaxLength( iLockCodeMaxLength ) )
            {
            iAccessHistory[ ETcNodeLockCodeMaxLength ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }

	if( ( iAccessHistory[ ETcNodeLockCodeRequireUpperAndLower ] & 
		CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeRequireUpperAndLowerInt ) )
        {
        if( KErrNone == iTCSession.SetPasscodeRequireUpperAndLower( 
        	iLockCodeRequireUpperAndLower ) )
            {
            iAccessHistory[ ETcNodeLockCodeRequireUpperAndLower ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }
        
    if( ( iAccessHistory[ ETcNodeLockCodeRequireCharsAndNumbers ] & 
    	CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeRequireCharsAndNumbersInt ) )
        {
        if( KErrNone == iTCSession.SetPasscodeRequireCharsAndNumbers( 
        	iLockCodeRequireCharsAndNumbers ) )
            {
            iAccessHistory[ ETcNodeLockCodeRequireCharsAndNumbers ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }
        
	if( ( iAccessHistory[ ETcNodeLockCodeMaxRepeatedCharacters ] & 
		CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeMaxRepeatedCharacters ) )
        {
        if( KErrNone == iTCSession.SetPasscodeMaxRepeatedCharacters( 
        	iLockCodeMaxRepeatedCharacters ) )
            {
            iAccessHistory[ ETcNodeLockCodeMaxRepeatedCharacters ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }        
	
	if( ( iAccessHistory[ ETcNodeLockCodeHistoryBuffer ] & 
		CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeHistoryBuffer ) )
        {
        if( KErrNone == iTCSession.SetPasscodeHistoryBuffer( 
        	iLockCodeHistoryBuffer ) )
            {
            iAccessHistory[ ETcNodeLockCodeHistoryBuffer ] &= 
            	( ~CTcAdapter::EReplace );
            }
        } 	
	
	if( ( iAccessHistory[ ETcNodeLockCodeExpiration ] & 
		CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeExpiration ) )
        {
        if( KErrNone == iTCSession.SetPasscodeExpiration( 
        	iLockCodeExpiration ) )
            {
            iAccessHistory[ ETcNodeLockCodeExpiration ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }
        
    if( ( iAccessHistory[ ETcNodeLockCodeMinChangeTolerance ] & 
    	CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeMinChangeTolerance ) )
        {
        if( KErrNone == iTCSession.SetPasscodeMinChangeTolerance( 
        	iLockCodeMinChangeTolerance ) )
            {
            iAccessHistory[ ETcNodeLockCodeMinChangeTolerance ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }
        
    if( ( iAccessHistory[ ETcNodeLockCodeMinChangeInterval ] & 
    	CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeMinChangeInterval ) )
        {
        if( KErrNone == iTCSession.SetPasscodeMinChangeInterval( 
        	iLockCodeMinChangeInterval ) )
            {
            iAccessHistory[ ETcNodeLockCodeMinChangeInterval ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }
        
    if( ( iAccessHistory[ ETcNodeLockCodeCheckSpecificStrings ] & 
    	CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeCheckSpecificStringsInt ) )
        {
        if( KErrNone == iTCSession.SetPasscodeCheckSpecificStrings( 
        	iLockCodeCheckSpecificStrings ) )
            {
            iAccessHistory[ ETcNodeLockCodeCheckSpecificStrings ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }
        
    if( ( iAccessHistory[ ETcNodeLockCodeMaxAttempts ] & 
    	CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeMaxAttempts ) )
        {
        if( KErrNone == 
        	iTCSession.SetPasscodeMaxAttempts( iLockCodeMaxAttempts ) )
            {
            iAccessHistory[ ETcNodeLockCodeMaxAttempts ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }
        
    if( ( iAccessHistory[ ETcNodeLockConsecutiveNumbers ] & 
    	CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeConsecutiveNumbersInt ) )
        {
        if( KErrNone == iTCSession.SetPasscodeConsecutiveNumbers( 
        	iLockCodeConsecutiveNumbers ) )
            {
            iAccessHistory[ ETcNodeLockConsecutiveNumbers ] &= 
            	( ~CTcAdapter::EReplace );
            }
        }

    if( ( iAccessHistory[ ETcNodeLockCodeMinSpecialCharacters ] & 
        CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeMinSpecialCharacters ) )
        {
        if( KErrNone == iTCSession.SetPasscodeMinSpecialCharacters( 
            iLockCodeMinSpecialCharacters ) )
            {
            iAccessHistory[ ETcNodeLockCodeMinSpecialCharacters ] &= 
                ( ~CTcAdapter::EReplace );
            }
        }        
    
    if( ( iAccessHistory[ ETcNodeLockDisallowSimple ] & 
        CTcAdapter::EReplace ) &&
        ( -1 != iLockCodeDisallowSimpleInt ) )
        {
        if( KErrNone == iTCSession.SetPasscodeDisallowSimple( 
            iLockCodeDisallowSimple ) )
            {
            iAccessHistory[ ETcNodeLockDisallowSimple ] &= 
                ( ~CTcAdapter::EReplace );
            }
        }

//#endif
// --------- Enhanced features END --------------------  


    for(i=0; i<ETcNodeNotUsedAndAlwaysLast; i++)
        {
        if(iAccessHistory[i] == CTcAdapter::EExecute) break;
        if(iAccessHistory[i] == CTcAdapter::EReplace) break;
        }


    if(i == ETcNodeNotUsedAndAlwaysLast)
        {
        Callback().SetStatusL( 0, CSmlDmAdapter::EOk );
        }
    else
        {
        Callback().SetStatusL( 0, CSmlDmAdapter::ERollbackFailed );
        }
        
    // Note: access history can remain dirty
    } 
    

// ----------------------------------------------------------------------
// CTcAdapter::StreamingSupport
// -----------------------------------------------------------------------
TBool CTcAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    RDEBUG("CTcAdapter::StreamingSupport()");

    return EFalse;
    }
    
// ------------------------------------------------------------------------
// CTcAdapter::StreamCommittedL
// ------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY	
void CTcAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void CTcAdapter::StreamCommittedL()
#endif	
	{	
	RDEBUG("CTcAdapter::StreamCommittedL(): begin");
	RDEBUG("CTcAdapter::StreamCommittedL(): end");
	}
    
// --------------------------------------------------------------------------
// CTcAdapter::CompleteOutstandingCmdsL
// --------------------------------------------------------------------------
void CTcAdapter::CompleteOutstandingCmdsL()
    {        
    RDEBUG("CTcAdapter::CompleteOutstandingCmdsL()");
    }

// -------------------------------------------------------------------------
// CTcAdapter::StartProcessL
// -------------------------------------------------------------------------
TInt CTcAdapter::StartProcessL(const TDesC8& aURI)
    {
    RDEBUG("CTcAdapter::StartProcessL()");

    TInt result = KErrNone;
    MSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    _LIT8(KLinkPostfix, "/ID");    
    TInt linkLength = aURI.Length() + KLinkPostfix().Length();    
    
    HBufC8* link = HBufC8::NewLC( linkLength );    
    TPtr8 linkPtr( link->Des() );
    linkPtr.Copy( aURI );
    linkPtr.Append( KLinkPostfix() );
    
    CBufFlat *object = CBufFlat::NewL( KFlatBufSize );
    CleanupStack::PushL( object );
    
    TRAP( result, Callback().FetchLinkL( linkPtr, *object, status ) );

    if( (KErrNone != result) || (
    	CSmlDmAdapter::EOk != status) || (object->Size() <= 0))
        {
        // Didn't find UID, argument is filename
        //
        result = iTCSession.StartNewProcess( EFalse, aURI );
        }
    else
        {
        // Argument is UID
        //
        TLex8 lex;
        TUint uid;
        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> uidBuf;

        lex.Assign( object->Ptr(0) );
        User::LeaveIfError( lex.Val( uid, EHex ) );
        uidBuf.Num( uid );

        result = iTCSession.StartNewProcess( ETrue, uidBuf );
        }

    CleanupStack::PopAndDestroy( object );
    CleanupStack::PopAndDestroy( link );

    return result;
    }

// -----------------------------------------------------------------------
// CTcAdapter::StopProcessL
// -----------------------------------------------------------------------
TInt CTcAdapter::StopProcessL(const TDesC8& aURI)
    {
    RDEBUG("CTcAdapter::StopProcessL()");

    TInt result = KErrNone;
    MSmlDmAdapter::TError status;

    _LIT8(KLinkPostfix, "/ID");    
    TInt linkLength = aURI.Length() + KLinkPostfix().Length();    
    
    HBufC8* link = HBufC8::NewLC( linkLength );    
    TPtr8 linkPtr( link->Des() );
    linkPtr.Copy( aURI );
    linkPtr.Append( KLinkPostfix() );
    
    CBufFlat *object = CBufFlat::NewL( KFlatBufSize );
    CleanupStack::PushL( object );
    
    TRAP( result, Callback().FetchLinkL( linkPtr, *object, status ) );

    if( (KErrNone != result) || (
    	CSmlDmAdapter::EOk != status) || (object->Size() <= 0) )
        {
        // Didn't find UID, argument is filename
        //
        result = iTCSession.StopProcess( EFalse, aURI );
        }
    else
        {
        // Argument is UID
        //
        TLex8 lex;
        TUint uid;
        TBuf8<MAX_NUMBER_OF_DIGITS_IN_10BASE_INT64> uidBuf;

        lex.Assign( object->Ptr(0) );
        User::LeaveIfError( lex.Val( uid, EHex ) );
        uidBuf.Num( uid );

        //result = iTCSession.StopProcess( ETrue, object->Ptr(0) );
        result = iTCSession.StopProcess( ETrue, uidBuf );
        }

    CleanupStack::PopAndDestroy( object );
    CleanupStack::PopAndDestroy( link );

    return result;
    }

#ifndef __WINS_DEBUG_TESTING__

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
    {
    { {KSmlDMTCAdapterImplUid}, (TProxyNewLPtr)CTcAdapter::NewL }
    };

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
	TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
    	sizeof( TImplementationProxy );
    return ImplementationTable;
    }

#endif

// -----------------------------------------------------------------------
//
// ------------------------------------------------------------------------
void CTcAdapter::ConstructL( MSmlDmCallback* /*aDmCallback*/ )
    {
    FeatureManager::InitializeLibL();
	if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement) &&
			 FeatureManager::FeatureSupported(KFeatureIdSapTerminalControlFw)	)		
	{
    RDEBUG("CTcAdapter::ConstructL()");

    // Connect to Terminal Control
    User::LeaveIfError( iTC.Connect() );
    if( KErrNone != NewTerminalControlSession() )
        {
            // Note: connect to tc will fail if dmsession is not open
            RDEBUG("CTcAdapter::ConstructL() WARNING: Initial connection \
            	to Terminal Control Server failed, I will try again later..." );
        }
     }
    else
    {
    	FeatureManager::UnInitializeLib();
   		User::Leave( KErrNotSupported );
  	}
  	FeatureManager::UnInitializeLib();
    }

// -------------------------------------------------------------------------
// CTcAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------
void CTcAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,
                                TSmlDmAccessTypes aAccTypes,
                                MSmlDmDDFObject::TOccurence aOccurrence,
                                MSmlDmDDFObject::TScope aScope, 
                                MSmlDmDDFObject::TDFFormat aFormat,
                                const TDesC8& aDescription,
                                const TDesC8& aMimeType)
    {
    RDEBUG("CTcAdapter::FillNodeInfoL()");

    aNode.SetAccessTypesL(aAccTypes);
    aNode.SetOccurenceL(aOccurrence);
    aNode.SetScopeL(aScope);
    aNode.SetDFFormatL(aFormat);
    aNode.SetDescriptionL(aDescription);
    if(aMimeType.Length() > 0)
        {    
        aNode.AddDFTypeMimeTypeL(aMimeType);
        }    
    }
    
// --------------------------------------------------------------------------
// CTcAdapter::CopyAndTrimMimeType()
// --------------------------------------------------------------------------
void CTcAdapter::CopyAndTrimMimeType(TDes8& aNewMime, const TDesC8& aType)
    {
    RDEBUG("CTcAdapter::CopyAndTrimMimeType()");

    if(aNewMime.MaxLength() < aType.Length())
        {
        aNewMime.Copy( aType.Left(aNewMime.MaxLength()) );
        }
    else
        {
        aNewMime.Copy( aType );           
        }        
    aNewMime.TrimAll();
    aNewMime.LowerCase();
    
    TInt pos;
    while(( pos = aNewMime.Locate(' ')  ) != KErrNotFound)
        {
            aNewMime.Delete(pos, 1);
        }
    }

// -----------------------------------------------------------------------
// CTcAdapter::CopyAndTrimMimeType()
// -----------------------------------------------------------------------
void CTcAdapter::CopyAndTrimURI(TDes8& aNewURI, const TDesC8& aURI)
    {
    RDEBUG("CTcAdapter::CopyAndTrimURI()");

    if(aNewURI.MaxLength() < aURI.Length())
        {
        aNewURI.Copy( aURI.Left(aNewURI.MaxLength()) );
        }
    else
        {
        aNewURI.Copy( aURI );           
        }
    aNewURI.LowerCase();

/* 

    aNewURI.TrimAll();
    


    TInt pos;
    while(( pos = aNewURI.Locate(' ')  ) != KErrNotFound)
        {
            aNewURI.Delete(pos, 1);
        }
*/        

    while(aNewURI.Left(1) == _L8("/"))
        {
            aNewURI = aNewURI.Right(aNewURI.Length()-1);
        }
    }

// ---------------------------------------------------------------------------
// CTcAdapter::GetNodeIdentifier
// ---------------------------------------------------------------------------
CTcAdapter::TTcNodeIdentifier CTcAdapter::GetNodeIdentifier(const TDesC8& aURI)
    {
    RDEBUG("CTcAdapter::GetNodeIdentifier()");

    TInt    numOfSegs = NSmlDmURI::NumOfURISegs( aURI ) ;
    TPtrC8I seg1 = NSmlDmURI::URISeg(aURI, 0);

    if(seg1 == KTCNodeName)
        {
        if(numOfSegs == 1) return CTcAdapter::ETcNodeTerminalControl;

        TPtrC8I seg2 = NSmlDmURI::URISeg(aURI, 1);

        if(seg2 == KTCFileScanNodeName)
            {
                TPtrC8I seg3 = NSmlDmURI::URISeg(aURI, 2);

                if(numOfSegs == 2)
                    {
                    return CTcAdapter::ETcNodeFileScan;
                    }
                    else
                if((numOfSegs == 3) && (seg3 == KTCFileScanResultsNodeName))
                    {
                    return CTcAdapter::ETcNodeFileScanResults;
                    }
#ifdef TCADAPTER_FILESCAN_EXECUTE_LEAF
                else if((numOfSegs == 3) && (seg3 == KTCFileScanExecuteNodeName))
                    {
                    return CTcAdapter::ETcNodeFileScanExecute;
                    }
#endif
                    else
                        {
                        return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                        }
            }
            else
        if(seg2 == KTCFileDeleteNodeName)
            {
            return CTcAdapter::ETcNodeFileDelete;
            }
            else
        if(seg2 == KTCDeviceLockNodeName)					
            {
                if(numOfSegs == 2)
                    {
                    // get TerminalControl/DeviceLock
                    return CTcAdapter::ETcNodeDeviceLock;
                    }
                    else
                if(numOfSegs == 3)
                    {
                    TPtrC8I seg3 = NSmlDmURI::URISeg(aURI, 2);

                    if(seg3 == KTCAutoLockPeriodNodeName)
                        {
                        return CTcAdapter::ETcNodeAutoLockPeriod;
                        }
                    if(seg3 == KTCMaxAutoLockPeriodNodeName)
                        {
                        return CTcAdapter::ETcNodeMaxAutoLockPeriod;
                        }
                        else
                    if(seg3 == KTCLockLevelNodeName)
                        {
                        return CTcAdapter::ETcNodeLockLevel;
                        }
                        else
                    if(seg3 == KTCLockCodeNodeName)
                        {
                        return CTcAdapter::ETcNodeLockCode;
                        }
// --------- Enhanced features BEGIN ------------------
//#ifdef __SAP_DEVICE_LOCK_ENHANCEMENTS    

					if( seg3 == KTCLockCodeMinLengthNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeMinLength;
                        }
                    if( seg3 == KTCLockCodeMaxLengthNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeMaxLength;
                        }
                    if( seg3 == KTCLockCodeRequireUpperAndLowerNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeRequireUpperAndLower;
                        }
                    if( seg3 == KTCLockCodeRequireCharsAndNumbersNodeName )
                        {
                        return 
                        	CTcAdapter::ETcNodeLockCodeRequireCharsAndNumbers;
                        }
                    if( seg3 == KTCLockCodeMaxRepeatedCharactersNodeName )
                        {
                        return 
                        	CTcAdapter::ETcNodeLockCodeMaxRepeatedCharacters;
                        }
                    if( seg3 == KTCLockCodeHistoryBufferNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeHistoryBuffer;
                        }
                    if( seg3 == KTCLockCodeExpirationNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeExpiration;
                        }
                    if( seg3 == KTCLockCodeMinChangeToleranceNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeMinChangeTolerance;
                        }
                    if( seg3 == KTCLockCodeMinChangeIntervalNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeMinChangeInterval;
                        }
                    if( seg3 == KTCLockCodeCheckSpecificStringsNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeCheckSpecificStrings;
                        }
                    if( seg3 == KTCLockCodeDisallowSpecificNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeDisallowSpecific;
                        }
                    if( seg3 == KTCLockCodeAllowSpecificNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeAllowSpecific;
                        }
                    if( seg3 == KTCLockCodeClearSpecificStringsNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeClearSpecificStrings;
                        }
                    if( seg3 == KTCLockCodeMaxAttemptsNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeMaxAttempts;
                        }
                    if( seg3 == KTCLockCodeConsecutiveNumbersNodeName )
                        {
                        return CTcAdapter::ETcNodeLockConsecutiveNumbers;
                        }
                    if( seg3 == KTCLockCodeMinSpecialCharactersNodeName )
                        {
                        return CTcAdapter::ETcNodeLockCodeMinSpecialCharacters;
                        }
                    if( seg3 == KTCLockCodeDisallowSimpleNodeName )
                        {
                        return CTcAdapter::ETcNodeLockDisallowSimple;
                        }
                    
//#endif
// --------- Enhanced features END --------------------                      
                    else
                        {
                        return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                        }                        
                    }
                    else
                        {
                        return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                        }
            }
            else
        if(seg2 == KTCDeviceWipeNodeName)
            {
            return CTcAdapter::ETcNodeDeviceWipe;
            }
            else
        if(seg2 == KTCLocalOperationsNodeName)
            {
            if(numOfSegs == 2)
                {
                return CTcAdapter::ETcNodeLocalOperations;
                }
            if(numOfSegs == 3)
                {
                TPtrC8I seg3 = NSmlDmURI::URISeg(aURI, 2);
                
                if(seg3 == KTCStartNodeName)
                    {
                    return CTcAdapter::ETcNodeStart;
                    }
                    else
                if(seg3 == KTCStopNodeName)
                    {
                    return CTcAdapter::ETcNodeStop;
                    }
                    else
                        {
                        return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                        }
                }
                else
                    {
                    return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                    }
            }
            else
        if(seg2 == KTCProcessesNodeName)
            {
                if(numOfSegs == 2)
                    {
                    return CTcAdapter::ETcNodeProcesses;
                    }
                    else
                if(numOfSegs == 3)
                    {
                    return CTcAdapter::ETcNodeProcessesX;
                    }
                    else
                if(numOfSegs == 4)
                    {
                    TPtrC8I seg4 = NSmlDmURI::URISeg(aURI, 3);
                    if(seg4 == KTCIDNodeName)
                        {
                        return CTcAdapter::ETcNodeID;
                        }
                        else
                    if(seg4 == KTCFromRAMNodeName)
                        {
                        return CTcAdapter::ETcNodeFromRAM;
                        }
                        else
                    if(seg4 == KTCMemoryInfoNodeName)
                        {
                        return CTcAdapter::ETcNodeMemoryInfo;
                        }
                        else
                    if(seg4 == KTCProcessFilenameNodeName)
                        {
                        return CTcAdapter::ETcNodeFilename;
                        }
                        else
                            {
                            return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                            }
                    }
                    else
                        {
                        return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                        }
                
            }
            else
        if(seg2 == KTCRebootNodeName)
            {
            if(numOfSegs == 2)
                {                
                return CTcAdapter::ETcNodeReboot;
                }
                else
                    {
                    return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                    }
            }
            else
                {
                    return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
                }        
        }
        else
            {
            return CTcAdapter::ETcNodeNotUsedAndAlwaysLast;
            }

    // Note: do not return anything here
    }

// --------------------------------------------------------------------------
// CTcAdapter::NewTerminalControlSession
// ---------------------------------------------------------------------------  
TInt CTcAdapter::NewTerminalControlSession()
    {
    RDEBUG("CTcAdapter::NewTerminalControlSession()");

    TInt ret = KErrNone;

    if( iConnected )
        {
        iTCSession.Close();
        iConnected = EFalse;
        }

    TCertInfo ci;

/*#ifdef __WINS__

    // Use magic  word to get through terminal control certificate checking...
    // Only works in WINS...
    ci.iFingerprint = _L8("WINS-TESTING-ADAPTER");

#else*/

    RDMCert dmcert;
    ret = dmcert.Get( ci );

//#endif

    if( KErrNone == ret )
        {
        ret = iTCSession.Open(iTC, ci);
        if( KErrNone == ret )
            {
            iConnected = ETrue;
            }
        }

    return ret;
    }

// -------------------------------------------------------------------------
// CTcAdapter::CheckTerminalControlSession
// -------------------------------------------------------------------------- 
TInt CTcAdapter::CheckTerminalControlSession()
    {
    RDEBUG("CTcAdapter::CheckTerminalControlSession()");

    TInt ret = KErrNone;

    if( ! iConnected )
        {
        ret = NewTerminalControlSession();
        }

    return ret;
    }

// ------------------------------------------------------------------------
// CTcAdapter::PolicyRequestResourceL
// ------------------------------------------------------------------------- 
TPtrC8 CTcAdapter::PolicyRequestResourceL( const TDesC8& /*aURI*/ )
    {
    RDEBUG("CTcAdapter::PolicyRequestResourceL()");

    return PolicyEngineXACML::KTerminalSecurityManagement();
    }

// --------------------------------------------------------------------------
// CTcAdapter::ParseStartPathL
// ----------------------------------------------------------------------------  
void CTcAdapter::ParseStartPathL( const TDesC8 &aArguments, TDes8 &aStartPath, 
		TBool &aRecursive)
    {
    RDEBUG("CTcAdapter::ParseStartPathL()");

    //
    // Check for '-r' at the end of arguments
    //
    TPtrC8 right( aArguments.Right(2) );
    if( right == _L8("-r") )
        {
        if(aArguments.Length()-2 <= aStartPath.MaxSize())
            {            
            aStartPath.Copy(aArguments.Left(aArguments.Length()-2));
            }
        else
            {
            aStartPath.Copy(aArguments.Left(aStartPath.MaxSize()));
            }
        aRecursive = ETrue;        
        return;        
        }

    //
    // -r not found, argument is startpath
    //
    if(aArguments.Length() <= aStartPath.MaxSize())
        {            
        aStartPath.Copy(aArguments);
        }
    else
        {
        aStartPath.Copy(aArguments.Left(aStartPath.MaxSize()));
        }
    aRecursive = EFalse;

    }



