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
* SyncInitiationNotifier.h
*/


#ifndef SYNCINITIATIONNOTIFIER_H_
#define SYNCINITIATIONNOTIFIER_H_


// FORWARD DECLARATIONS
class CRepository;
// CLASS DECLARATION

/**
* CSyncInitiationNotifier Central repository notifier.
*/
class CSyncInitiationNotifier : public CActive
    {
	public: //Constructors and destructors

		static CSyncInitiationNotifier* NewL();
 
		static CSyncInitiationNotifier* NewLC();

		~CSyncInitiationNotifier();

	protected:	//Constructors and destructors

		/**
		* C++ constructor.
		*/
		CSyncInitiationNotifier();
	
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

#endif /* SYNCINITIATIONNOTIFIER_H_ */
