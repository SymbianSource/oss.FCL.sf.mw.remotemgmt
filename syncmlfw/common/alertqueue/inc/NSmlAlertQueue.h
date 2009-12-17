/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: SyncML alert parser and message handler.   
*
*/


#ifndef __NSMLALERTQUEUE_H__
#define __NSMLALERTQUEUE_H__

#include <e32base.h>
#include <e32std.h>
#include <SyncMLDef.h>
#include <SyncMLHistory.h>
#include <SyncMLAlertInfo.h>
#include <utf.h>

#include <nsmlunicodeconverter.h>
#include "nsmldssettings.h"
#include "nsmldmsettings.h"
#include "NSmlHistoryArray.h"

#include "WBXMLSyncMLParser.h"
#include "WBXMLParser.h"
#include "WBXMLParserError.h"
#include "smlmetinfdtd.h"
#include "NSmlURI.h"
#include "NSmlAlertQueueDefs.h"



//container for profile related alert info
class CNSmlAlertInfo : public CBase
	{
	public:
		~CNSmlAlertInfo();
		TInt iProfileId;
		TInt iMatchCount;
		TInt iIAPId;
		RPointerArray<TNSmlContentTypeInfo> iTaskInfo;
	};
	
//Base class for server alert parsers
class CNSmlMessageParserBase : public CBase
	{
	public:
	
		/**
		* Constuctor
        * CNSmlMessageParserBase( CSmlAlertInfo& aAlertInfo,
        * CSyncMLHistoryPushMsg& aHistoryInfo )
        * @param aAlertInfo - reference to CSmlAlertInfo : container for hold
        *                       parsed data
        * @param aHistoryInfo - reference to CSyncMLHistoryPushMsg : container
        *                       for hold parsed data
		* @return - 
        */
		CNSmlMessageParserBase( CSmlAlertInfo& aAlertInfo, 
		    CSyncMLHistoryPushMsg& aHistoryInfo );
		
		/**
		* Destructor
        * ~CNSmlMessageParserBase()
        * @param -
		* @return - 
        */
		virtual ~CNSmlMessageParserBase();
		
		/**
		* Pure virtual method to implement the parsing
        * void ParseMessageL()
        * @param -
        * @return - 
        */
		virtual void ParseMessageL() = 0;
		
		/**
		* Pure virtual method to target a correct profile
        * void SearchProfileL(TDesC8& aServerUri)
        * @param aServerUri - Server Id identifier
        * @return - 
        */
		virtual void SearchProfileL( TDesC8& aServerUri ) = 0;
		
		/**
		* Method to target correct content type
        * void MatchContentTypeL( TDesC8& aPath )
        * @param aPath - Path to local database
        * @return - 
        */
		virtual void MatchContentTypeL( TDesC8& /*aPath*/ ) {	}
		
		/**
		* Returns a pointer to HBufC8 buffer
        * TPtr8& Message()
        * @param -
        * @return - Reference to HBufC8 buffer 
        */
		TPtr8& Message();
		
		/**
		* Allocates buffer of specified size
        * void CreateBufferL( TInt aSize )
        * @param aSize - size of buffer to hold the SAN message
        * @return -
        */
		void CreateBufferL( TInt aSize );
		
		/**
		* Resolves the profile if more than one match is found
        * void ResolveProfileL( TInt aContentCount )
        * @param aContentCount - Number of alerted contents
        * @return - 
        */
		virtual void ResolveProfileL( TInt /*aContentCount*/ ) { }
			
		/**
		* Leaves with KErrCorrupt if message length < given position
        * void CheckLengthL(TInt aPos);
        * @param aPos - Position in message
        * @return - 
        */
		virtual void CheckLengthL( TInt aPos );
		
		/**
		* Creates a pointer of the message to heap
        * TPtrC8 DoMessageCopy()
        * @param -
        * @return - Pointer to new message instance 
        */
		TPtrC8 DoMessageCopyLC();
		
	protected:
		CSmlAlertInfo& iAlertInfo;
		CSyncMLHistoryPushMsg& iHistoryInfo;
		RPointerArray<CNSmlAlertInfo> iFoundProfiles;
	private:
		HBufC8* iMessage;
		TPtr8 iMessagePtr;
	};

