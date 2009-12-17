/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef SCPTIMER_H
#define SCPTIMER_H

//  INCLUDES
#include <bldvariant.hrh>
#include <e32base.h>

/**
*  An interface class that specifies the type of the callback function
*/
class MSCPTimeoutHandler
    {	    
	public:
	
	/**
    * This method is called when the timer expires
    * @param aParam The parameter given to the timer when initialized.
    */  
	virtual void Timeout( TAny* aParam ) = 0;
	};


/**
*  A base class for the active objects responsible for the lock phone functionality
*/
class CSCPTimer : public CActive
	{		
	public:  // Methods

        // Constructors and destructor                
        
        /**
        * Static constructor.
        */
   	    static CSCPTimer* NewL( TUint aTimeout, TAny* aParam,
   	        MSCPTimeoutHandler* aHandler );
        
        /**
        * Static constructor, that leaves the pointer to the cleanup stack.
        */
        static CSCPTimer* NewLC( TUint aTimeout, TAny* aParam, 
            MSCPTimeoutHandler* aHandler );         

        /**
        * Destructor.
        */
        virtual ~CSCPTimer();

                
    protected:  // Methods
                        
        // Methods from base classes       
        
        /**
        * From CActive The AO worker method. Called when the timer expires.
        */
        void RunL();   
        
        /**
        * From CActive The AO cancellation method.
        */
        void DoCancel();
        
        
    private: //Methods 
        
        // New Methods   
                           
        /**
        * C++ default constructor.
        */
        CSCPTimer( TAny* aParam, MSCPTimeoutHandler* aHandler );
        
        /**
        * Symbian second-phase constructor
        */
        void ConstructL( TUint aTimeout );           
          
    private:   // Data 
        /** The timer handle */
        RTimer iTimer; 
        /** The parameter given when instanciated (not owned) */                                      
        TAny*   iParam;        
        /** A pointer to the timeout handler (not owned) */
        MSCPTimeoutHandler* iHandler;        
    };

#endif      // SCPTIMER_H   
            
// End of File

