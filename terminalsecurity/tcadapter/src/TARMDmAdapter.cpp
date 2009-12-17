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
* Description:  $Description: $
*
*/



// INCLUDE FILES
#include "TARMDmAdapter.h"
#include <PolicyEngineClient.h>
#include <SettingEnforcementInfo.h>
#include <DMCert.h>
#include <PolicyEngineXACML.h>
#include "debug.h"
#include <smldmadapter.h>


// ============================ MEMBER FUNCTIONS ===============================


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CTARMDmAdapter
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
CTARMDmAdapter::CTARMDmAdapter( MSmlDmCallback* aCallback )
	: CSmlDmAdapter( aCallback )
	, iInitialized( EFalse )
	{
	RDEBUG( "CTARMDmAdapter::CTARMDmAdapter" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
/*
void CTARMDmAdapter::ConstructL()
	{
    User::LeaveIfError( iPE.Connect() );
    User::LeaveIfError( iPR.Open( iPE ) );
	}
*/

void CTARMDmAdapter::InitializeL()
	{
	RDEBUG( "CTARMDmAdapter::InitializeL >" );
	if ( !iInitialized )
		{
	    User::LeaveIfError( iPE.Connect() );
	    User::LeaveIfError( iPR.Open( iPE ) );
	    iInitialized = ETrue;
		}
    RDEBUG( "CTARMDmAdapter::InitializeL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
CTARMDmAdapter::~CTARMDmAdapter()
	{
	RDEBUG( "CTARMDmAdapter::~CTARMDmAdapter >" );
	if ( iInitialized )
		{
		iPR.Close();
		iPE.Close();
		}
	RDEBUG( "CTARMDmAdapter::~CTARMDmAdapter <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::UpdateLeafObjectL >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_UpdateLeafObjectL( aURI, aLUID, aObject, aType, aStatusRef );
	RDEBUG( "CTARMDmAdapter::UpdateLeafObjectL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::UpdateLeafObjectL >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_UpdateLeafObjectL( aURI, aLUID, aStream, aType, aStatusRef );
	RDEBUG( "CTARMDmAdapter::UpdateLeafObjectL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::DeleteObjectL >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_DeleteObjectL( aURI, aLUID, aStatusRef );
	RDEBUG( "CTARMDmAdapter::DeleteObjectL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::FetchLeafObjectL >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_FetchLeafObjectL( aURI, aLUID, aType, aResultsRef, aStatusRef );
	RDEBUG( "CTARMDmAdapter::FetchLeafObjectL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::FetchLeafObjectSizeL >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_FetchLeafObjectSizeL( aURI, aLUID, aType, aResultsRef, aStatusRef );
	RDEBUG( "CTARMDmAdapter::FetchLeafObjectSizeL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID, const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, TInt aResultsRef, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::ChildURIListL >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_ChildURIListL( aURI, aLUID, aPreviousURISegmentList, aResultsRef, aStatusRef );
	RDEBUG( "CTARMDmAdapter::ChildURIListL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::AddNodeObjectL >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_AddNodeObjectL( aURI, aParentLUID, aStatusRef );
	RDEBUG( "CTARMDmAdapter::AddNodeObjectL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::ExecuteCommandL >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_ExecuteCommandL( aURI, aLUID, aArgument, aType, aStatusRef );
	RDEBUG( "CTARMDmAdapter::ExecuteCommandL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::ExecuteCommandL (stream) >" );
	if (CheckPolicyL( aURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_ExecuteCommandL( aURI, aLUID, aStream, aType, aStatusRef );
	RDEBUG( "CTARMDmAdapter::ExecuteCommandL (stream) <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
void CTARMDmAdapter::CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, const TDesC8& aSourceURI, const TDesC8& aSourceLUID, const TDesC8& aType, TInt aStatusRef )
	{
	RDEBUG( "CTARMDmAdapter::CopyCommandL >" );
	if (CheckPolicyL( aTargetURI ) != KErrNone)
		{
		Callback().SetStatusL( aStatusRef, EError );
		return;
		}
	_CopyCommandL( aTargetURI, aTargetLUID, aSourceURI, aSourceLUID, aType, aStatusRef );
	RDEBUG( "CTARMDmAdapter::CopyCommandL <" );
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
TInt CTARMDmAdapter::CheckPolicyL( const TDesC8& aURI )
	{
	RDEBUG( "CTARMDmAdapter::CheckPolicyL >" );
	InitializeL();
	TPtrC8 resource = PolicyRequestResourceL( aURI );

//sf	TPolicyRequestParams params = iAdapter->CreatePolicyRequestParamsL( aURI );
	TInt ret = CheckPolicy2L( resource );
	RDEBUG_2( "CTARMDmAdapter::CheckPolicyL < %d", ret );
	return ret;
	}

// -----------------------------------------------------------------------------
// CTARMDmAdapter::
// 
// -----------------------------------------------------------------------------
//
TInt CTARMDmAdapter::CheckPolicy2L( const TDesC8& aResource )
	{
	RDEBUG( "CTARMDmAdapter::CheckPolicy2L >" );
	TInt ret = KErrNone;
	
    TRequestContext context;
    TResponse response;

	// Get Certificate
    RDMCert dmcert;
    TCertInfo ci;
    ret = dmcert.Get( ci );

    // If got certificate then put is as subject to policy request
	if (ret == KErrNone)
		{
	    context.AddSubjectAttributeL( PolicyEngineXACML::KTrustedSubject, ci );
		}
	else if (ret == KErrNotFound)
		{
		ret = KErrNone;
		}
//sf-    User::LeaveIfError( ret );

	// Check Policy
	if (ret == KErrNone)
		{
	    context.AddResourceAttributeL( 
	        PolicyEngineXACML::KResourceId, 
	        aResource,
	        PolicyEngineXACML::KStringDataType );
		ret = iPR.MakeRequest( context, response );
		}
//sf-    User::LeaveIfError( ret );

	// Check Policy request result
	if (ret == KErrNone)
		{
    	TResponseValue resp = response.GetResponseValue();
	    switch( resp )
	        {
		    case EResponsePermit:
	        	ret = KErrNone;
		        break;
	        case EResponseDeny:
		    case EResponseNotApplicable:
		    case EResponseIndeterminate:
		    default:
	    	    ret = KErrAccessDenied;
	    	    break;
			}
		}

    RDEBUG_2( "CTARMDmAdapter::CheckPolicy2L < %d", ret );
	return ret;
	}

// -----------------------------------------------------------------------------
// xxx::
// 
// -----------------------------------------------------------------------------
//

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
//?type  ?function_name(
//    ?arg_type arg,  // ?description
//    ?arg_type arg )  // ?description
//    {
//
//    ?code
//
//    }

// End of File