//Class for parsing DM server alert notification version 1.1
class CNSmlDSAlertParser11 : public CNSmlMessageParserBase, 
                            public MWBXMLSyncMLCallbacks
	{
	public:	
	
		/**
		* Constuctor
        * CNSmlDMAlertParser11( CSmlAlertInfo& aAlertInfo, 
        * CSyncMLHistoryPushMsg& aHistoryInfo )
        * @param aAlertInfo - reference to CSmlAlertInfo : container for hold
        *                       parsed data
        * @param aHistoryInfo - reference to CSyncMLHistoryPushMsg : container
        *                       for hold parsed data
		* @return - 
        */
		CNSmlDSAlertParser11( CSmlAlertInfo& aAlertInfo, 
                CSyncMLHistoryPushMsg& aHistoryInfo );
		
			/**
		* Destructor
        * ~CNSmlDSAlertParser11();
        * @param -
        * @return - 
        */
		virtual ~CNSmlDSAlertParser11();
		
		/**
		* Derived from base class. Implements the parsing.
        * void ParseMessageL()
        * @param -
        * @return -
        */
		virtual void ParseMessageL();
		
		/**
		* Derived from base class. Targets the profile using server id.
        * void SearchProfileL( TDesC8& aServerUri )
        * @param aServerUri - Server Id.
        * @return - 
        */
		virtual void SearchProfileL( TDesC8& aServerUri );		
		
		/**
		* Targets the profile using server id.
        * TBool SearchProfileURIL( CNSmlURI& aUri )
        * @param aServerUri - Server Id.
        * @return - ETrue if profile is found
        */
		TBool SearchProfileURIL( CNSmlURI& aUri );
	
		
		/**
		* Converts data in Pcdata structure
        * 	void PCDATAToUnicodeLC( const SmlPcdata_t& aPcdata, 
        *                           HBufC*& aUnicodeData ) const;
        * @param aServerUri - Server Id.
        * @return - 
        */
		void PCDATAToUnicodeLC( const SmlPcdata_t& aPcdata, 
		                        HBufC*& aUnicodeData ) const;
		
		/**
		* Resolves the profile if more than one match is found
        * void ResolveProfileL( TInt aContentCount )
        * @param aContentCount - Number of alerted contents
        * @return - 
        */
		virtual void ResolveProfileL( TInt aContentCount );
				
		/**
		* Derived from base class. Targets content type with 
		* remote database path + MimeType.
        * void MatchContentTypeL( TDesC8& aPath, TInt aContentType, 
        *                           TInt aSyncType )
        * @param aPath - Database path.
        * @param aContentType - Value of content type defined in SAN message
        * @param aSyncType - Sync type defined in SAN message for the content
        * @return -
        */
		void MatchContentTypeL( TDesC8& aPath, TDesC8& aContentType, 
		                        TSmlSyncType aSyncType, TInt aProfileIndex );
		
		/**
		* Targets content type with mime-type
        * TBool ConvertContentTypeL( TDesC& aMimeType, 
        *                            const TInt aDataProviderId )
        * @param aMimeType - Mime type of the data provider
        * @param aDataProviderId - Implementation UID of the data provider
        * @return - ETrue if content type is allowed 
        */
		TBool ConvertContentTypeL( TDesC& aMimeType, 
		                            const TInt aDataProviderId );
		
	private:
		
		void TrimRightSpaceAndNull (TDes8& aDes ) const;
		
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
	 	TInt iContentCount;
	 	SmlMetInfMetInf_t* iMetInf;
	};
	

