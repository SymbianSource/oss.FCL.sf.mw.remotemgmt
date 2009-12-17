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
* Description:  DS server alert handler
*
*/


#ifndef __NSMLDSALERT_H
#define __NSMLDSALERT_H

#include <e32base.h>
#include <nsmlsyncalerthandlerif.h>
#include "WBXMLSyncMLParser.h"

//used for SA message validation
_LIT8( KMdoAgentSupportedVersion, "NokiaInq/*" );

const TUint KNSmlDSAlertImplUid = 0x101F99EC; 
const TInt KNSmlDSAlertVersionPos = 16;
const TInt KNSmlDSAlertVersion = 12;
// CLASS DECLARATION

/**
*  CNSmlDSAlert 
*  
*/
class CNSmlDSAlert:public CNSmlSyncAlertHandler, public MWBXMLSyncMLCallbacks
	{
    public:  // Constructors and destructor
        
    static CNSmlDSAlert* NewL();
    ~CNSmlDSAlert();       //from CBase

    public: // New functions
        
	TNSmlAlertResult StartSyncL( const TDesC8& aSyncMLMessage,
			TInt aTransport,
            TBTDevAddr aBTAddress,
            const TDesC& aBTName,
            MNSmlAlertObexPluginContext* aPluginRes );
		
    private: //functions
	
	CNSmlDSAlert();
	CNSmlDSAlert( const CNSmlDSAlert& aOther );
	CNSmlDSAlert& operator=( const CNSmlDSAlert& aOther );

	void ParseMessageL( const TDesC8& aSyncMLMessage );
	TBool Is12Package( const TDesC8& aTriggerMessage ) const;
	
	void PCDATAToUnicodeLC( const SmlPcdata_t& aPcdata, HBufC*& aUnicodeData ) const;
	void TrimRightSpaceAndNull (TDes8& aDes ) const;
	
	// Functions from MWBXMLSyncMLCallbacks  

	Ret_t smlAlertCmdFuncL( SmlAlertPtr_t aContent);          
	Ret_t smlGetCmdFuncL( SmlGetPtr_t aContent );                      
	Ret_t smlPutCmdFuncL( SmlPutPtr_t aContent );            
	Ret_t smlResultsCmdFuncL( SmlResultsPtr_t aContent );   
	Ret_t smlStatusCmdFuncL( SmlStatusPtr_t aContent );       
	Ret_t smlStartMessageFuncL( SmlSyncHdrPtr_t aSyncHdr);    
	Ret_t smlEndMessageFuncL( Boolean_t final );           
	Ret_t smlStartSyncFuncL( SmlSyncPtr_t aContent );       
	Ret_t smlEndSyncFuncL();                               
	Ret_t smlStartAtomicFuncL( SmlAtomicPtr_t aContent );  
	Ret_t smlEndAtomicFuncL();                              
	Ret_t smlStartSequenceFuncL( SmlSequencePtr_t aContent ); 
	Ret_t smlEndSequenceFuncL();                              
	Ret_t smlAddCmdFuncL( SmlAddPtr_t aContent );          
   	Ret_t smlCopyCmdFuncL( SmlCopyPtr_t aContent);               
	Ret_t smlDeleteCmdFuncL( SmlDeletePtr_t aContent );    
	Ret_t smlExecCmdFuncL( SmlExecPtr_t aContent );        
	Ret_t smlMapCmdFuncL( SmlMapPtr_t aContent );          
	Ret_t smlReplaceCmdFuncL( SmlReplacePtr_t aContent );  
	Ret_t smlSearchCmdFuncL( SmlSearchPtr_t aContent );
    Ret_t smlMoveCmdFuncL( SmlMovePtr_t aContent );

    private:
    
    TBool iPacketShouldGoToMdo;
	};

#endif      // __NSMLDSALERT_H
            
// End of File
