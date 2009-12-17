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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/


#ifndef MANAGEMENT_CONTEXT_HEADER_
#define MANAGEMENT_CONTEXT_HEADER_


// INCLUDES
#include <e32base.h>
#include <PolicyEngineClientServer.h>


// CLASS DECLARATION
class RElementIdArray : public RArray<HBufC8*>
{
	public:
		IMPORT_C void Close();	
	private:
		void SetListL( const TDesC8& aChilds);

		friend class RPolicyManagement;
};


class TElementInfo
{	
	public:		
		IMPORT_C TElementInfo( const TDesC8& aElementId );
		IMPORT_C ~TElementInfo();
		
		IMPORT_C const TDesC8& GetElementId() const;
		
		IMPORT_C const TDesC8& GetDescription() const;

		IMPORT_C const TDesC8& GetXACML() const;
		
		IMPORT_C const RElementIdArray& GetChildElementArray() const;
	
	private:
		TElementId iElementId;
		RElementIdArray iChildElements;
		HBufC8 * iDescription;
		HBufC8 * iXACMLContent;
		
		friend class RPolicyManagement;
};

class TParserResponse
{
	public:
		typedef TBuf8<KMaxReturnMessageLength> TReturnMessage;

	public: 
		IMPORT_C TParserResponse();
		IMPORT_C TParserResponse( TDes8& aReturnMessage);
	
		IMPORT_C void Zero();

		IMPORT_C const TDesC8& GetReturnMessage();

	private:
		TReturnMessage iReturnMessage;
};

#endif