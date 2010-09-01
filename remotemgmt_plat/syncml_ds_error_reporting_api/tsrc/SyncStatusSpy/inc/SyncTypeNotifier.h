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
* SyncTypeNotifier.h
*/


#ifndef SYNCTYPENOTIFIER_H_
#define SYNCTYPENOTIFIER_H_



// FORWARD DECLARATIONS
class CRepository;
// CLASS DECLARATION

/**
* CSyncTypeNotifier Central repository notifier.
*/
class CSyncTypeNotifier : public CActive
    {
	public: //Constructors and destructors

		static CSyncTypeNotifier* NewL();
 
		static CSyncTypeNotifier* NewLC();

		~CSyncTypeNotifier();

	protected:	//Constructors and destructors

		/**
		* C++ constructor.
		*/
		CSyncTypeNotifier();
	
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

#endif /* SYNCTYPENOTIFIER_H_ */
