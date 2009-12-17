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
* Description:  Status response buffering
*
*/


#ifndef __NSMLRESPONSECONTROLLER_H
#define __NSMLRESPONSECONTROLLER_H


// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CNSmlFutureReservation;

// CLASS DECLARATION
class CNSmlResponseController : public CBase 
	{
	public:  // enumerations
		enum TResponseDetail 
		{
		EResponseNoDetail,
		EResponseInitAlert
		};

	public: // constructors and desctructor
	static CNSmlResponseController* NewL(); 
	static CNSmlResponseController* NewLC();
	~CNSmlResponseController();    //from CBase

	public: //new functions
 
	TInt CreateNewResponseItemL();
	void RemoveResponseItem( TInt aMsgID, TInt aCmdID ); 
	void SetMsgID( TInt aEntryID, TInt aMsgID );
	void SetCmdL( TInt aEntryID, const TDesC8& aCmd );
	void SetCmdID( TInt aEntryID, TInt aCmdID );
	IMPORT_C void SetLUIDL( TInt aEntryID, const TDesC8& aLUID );
	IMPORT_C void SetAppIndex( TInt aEntryID, TInt aAppIndex );
	void SetChalTypeL( TInt aEntryID, const TDesC8& aType );
	void SetChalFormatL( TInt aEntryID, const TDesC8& aFormat );
	void SetChalNextNonceL( TInt aEntryID, const TDesC8& aNextNonce );
	IMPORT_C void SetResponseDetail( TInt aEntryID, TResponseDetail aResponseDetail );
	IMPORT_C void SetMoreData( TInt aEntryID );
	void RemoveAllReceivedOnes();  
	IMPORT_C void ResetL();
	TBool MatchStatusElement( const TDesC8& aMsgID, const TDesC8& aCmdID, TInt aStatusCode, TInt& aEntryID );

	void Begin();
	TBool NextResponseInfo( TInt& aEntryID );
	TBool ItemResponseInfo( const TDesC8& aLUID, TInt& aEntryID ) const;

	IMPORT_C TDesC8& Cmd( TInt aEntryID ) const;
	IMPORT_C TPtrC8 LUID( TInt aEntryID ) const;
	IMPORT_C TInt AppIndex( TInt aEntryID ) const;
	IMPORT_C TInt StatusCode( TInt aEntryID ) const;
	TBool StatusReceived( TInt aEntryID ) const;
	IMPORT_C TPtrC8 ChalType( TInt aEntryID ) const;
	TPtrC8 ChalFormat( TInt aEntryID ) const;
	TPtrC8 ChalNextNonce( TInt aEntryID ) const;
	IMPORT_C TResponseDetail ResponseDetail( TInt aEntryID ) const;
	TBool BusyStatus( TInt aMsgID ) const;
	IMPORT_C TBool MoreData( TInt aEntryID ) const;
	IMPORT_C TInt EntryID( TInt aCmdID, TInt aMsgID) const;

	private:
	void ConstructL();
	CNSmlResponseController();
	CNSmlResponseController( const CNSmlResponseController& aOther );
	CNSmlResponseController& operator=( const CNSmlResponseController& aOther );
	void CreateResponseChalL( TInt aIndex );
	void FreeResources( TInt aI );
		
	private: //Data
	//
	struct TChal
		{
		HBufC8* chalType;
		HBufC8* chalFormat;
		HBufC8* chalNextNonce;
		};
	
	struct TResponseData
		{
		TInt responseMsgID;
		HBufC8* responseCmd;
		TInt responseCmdID;
		HBufC8* responseLUID; 
		TInt responseAppIndex;
		TInt responseStatusCode;
		TBool responseStatusReceived;
		TChal* responseChal;
		TResponseDetail responseDetail;
		TBool responseMoreData;
		};
	CArrayFixFlat<TResponseData>* iResponse;
	TInt iEntryID;
	// Reserved to maintain binary compability
	CNSmlFutureReservation* iReserved;
	};

#endif // __NSMLRESPONSECONTROLLER_H
