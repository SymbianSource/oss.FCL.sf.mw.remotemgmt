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
* Description: Implementation of policymanagement components
*
*
*/


#ifndef POLICTY_ENGINE_UI_HEADER_
#define POLICTY_ENGINE_UI_HEADER_

//  INCLUDES
#include <e32base.h>
#include <eiknotapi.h>

//  CONSTANTS

_LIT8( KUserAcceptMark, "A");
_LIT8( KUserDenyMark, "D");
const TUint KDelimeterChar = '|';

enum TUserResponse
	{
	EUserAccept,
	EUserDeny,
	};

//  FORWARD DECLARATIONS

class CCoeEnv;
class CPolicyEngineUi;


class CPolicyEngineNotifier : public CBase, public MEikSrvNotifierBase2
	{
	public:
	
		CPolicyEngineNotifier();
		~CPolicyEngineNotifier();
		static CPolicyEngineNotifier* NewL();
		void ConstructL();
		
		void Complete( TUserResponse aResponse);
		
    public: //From MEikSrvNotifierBase2
    
		void Release();
		TNotifierInfo RegisterL();
		TNotifierInfo Info() const;
		TPtrC8 StartL(const TDesC8& aBuffer);
		void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
		void Cancel();
		TPtrC8 UpdateL(const TDesC8& aBuffer); 
		  
	private:
		CPolicyEngineUi * iPolicyEngineUi;
		
        /**
        * refers connections is intialized or not
        */
        TNotifierInfo         iInfo;             ///< Notifier info
        
        TInt iReplySlot;
        RMessagePtr2 iMessage;
	};



/**
* This class provides a wrapper for some common dialogs for the policy engine
*
* @lib policyengineui.lib
*/
class CPolicyEngineUi : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        */
        CPolicyEngineUi();

        /**
        * Two-phased constructor.
        */
        static CPolicyEngineUi* NewL();
    
        /**
        * Destructor.
        */
        virtual ~CPolicyEngineUi();
        
        void ActivateL( const TDesC8& aCorporate, const TDesC8& aRandomPart, CPolicyEngineNotifier* iNotifier); 
    
		void RunL();
		TInt RunError(TInt aError);
		void DoCancel();

	public:

        /**
        * Execute corporate policy question chain, which user deny or accept. One response generated.
        * @param aCorporate - Corparate name used in queries
        * @param aRandomPart - Random part of certificate, which user must know.
        * @return TUserResponse - Response value for user
        */
        
        TUserResponse ShowCorporateConfirmationDialogL( const TDesC8& aCorporate,
        										   		 const TDesC8& aRandomPart);
	private:
	
		//private enums
		enum TDialog
			{
			EControl = 0, 
			EQuestion,
			ERemove,
			EDenied,
			EUnMatch
			};
		
		enum TDlgResp
			{
			EOkResp,
			ECancelResp
			};
		
	private:
	
        /**
        * Two-phased constructor.
        */
		void ConstructL();

	
		TInt ShowDialogL( const TDialog& aDialog);
		TDlgResp ShowConfirmationQueryL( const TDesC& aText, const TBool& aWithCancel );
		TDlgResp ShowPossessionMessageQueryL();
		TDlgResp DataQueryL( const TDesC& aText, TDes& aInput );

    private: // Data
    
    	HBufC * iCorporate;
    	HBufC * iRandomPart;
    	TInt iState;
    	CPolicyEngineNotifier * iNotifier;
    	
	    TInt iResourceFileOffset;
        CCoeEnv* iCoeEnv;  // not owned
        };


#endif      // POLICTY_ENGINE_UI_HEADER_
            
// End of File
