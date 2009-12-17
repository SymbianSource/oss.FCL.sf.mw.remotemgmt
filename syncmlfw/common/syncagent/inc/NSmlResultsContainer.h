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
* Description:  Client's Results command buffering
*
*/



#ifndef __NSMLRESULTSCONTAINER_H
#define __NSMLRESULTSCONTAINER_H

// INCLUDES
#include <e32base.h>
#include "smldtd.h"


// FORWARD DECLARATIONS
class CNSmlCmdsBase;
class CNSmlFutureReservation;

// CLASS DECLARATION
class CNSmlResultsContainer : public CBase 
	{
	//  
	public:  // constructor and destructor
	static CNSmlResultsContainer* NewL( CNSmlCmdsBase* aSyncMLCmds = NULL ); 
	~CNSmlResultsContainer();    //from CBase
	public: // new functions
	// Creates new Results element  
	IMPORT_C TInt CreateNewResultsL( const TDesC8& aMsgRef, const TDesC8& aCmdRef, const SmlTarget_t* aTargetRef, const SmlSource_t* aSourceRef );
	// Set CmdID to Results element
	IMPORT_C void SetCmdIDL( TInt aEntryID, const SmlPcdata_t* aCmdID );
	// Add Item element
	IMPORT_C void AddItemL( TInt aEntryID, const CBufBase& aObject, const TDesC8& aType, const TDesC8& aFormat ); 
	// Set Id of Status ( in CNSmlStatusContainer instance )
	IMPORT_C void SetStatusID( TInt aEntryID, TInt aStatusEntryID );
	//
	IMPORT_C void RemoveWritten (TInt aEntryID );
		
	IMPORT_C void Begin();
	IMPORT_C TBool NextResultsElement( SmlResults_t*& aResults );
	// Get ID of the current element
	IMPORT_C TInt CurrentEntryID() const;	
	// Any results in container
	IMPORT_C TBool AnyResults() const;	
	// Get Id of Status ( in CNSmlStatusContainer instance )
	IMPORT_C TInt StatusID( TInt aEntryID ) const;
	//  
	// 
		
	private:
	void ConstructL( CNSmlCmdsBase* aSyncMLCmds );
	CNSmlResultsContainer();
	CNSmlResultsContainer( const CNSmlResultsContainer& aOther );
	CNSmlResultsContainer& operator=( const CNSmlResultsContainer& aOther );

	void CopyPcdataL( const SmlPcdata_t* aFromPcdata, SmlPcdata_t*& aToPcdata ) const;
	void RemoveAllWrittenOnes();
	void FreeResources( TInt aI );
	//  Private variables
	
	private: //Data
	struct TResultsData
		{
		HBufC8* iMsgRef;
		HBufC8* iCmdRef;
		SmlResults_t* iResults;
		TBool iWasWritten;
		TInt iStatusRef;
		};
	TInt iArrayInd;
	CArrayFixFlat<TResultsData>* iResultsArray;
	//used classes
	CNSmlCmdsBase* iSyncMLCmds;
	// Reserved to maintain binary compability
	CNSmlFutureReservation* iReserved;
	};

#endif // __NSMLRESULTSCONTAINER_H
