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
* Description:  Client's Status command buffering
*
*/




#ifndef __NSMLSTATUSCONTAINER_H
#define __NSMLSTATUSCONTAINER_H

// INCLUDES
#include <e32base.h>
#include "smldtd.h"

// FORWARD DECLARATIONS
class CNSmlFutureReservation;

// CLASS DECLARATION
class CNSmlStatusContainer : public CBase 
	{
	//  
	public:  // constructors and destructor 
	static CNSmlStatusContainer* NewL( TBool aClearText = EFalse); 
	static CNSmlStatusContainer* NewLC( TBool aClearText = EFalse);
	 ~CNSmlStatusContainer();    //from CBase
	
	public: // new functions 
	// Initiate MsgRef element
	void InitMsgRefL( const SmlPcdata_t* aMsgRef );
	// Create New Status element, returns entry ID to created element
	IMPORT_C TInt CreateNewStatusElementL();
	// Set values to Status element
	void SetCmdIDL( TInt aEntryID, const SmlPcdata_t* aCmdID );
	void SetMsgRefL( TInt aEntryID, const SmlPcdata_t* aMsgRef );
	IMPORT_C void SetCmdRefL( TInt aEntryID, const SmlPcdata_t* aCmdRef );
	IMPORT_C void SetCmdL( TInt aEntryID, const TDesC8& aCmd );
	IMPORT_C void SetStatusCodeL( TInt aEntryID, TInt aStatusCode, TBool aFixStatus = EFalse );
	void SetChalL( TInt aEntryID, const TDesC8& aNonce );  
	IMPORT_C void SetNoResponse( TInt aEntryID , TBool aNoResponce );
	
	IMPORT_C void SetAtomicOrSequenceId( TInt aEntryID, TInt aAtomicOrSequenceID );
    IMPORT_C void SetPerformedInAtomic( TInt aEntryID );
	IMPORT_C void SetStatusCodeToAtomicOrSequenceCmdL( TInt aAtomicOrSequenceID, TInt aStatusCode, const TDesC8& aCmd );
	IMPORT_C void SetStatusCodesInAtomicL( TInt aAtomicID, TInt aStatusCode, TBool aOnlyForPerformed );
	// Add values to Status element
	IMPORT_C void AddTargetRefL( TInt aEntryID, const SmlTarget_t* aTarget ); 
	IMPORT_C void AddSourceRefL( TInt aEntryID, const SmlSource_t* aSource ); 
	IMPORT_C void AddItemDataL (TInt aEntryID, const SmlPcdata_t* aData );
	void RemoveWritten (TInt aEntryID );
	// Get Status Elements
	IMPORT_C void Begin();
	IMPORT_C TBool NextStatusElement( SmlStatus_t*& aStatus, TBool aOnlyIfResponse = ETrue );
	// Get ID of the current element
	TInt CurrentEntryID() const;	
	// Get ID of the last element
	IMPORT_C TInt LastEntryID() const;	
	//  
	TBool AnyOtherThanOkSyncHdrStatus() const;
	
	private:
	void ConstructL( TBool aClearText);
	CNSmlStatusContainer();
	CNSmlStatusContainer( const CNSmlStatusContainer& aOther );
	CNSmlStatusContainer& operator=( const CNSmlStatusContainer& aOther );
	void CreatePcdataL ( SmlPcdata_t*& aPcdata, const TDesC8& aContent ) const;
	void CopyPcdataL( const SmlPcdata_t* aFromPcdata, SmlPcdata_t*& aToPcdata ) const;	
	void CopyMetInfL ( const void* aFromMetInf, void*& aToMetInf ) const; 
	void CreateTargetRefL( const SmlTarget_t* aTarget, SmlPcdata_t*& aTargetRef ) const;
	void CreateSourceRefL( const SmlSource_t* aSource, SmlPcdata_t*& aSourceRef ) const;
	void RemoveAllWrittenOnes();
	void FreeResources( TInt aI );
	TBool StatusCodeVal ( SmlPcdata_t*& aStatusCode, TInt& aVal ) const;
	
	private:  //Data
	struct TStatusData
		{
		SmlStatus_t* status;
		TBool noResponse;
		TBool wasWritten;
		TInt atomicOrSequenceID;
		TBool performedInAtomic;
		TBool statusIsFixed;
		};
	TInt iArrayInd;
	SmlPcdata_t* iMsgRef;
	CArrayFixFlat<TStatusData>* iStatusArray;
	TBool iClearText; 
	// Reserved to maintain binary compability
	CNSmlFutureReservation* iReserved;
	};

#endif // __NSMLSTATUSCONTAINER_H