//Class for parsing DM server alert notification version 1.1
class CNSmlDMAlertParser11 : public CNSmlMessageParserBase
	{
	public:	
	
		/**
		* Constuctor
        * CNSmlDMAlertParser11( CSmlAlertInfo& aAlertInfo, 
        * CSyncMLHistoryPushMsg& aHistoryInfo )
        * @param aAlertInfo - reference to CSmlAlertInfo : container for hold
        *                       parsed data
        * @param aHistoryInfo - reference to CSyncMLHistoryPushMsg : 
        *                       container for hold parsed data
		* @return - 
        */
		CNSmlDMAlertParser11( CSmlAlertInfo& aAlertInfo, 
		                        CSyncMLHistoryPushMsg& aHistoryInfo );
		
		/**
		* Destructor
        * ~CNSmlDMAlertParser11();
        * @param -
        * @return - 
        */
		virtual ~CNSmlDMAlertParser11();
		
		/**
		* Derived from base class. Implements the parsing.
        * void ParseMessageL()
        * @param -
        * @return -
        */
		virtual void ParseMessageL();
		
		/**
		* Derived from base class. Targets the profile using server id.
        * void SearchProfileL( TDesC8& aServerUri )
        * @param aServerUri - Server Id.
        * @return -
        */
		virtual void SearchProfileL( TDesC8& aServerUri );
		
		/**
		* Resolves the profile if more than one match is found
        * void ResolveProfileL(TInt aContentCount)
        * @param aContentCount - Number of alerted contents
        * @return - 
        */
		virtual void ResolveProfileL( TInt aContentCount );		
	};
	
//Class for parsing DS server alert notification version 1.2
class CNSmlDSAlertParser12 : public CNSmlMessageParserBase
	{
	public:	
		
		/**
		* Constuctor
        * CNSmlDSAlertParser12( CSmlAlertInfo& aAlertInfo, 
        * CSyncMLHistoryPushMsg& aHistoryInfo )
        * @param aAlertInfo - reference to CSmlAlertInfo : container for hold
        *                       parsed data
        * @param aHistoryInfo - reference to CSyncMLHistoryPushMsg : 
                                container for hold parsed data
		* @return - 
        */
		CNSmlDSAlertParser12( CSmlAlertInfo& aAlertInfo, 
		                        CSyncMLHistoryPushMsg& aHistoryInfo );
		
		/**
		* Destructor
        * ~CNSmlDSAlertParser12();
        * @param -
        * @return - 
        */
		virtual ~CNSmlDSAlertParser12();
		
		/**
		* Derived from base class. Implements the parsing.
        * void ParseMessageL()
        * @param -
        * @return -
        */
		virtual void ParseMessageL();
		
		/**
		* Derived from base class. Targets the profile using server id.
        * void SearchProfileL( TDesC8& aServerUri )
        * @param aServerUri - Server Id.
        * @return - 
        */
		virtual void SearchProfileL( TDesC8& aServerUri );
		
		/**
		* Derived from base class. Targets content type with remote database
		* path + MimeType.
        * void MatchContentTypeL( TDesC8& aPath, TInt aContentType, 
        *    TSmlSyncType aSyncType, TInt aProfileIndex )
        * @param aPath - Database path.
        * @param aContentType - Value of content type defined in SAN message
        * @param aSyncType - Sync type defined in SAN message for the content
        * @param aProfileIndex - index of the profile data container
        * @return -
        */
		virtual void MatchContentTypeL( TDesC8& aPath, TInt aContentType, 
		                        TSmlSyncType aSyncType, TInt aProfileIndex );
		
		/**
		* Resolves the profile if more than one match is found
        * void ResolveProfileL( TInt aContentCount )
        * @param aContentCount - Number of alerted contents
        * @return - 
        */
        virtual void ResolveProfileL( TInt aContentCount );
        
	private:
		
		/**
		* Derived from base class. Targets content type with mime-type.
        * TBool ConvertContentTypeLC( const TInt aContentNum, 
        * const TInt aDataProviderId )
        * @param aContentNum - Value of content type defined in SAN message
        * @param aDataProviderId - UID of the content type
        * @return - ETrue if content type is found
        */
		TBool ConvertContentTypeL( const TInt aContentNum, 
		                            const TInt aDataProviderId );
		
		/**
		* Derived from base class. Implements the parsing.
        * void ValidateSyncType( TInt& aSyncType )
        * @param aSyncType - received sync type
        * @return -
        */
        void ValidateSyncType( TInt& aSyncType );
	};


