/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef ERRORCODENOTIFIER_H_
#define ERRORCODENOTIFIER_H_

// INCLUDES


// FORWARD DECLARATIONS
class CRepository;
// CLASS DECLARATION

/**
* CErrorCodeNotifier Central repository notifier.
*/
class CErrorCodeNotifier : public CActive
    {
	public: //Constructors and destructors

		static CErrorCodeNotifier* NewL();
 
		static CErrorCodeNotifier* NewLC();

		~CErrorCodeNotifier();

	protected:	//Constructors and destructors

		/**
		* C++ constructor.
		*/
		CErrorCodeNotifier();
	
		/**
		* 2nd phase constructor.
		*/
		void ConstructL();


    public:

	
    private:
    	 void ListenForKeyNotifications();
    	
    	 void DoCancel();
    	        
    	 void RunL();
    	 
    private:
    	RFs iFs;
    	CRepository* iRep;
    };

#endif /* ERRORCODENOTIFIER_H_ */
