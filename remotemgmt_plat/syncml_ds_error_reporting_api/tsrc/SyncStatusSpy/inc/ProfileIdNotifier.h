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
* ProfileIdNotifier.h
*/


#ifndef PROFILEIDNOTIFIER_H_
#define PROFILEIDNOTIFIER_H_



// FORWARD DECLARATIONS
class CRepository;
// CLASS DECLARATION

/**
* CProfileIdNotifier Central repository notifier.
*/
class CProfileIdNotifier : public CActive
    {
	public: //Constructors and destructors

		static CProfileIdNotifier* NewL();
 
		static CProfileIdNotifier* NewLC();

		~CProfileIdNotifier();

	protected:	//Constructors and destructors

		/**
		* C++ constructor.
		*/
		CProfileIdNotifier();
	
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

#endif /* PROFILEIDNOTIFIER_H_ */