class NSmlParserFactory
	{
	public:
	
		/**
		* Factory method to create appropriate alert parser
        * CreateAlertParserL( TSmlUsageType aType, TSmlProtocolVersion aVersion
        * , CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo  )
        * @param aType - Specifies the usage type DM/DS
        * @param aVersion - Specifies the protocol version
        * @param aAlertInfo - reference to CSmlAlertInfo instance
       	* @param aHistoryInfo - reference to CSyncMLHistoryPushMsg instance
        * @return - instance of CNSmlMessageParserBase derived class
        */
		static CNSmlMessageParserBase* CreateAlertParserL( TSmlUsageType aType,
		    TSmlProtocolVersion aVersion, CSmlAlertInfo& aAlertInfo, 
            CSyncMLHistoryPushMsg& aHistoryInfo  );	
	};


// container of the alert message. Item of alert message queue
class CNSmlMessageItem : public CBase
	{
	public:
	
		/**
		* Two phase constructor
        * CNSmlMessageItem* NewL( const TDesC8& aMessage, 
        * TSmlUsageType aType, TSmlProtocolVersion aVersion, 
        * TSmlTransportId aBearerType )
        * @param aMessage - reference to message 
        * @param aType - Defines if the message is DM or DS
        * @param aVersion - specifies used DS or DM protocol version
        * @param aBearerType - specifies used bearer type
		* @return - instance of CNSmlMessageItem 
        */
		IMPORT_C static CNSmlMessageItem* NewL( const TDesC8& aMessage,
		                TSmlUsageType aType, TSmlProtocolVersion aVersion, 
		                TSmlTransportId aBearerType );
		
		/**
		* Destructor
        * ~CNSmlMessageItem();
        * @param -
        * @return -
        */
		~CNSmlMessageItem();
		
		static const TInt iOffset;
		
		/**
		* returns reference to message
        * TDesC8& Message()
        * @param -
        * @return -
        */
		IMPORT_C TDesC8& Message();
				
	private:
	
		/**
		* Constructor
        * CNSmlMessageItem();
        * @param -
        * @return -
        */
		CNSmlMessageItem();
		
		/**
		* Second constructor
        * ConstructL( const TDesC8& aMessage, TSmlUsageType aType, 
        * TSmlProtocolVersion aVersion, TSmlTransportId aBearerType );
        * @param aMessage - reference to message 
        * @param aType - Defines if the message is DM or DS
        * @param aVersion - specifies used DS or DM protocol version
        * @param aBearerType - specifies used bearer type
        * @return -
        */
		void ConstructL( const TDesC8& aMessage, TSmlUsageType aType, 
                TSmlProtocolVersion aVersion, TSmlTransportId aBearerType );

	private:
		TDblQueLink iDlink;
		HBufC8* iMessage;
		TSmlUsageType iUsageType;
		TSmlProtocolVersion iProtocol;
		TSmlTransportId iBearerType;
		friend class CNSmlMessageQueue;	
	};

class MNSmlMessageHandler
	{
	public:
	
		/**
		* Checks if there is message in the queue
        * void CheckMessage( TBool& aMore, TSmlUsageType& aType, 
        * TSmlProtocolVersion& aVersion, TSmlTransportId& aBearerType  )
        * @param aMore - Set to ETrue if there is message in the queue
        * @param aType - Set to usage type of the message
        * @param aVersion - Set to used protocol version
        * @param aBearerType - specifies used bearer type
        * @return -
        */
		virtual void CheckMessage( TBool& aMore, TSmlUsageType& aType, 
		                TSmlProtocolVersion& aVersion, 
		                TSmlTransportId& aBearerType ) = 0;
		
		/**
		* Returns the length of the queued SAN message
        * TInt MessageSize()
        * @param -
        * @return TInt - size of the message
        */
		virtual TInt MessageSize() = 0;
		
		/**
		* Makes a copy of the message
        * void AlertMessage(TDes8& aMessage )
        * @param aMessage - reference of descriptor the message is copied to
        * @return -
        */
		virtual void AlertMessage( TDes8& aMessage ) = 0;
		
		/**
		* Creates a job
        * void CreateJobL( CSmlAlertInfo& aInfo, TBool& aQuit, 
        * const TPtrC8& aPackage )
        * @param aInfo - Container of the parsed alert info
        * @param aQuit - Returns whether there is next is message in queue
        * @param aPackage - Contains alert package if 1.1 version alert message
        * @return -
        */
		virtual void CreateJobL( CSmlAlertInfo& aInfo, TBool& aQuit, 
		                            const TPtrC8& aPackage  ) = 0;
		
		/**
		* Disconnects local connection.
		* void DoDisconnect()
		* @param -
        * @return -        
		*/		                            
		virtual void DoDisconnect() = 0;
	};							 


