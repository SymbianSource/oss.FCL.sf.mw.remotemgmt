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
* Description:  header file for lawmo Client
*
*/

#ifndef __LAWMOCLIENT_H__
#define __LAWMOCLIENT_H__


#include <e32base.h>
#include <f32file.h>
#include <dcmoconst.h>

// needed for creating server thread.

class RLawmoClient : public RSessionBase
    {
public:

	/**
	 * connects to DCMOServer
	 * @param None
	 * @return KErrNone Symbian error code
	 */	
	IMPORT_C TInt Connect();  
	/**
	 * Close server connection
	 * @param None
	 * @return None
	 */
	IMPORT_C void Close();
	/**
	* Wipe the contents of the Device
	* @param None
	* @return None
	*/
	IMPORT_C TLawMoStatus DeviceWipe();

	/**
	* Wipe All the contents in the Device
	* @param None
	* @return None
	*/
	IMPORT_C TLawMoStatus DeviceWipeAll( );

	/**
	/*Fetches the ListItemName of the lawmo node
	* @param ItemName - Node on which this fetch is initiated
	* @param NodeValue- List Item name value
	* @return Symbian error code
	*/
	IMPORT_C TInt GetListItemName( TDes& aItemName, TDes& aNodeValue);

	/**
	/*Fetches the tobewiped of the lawmo node
	* @param ItemName - Node on which this fetch is initiated
	* @param NodeValue- List Item name value
	* @return Symbian error code
	*/
	IMPORT_C TInt GetToBeWipedValue( TDes& aItemName, TBool& aWipeVal);

	/**
	/*Sets the wipe value for the lawmo node	
	* @param ItemName - Node on which this operation is initiated
	* @param NodeValue- wipe value
	* @return Symbian error code
	*/	
	IMPORT_C TInt SetToBeWipedValue( TDes& aItemName, TInt aWipeVal );

private:

    };

#endif // __LAWMOCLIENT_H__
