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
* Description:  
*
*/


// INCLUDE FILES
#include	"CWPWimHandler.h"
#include	<e32std.h>  
#include	"CWPEngine.pan"
// #include	"SimApiTest.h" 
#include	<ProvSC.h>
#include	"MWPWimObserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPWimHandler::CWPWimHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWimHandler::CWPWimHandler( MWPWimObserver& aObserver )
    : CActive( EPriorityIdle ), iObserver( aObserver ),
	iClientInitialized( EFalse ), iProvData( 0, 0 ), iFileType( EBootStrap)
    {
    }

// -----------------------------------------------------------------------------
// CWPWimHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWimHandler::ConstructL()
    {
	CActiveScheduler::Add( this );
//	iFileLoader = CSimApiTest::NewL();

	iFileLoader = CWimOMAProv::NewL();
    }

// -----------------------------------------------------------------------------
// CWPWimHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPWimHandler* CWPWimHandler::NewL( MWPWimObserver& aObserver )
    {
	CWPWimHandler* handler  = CWPWimHandler::NewLC( aObserver );
    CleanupStack::Pop();
    return handler;
    }

// -----------------------------------------------------------------------------
// CWPWimHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPWimHandler* CWPWimHandler::NewLC( MWPWimObserver& aObserver )
    {
    CWPWimHandler* self = new( ELeave ) CWPWimHandler( aObserver );    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CWPWimHandler::~CWPWimHandler
// Destructor
// -----------------------------------------------------------------------------
//
CWPWimHandler::~CWPWimHandler()
    {
    Cancel(); // CActive
	delete iFileLoader;
	delete iProvisioningDoc;
    }

// -----------------------------------------------------------------------------
// CWPWimHandler::RunL
// -----------------------------------------------------------------------------
//
void CWPWimHandler::RunL()
    {
	switch( iPhase )
		{
		case EInitialize:
			{
			if( iStatus == KErrNone )
				{
				Retrieve(); // ready to read the actual document
				}
			else
				{
				iObserver.ReadErrorL();
				}
			break;
			}
		case EGetSize:
			{
			if (iFileSize > 0 && iStatus == KErrNone )
				{
				iProvisioningDoc = HBufC8::NewL( iFileSize );
				// get the provisioningdoc from sim card, use first accessible
				// file type (EBootStrap, EConfig1, EConfig2)
				iProvData.Set(iProvisioningDoc->Des()) ;
				iFileLoader->Retrieve( iFileType, iProvData, iStatus );
				iPhase = ERetrieve;
				SetActive();
				}
			else // try to get another type
				{
				TBool tryToRead( ETrue );
				switch( iFileType )
					{
					case EBootStrap:
						{
						iFileType = EConfig1;
						}
					break;
					case EConfig1:
						{
						iFileType = EConfig2;
						}
					break;
					default:
						{
						tryToRead = EFalse;
						}
					break;
					}
				if( tryToRead )
					{
					iFileLoader->GetSize( iFileSize, iFileType, iStatus  );	
					iPhase = EGetSize;
					SetActive();
					}
				else
					{
					iObserver.ReadErrorL();
					}
				}
			break;
			}
		case ERetrieve:
			{
			if( iStatus==KErrNone )
				{
				iObserver.ReadCompletedL();
				}
			else 
				{
				iObserver.ReadErrorL();
				}
			break;
			}
		default:
			{
			break;
			}
		}
    }

TInt CWPWimHandler::RunError ( TInt /*aError*/ )
    {
        return KErrNone;
    }	
// -----------------------------------------------------------------------------
// CWPWimHandler::DoCancel
// Cancel active request
// -----------------------------------------------------------------------------
//
void CWPWimHandler::DoCancel()
    {
	TRAPD( ignoreError, iObserver.ReadCancelledL() );
	if (ignoreError) ignoreError = KErrNone; 
	switch( iPhase )
		{
		case EInitialize:
			{
			iFileLoader->CancelInitialize();
			break;
			}
		case EGetSize:
			{
			iFileLoader->CancelGetSize();
			break;
			}
		case ERetrieve:
			{
			iFileLoader->CancelRetrieve();
			break;
			}
		default:
			{
			break;
			}
		}
    }

// -----------------------------------------------------------------------------
// CWPWimHandler::ReadProvFile();
// -----------------------------------------------------------------------------
//
void CWPWimHandler::ReadProvFile()
    {
	if(!iClientInitialized)
		{
		iPhase = EInitialize;
		iFileLoader->Initialize( iStatus );
		SetActive();
		}
    }

void CWPWimHandler::Retrieve()
	{
	if( iProvisioningDoc )
		{
		delete iProvisioningDoc;
		iProvisioningDoc = NULL;
		}
	// 1. try to get the size of the first possible filetype
	iFileType = EBootStrap;
	iFileLoader->GetSize( iFileSize, iFileType, iStatus  );	
	iPhase = EGetSize;
	SetActive();
	// 2. other types are tried and read in RunL
	}


// -----------------------------------------------------------------------------
// CWPWimHandler::DocLength();
// -----------------------------------------------------------------------------
//
TInt CWPWimHandler::DocLength()
	{
	return iProvisioningDoc->Length(); 
	}

// -----------------------------------------------------------------------------
// CWPWimHandler::Doc();
// -----------------------------------------------------------------------------
//
HBufC8* CWPWimHandler::DocL()
	{
	return iProvisioningDoc->AllocL();
	}

//  End of File  