class CNSmlAlertHandler : public CActive
	{
	public:
	
		/**
		* Two phase constructor
        * CNSmlAlertHandler* NewL(MNSmlMessageHandler* aMsgHandler)
        * @param aMsgHandler - Pointer to the alert message handler
        * @return - new instance of CNSmlAlertHandler
        */
		static CNSmlAlertHandler* NewL( MNSmlMessageHandler* aMsgHandler );
		
		/**
		* Destructor
        * ~CNSmlAlertHandler()
        * @param -
        * @return - 
        */
		~CNSmlAlertHandler();
		
		/**
		* Activates alert handler
        * void ProcessAlert()
        * @param -
        * @return - 
        */
		void ProcessAlert();
		
		/**
		* Method from base class
        * void DoCancel()
        * @param -
        * @return - 
        */
		void DoCancel();
		
		/**
		* Method from base class. Parses the message.
        * void RunL()
        * @param -
        * @return - 
        */
		void RunL();
		
	private:
		
		/**
		* Fills the alert info before creating job
        * TBool FinalizeBeforeJobCreationL()
        * @param -
        * @return - ETrue if query is accepted
        */
		TBool FinalizeBeforeJobCreationL();
				
		/**
		* Checks whether alert has already been handled
        * void CheckDigestL(const TInt aProfileId, const TDesC8& aDigest)
        * @param aProfileId - Id of the profile
        * @param aDigest - Digest parsed from the SAN message
        * @return - 
        */
		void CheckDigestL( const TInt aProfileId, const TDesC8& aDigest );
		
		/**
		* Saves the result to profile history log and adds new entry
        * void SaveAlertInfoL(TBool aNewEntry);
        * @param aNewEntry - ETrue if new entry
        * @return - 
        */
		void SaveAlertInfoL();
		
		/**
		* Checks if profile has different version and changes to alert's
		* version if different.
        * void CheckProtocolAndChangeL( TSmlProtocolVersion& aVersion )
        * @param aVersion - used protocol version
        * @return - 
        */
		void CheckProtocolAndChangeL( TSmlProtocolVersion& aVersion ) const;
		
		/**
		* Parses the message.
        * void DoRunL()
        * @param -
        * @return - 
        */
		void DoRunL();
		
	private:
		
		/**
		* Constructor
        * CNSmlAlertHandler()
        * @param -
        * @return - 
        */
		CNSmlAlertHandler();
		
		/**
		* Second phase constructor
        * void ConstructL( MNSmlMessageHandler* aMsgHandler )
        * @param aMsgHandler - Pointer to MNSmlMessageHandler observer
        * @return - 
        */
		void ConstructL( MNSmlMessageHandler* aMsgHandler );
		
	private:
		MNSmlMessageHandler* iMsgHandler;
		CSyncMLHistoryPushMsg* iHistoryInfo;
		CSmlAlertInfo* iAlertInfo;
		CNSmlHistoryArray* iHistoryArray;
		CNSmlMessageParserBase* iAlertParser;
		TBool iNewEntry;
		HBufC* iPackage;
	};

