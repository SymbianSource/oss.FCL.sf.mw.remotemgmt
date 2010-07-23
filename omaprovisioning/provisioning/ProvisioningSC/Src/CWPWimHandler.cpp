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
#include	<ProvSC.h>
#include	"ProvisioningDebug.h"
#include	"MWPWimObserver.h"


// ============================ MEMBER FUNCTIONS ===============================


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
// CWPWimHandler::ReadProvFile();
// -----------------------------------------------------------------------------
//
void CWPWimHandler::ReadProvFile( const TOMAType& aFileType )
    {
    FLOG( _L( "[ProvisioningSC] CWPWimHandler::ReadProvFile" ) );
    
	iFileType = aFileType;
	if( !iClientInitialized )
		{
		iPhase = EInitialize;
		iFileLoader->Initialize( iStatus );
		SetActive();
		}
    }

// -----------------------------------------------------------------------------
// CWPWimHandler::DocL();
// -----------------------------------------------------------------------------
//
HBufC8* CWPWimHandler::DocL()
	{
	if( iProvisioningDoc )
		{
		return iProvisioningDoc->AllocL();
		}
	else
		{
		return NULL;
		}
	}

// -----------------------------------------------------------------------------
// CWPWimHandler::CWPWimHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPWimHandler::CWPWimHandler( MWPWimObserver& aObserver )
    : CActive( EPriorityIdle ), iObserver( aObserver ),
	 iProvData( NULL, 0 ), iClientInitialized( EFalse ), iFileType( EBootStrap)
    {
    }

// -----------------------------------------------------------------------------
// CWPWimHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPWimHandler::ConstructL()
    {
    FLOG( _L( "[ProvisioningSC] CWPWimHandler::ConstructL" ) );
    
	CActiveScheduler::Add( this );
	iFileLoader = CWimOMAProv::NewL();
    }

// -----------------------------------------------------------------------------
// CWPWimHandler::RunL
// -----------------------------------------------------------------------------
//
void CWPWimHandler::RunL()
    {
    FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL" ) );
    
	switch( iPhase )
		{
		case EInitialize:
			{
			FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL EInitialize" ) );
			if( iStatus == KErrNone )
				{
				Retrieve(); // ready to read the actual document
				}
			else
				{
				FTRACE(RDebug::Print(_L("[ProvisioningSC] CWPWimHandler::RunL EInitialize err (%d)"), iStatus.Int()));
				iObserver.ReadErrorL();
				}
			break;
			}
		case EGetSize:
			{
            FTRACE(RDebug::Print(_L("[ProvisioningSC] CWPWimHandler::RunL EGetSize status:%d, size:%d"), iStatus.Int(), iFileSize));
			
			if (iFileSize > 0 && iStatus == KErrNone )
				{
				delete iProvisioningDoc;
				iProvisioningDoc = NULL;
				iProvisioningDoc = HBufC8::NewL( iFileSize );
				// get the provisioningdoc from sim card, use first accessible
				// file type (EBootStrap, EConfig1, EConfig2)
				iProvData.Set(iProvisioningDoc->Des()) ;
				FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL EGetSize retrieving file" ) );
				
				iFileLoader->Retrieve( iFileType, iProvData, iStatus );
				iPhase = ERetrieve;
				SetActive();
				}
			else // try to get another type
				{
				FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL EGetSize error" ) );
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
					FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL EGetSize file not found" ) );
					iFileLoader->GetSize( iFileSize, iFileType, iStatus  );	
					iPhase = EGetSize;
					SetActive();
					}
				else
					{
					FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL EGetSize no more files" ) );
					iObserver.ReadErrorL();
					}
				}
			break;
			}
		case ERetrieve:
			{
			FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL ERetrieve" ) );
			if( iStatus==KErrNone )
				{
				FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL ERetrieve ok" ) );
                #ifdef _DEBUG
				FHex(iProvisioningDoc->Des());
                #endif
				FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL ERetrieve finished" ) );
				
				iObserver.ReadCompletedL();
				}
			else 
				{
	            FTRACE(RDebug::Print(_L("[ProvisioningSC] CWPWimHandler::RunL ERetrieve err:%d"), iStatus.Int()));
				iObserver.ReadErrorL();
				}
			break;
			}
		default:
			{
			FLOG( _L( "[ProvisioningSC] CWPWimHandler::RunL default" ) );
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
    FLOG( _L( "[ProvisioningSC] CWPWimHandler::DoCancel" ) );
    
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
// CWPWimHandler::Retrieve
// -----------------------------------------------------------------------------
//
void CWPWimHandler::Retrieve()
	{
	FLOG( _L( "[ProvisioningSC] CWPWimHandler::Retrieve" ) );

	if( iProvisioningDoc )
		{
		delete iProvisioningDoc;
		iProvisioningDoc = NULL;
		}
	iFileLoader->GetSize( iFileSize, iFileType, iStatus  );	
	iPhase = EGetSize;
	SetActive();
	}

//  End of File
