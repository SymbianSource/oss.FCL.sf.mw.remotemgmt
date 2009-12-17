/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of syncservice constants/exported methods
* 	This is part of remotemgmt_plat.
*/


#ifndef SYNCSERVICESESSION_H
#define SYNCSERVICESESSION_H

#include <eikdoc.h>
#include <AknServerApp.h>

enum TSyncServiceCmds
	{
	ESyncServiceCommand = RApaAppServiceBase::KServiceCmdBase
	};

/**
*  CSyncServiceSession  
*
*  @lib syncservice.lib
*  @since Series 60 3.0
*/
class CSyncServiceSession : public CAknAppServiceBase
	{
	public:
	
        /**
        * C++ default constructor.
        */
		IMPORT_C CSyncServiceSession( );
		
    	/**
		* C++ Destructor
		*/
		virtual ~CSyncServiceSession();

	protected: // from CSession2
	
		/**
		* Just basecalls CAknAppServiceBase
		*/
		void CreateL();
		
		/**
		* Checks the function from aMessage. If it is
		* ESyncServiceStart -> calls
		* OpenEmbedded. Otherwise basecalls CAknAppServiceBase.
		* @param aMessage 
		* @return None
		*/
		void ServiceL( const RMessage2& aMessage );
		
		/**
		* Basecalls CAknAppServiceBase.
		* @param aMessage 
		* @param aError
		* @return None
		*/
		void ServiceError( const RMessage2& aMessage, TInt aError );

	private:
		
		/**
		* Constructs TSyncParameters from aMessage
		* Externalizes the params to store and restore
		* CEikDocument from there (ie. starts DS DM sync observing).
		* Completes aMessage.
		* @param aMessage 
		* @return None
		*/
		void OpenEmbeddedL( const RMessage2& aMessage );

	private:
		CEikDocument* iDoc;
	};

#endif //SYNCSERVICESESSION_H