class CNSmlMessageQueue : public CBase, public MNSmlMessageHandler
	{
	public:
		
		enum TNSmlQueueState
			{
			ENSmlIdle,
			ENSmlProcessing,
			ENSmlSuspended
			};
		/**
		* Two phase constructor
        * CNSmlMessageQueue* NewL( MNSmlAlertObserver* aObserver );
        * @param aMsgHandler - Pointer to the alert message handler
        * @return - new instance of CNSmlMessageQueue
        */
		IMPORT_C static CNSmlMessageQueue* NewL( MNSmlAlertObserver* aObserver );
		
		/**
		* Destructor
        * ~CNSmlMessageQueue()
        * @param -
        * @return - 
        */
		IMPORT_C virtual ~CNSmlMessageQueue();
		
		/**
		* Inserts new alert message to alert message queue
        * void AddMessageL( const TDesC8& aMessage, TSmlUsageType aType, 
        * TSmlProtocolVersion aVersion, TSmlTransportId aBearerType );
        * @param aMessage - Alert message
        * @param aType - Usage type of the message
        * @param aVersion - Protocol version of the message
        * @param aBearerType - specifies used bearer type
        * @return - 
        */
		IMPORT_C void AddMessageL( const TDesC8& aMessage, TSmlUsageType aType, 
		                       TSmlProtocolVersion aVersion, 
		                       TSmlTransportId aBearerType );
		
		/**
		* Check if queue is empty
        * TBool IsEmpty()
        * @param -
        * @return - ETrue if message queue is empty
        */
		IMPORT_C TBool IsEmpty();
		
		/**
        * void ServerSuspendedL( TBool aSuspend )
        * @param aSuspend - ETrue if server goes to suspend mode (back up)
		* @return - 
        */
		IMPORT_C void ServerSuspended( TBool aSuspend );
		
		/**
		* Method from the base class
        * void CheckMessage( TBool& aMore, TSmlUsageType& aType, 
        * TSmlProtocolVersion& aVersion, TSmlTransportId &aBearerType )
        * @param aMore - Set to ETrue if there is message in the queue
        * @param aType - Set to usage type of the message
        * @param aVersion - Set to used protocol version
        * @param aBearerType - specifies used bearer type
        * @return -
        */
		virtual void CheckMessage( TBool& aMore, TSmlUsageType& aType, 
		                        TSmlProtocolVersion& aVersion, 
		                        TSmlTransportId &aBearerType );
		
		/**
		* Method from the base class
        * TInt MessageSize()
        * @param -
        * @return TInt - size of the message
        */
		virtual TInt MessageSize();
		
		/**
		* Method from the base class
        * void AlertMessage( TDes8& aMessage )
        * @param aMessage - reference of descriptor the message is copied to
        * @return -
        */
		virtual void AlertMessage( TDes8& aMessage );
		
		/**
		* Method from the base class
        * void CreateJobL( CSmlAlertInfo& aInfo, TSmlUsageType aType, TBool& aQuit, const TPtrC8& aPackage )
        * @param aInfo - Container of the parsed alert info
        * @param aQuit - Returns whether there is next is message in queue
        * @param aPackage - pointer to the message
        * @return -
        */
		virtual void CreateJobL( CSmlAlertInfo& aInfo, TBool& aQuit, const TPtrC8& aPackage );
		
		/**
		* Disconnects local connection.
		* void DoDisconnect()
		* @param -
        * @return -        
		*/	
		virtual void DoDisconnect();
		
	private:
	
		/**
		* Constructor
        * CNSmlMessageQueue(MNSmlAlertObserver* aObserver)
        * @param aObserver - Pointer to the alert observer
        * @return - 
        */
		IMPORT_C CNSmlMessageQueue( MNSmlAlertObserver* aObserver );
		
		/**
		* Second phase constructor
        * void ConstructL()
        * @param aMsgHandler - Pointer to MNSmlMessageHandler observer
        * @return - 
        */
		void ConstructL();
		
		/**
		* Clears the queue
        * void DestroyItems()
        * @param -
        * @return - 
        */
		void DestroyItems();
		
	private:
		TDblQue<CNSmlMessageItem> iAlertQueue;
		TDblQueIter<CNSmlMessageItem> iAlertIterator;
		CNSmlAlertHandler* iAlertHandler;
		MNSmlAlertObserver* iAlertObserver; 
		TNSmlQueueState iState;		
	};

#endif