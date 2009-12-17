/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef __TARMDmAdapter_H__
#define __TARMDmAdapter_H__

//  INCLUDES
#include <e32base.h>
#include <smldmadapter.h>

#include <PolicyEngineClient.h>



// Adapter to check policy and if allowed to call "original" adapter
class CTARMDmAdapter : public CSmlDmAdapter
	{
public:

	// Implementation of MSmlDmAdapter interface functions that make policy checks
	// ===========================================================================
private:
	void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef );
	void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );
	void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef );
	void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
	void FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef );
	void ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, TInt aResultsRef, TInt aStatusRef );

	void AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef );
	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef );
	void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef );
	void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, const TDesC8& aSourceURI, const TDesC8& aSourceLUID, const TDesC8& aType, TInt aStatusRef );

	// Implementation of MSmlDmAdapter interface functions AFTER policy checks
	// =======================================================================
public:
	virtual void _UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef ) = 0;
	virtual void _UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef ) = 0;
	virtual void _DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef ) = 0;
	virtual void _FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef ) = 0;
	virtual void _FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef ) = 0;
	virtual void _ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, TInt aResultsRef, TInt aStatusRef ) = 0;

	virtual void _AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef ) = 0;
	virtual void _ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef ) = 0;
	virtual void _ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef ) = 0;
	virtual void _CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, const TDesC8& aSourceURI, const TDesC8& aSourceLUID, const TDesC8& aType, TInt aStatusRef ) = 0;

	// Other

	// PolicyCheck
	virtual TPtrC8 PolicyRequestResourceL( const TDesC8& aURI ) = 0;
	virtual TInt CheckPolicyL( const TDesC8& aURI );
	virtual TInt CheckPolicy2L( const TDesC8& aResource );

	// Other

protected:
	CTARMDmAdapter( MSmlDmCallback* aCallback );

	virtual ~CTARMDmAdapter();

private:
	virtual void InitializeL();

protected:
	TBool iInitialized;
	
private:
    RPolicyEngine   iPE;
    RPolicyRequest  iPR;
	};

#endif      // __TARMDmAdapter_H__

// End of File
