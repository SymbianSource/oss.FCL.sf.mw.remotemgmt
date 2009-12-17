/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used to transfer Alert messages and DM Auth info to SyncML Symbian OS Server
*
*/


#ifndef __NSMLPRIVATEAPI_H__
#define __NSMLPRIVATEAPI_H__

#include <e32base.h>
#include <SyncMLDef.h>
#include "nsmlconstants.h"		 
#include <s32mem.h>
class CNSmlDMAuthInfo;

struct CNSmlDMAlertItem { 
        HBufC8* iSource;
	    HBufC8* iTarget;
	    HBufC8* iMetaType;
	    HBufC8* iMetaFormat;
	    HBufC8* iMetaMark;
	    HBufC8* iData;
};


/**
*  RNSmlPrivateAPI class 
*  Used to transfer Alert messages and DM Auth info to SyncML Symbian OS Server
*
*  @lib NSmlPrivateApi.lib
*/
class RNSmlPrivateAPI : public RSessionBase
	{
	
	public:

		/**
		* C++ default constructor.
	    */
		IMPORT_C RNSmlPrivateAPI();
		
		/**
	    * Creates new session to SyncML SOS Server. Server is launched if not already running.
	    * @param -
	    * @return -
	    */
		IMPORT_C void OpenL();
		
		/**
	    * Transfer message to SyncML SOS Server.
	    * @param aData message to be transferred.
	    * @param aType Specifies the usage type (DS/DM).
	    * @param aVersion specifies the protocol of the message
	    * @return -
	    */
		IMPORT_C void SendL(const TDesC8& aData, TSmlUsageType aType, TSmlProtocolVersion aVersion, TUid aTransportId = KUidNSmlMediumTypeInternet );

		/**
	    * Sends DM authetication info to server for saving.
	    * @param aAuthInfo Reference to authentication data container.
	    * @return -
	    */
		IMPORT_C void SetDMAuthInfoL( const CNSmlDMAuthInfo& aAuthInfo );
		
		/**
	    * Retrieves the DM authentication data from server.
	    * @param aAuthInfo Reference to authentication data container.
	    * @return -
	    */
		IMPORT_C void GetDMAuthInfoL( CNSmlDMAuthInfo& aAuthInfo );
		
		// FOTA
		
		/**
	    * Adds a Generic Alert to DM Agent if it is found.
	    * @param aMgmtUri The uri which has been used in the exe command whose final result is 
	    * about to be reported.
	    * @param aMetaType Meta/Type that should be used in the alert.
	    * @param aMetaFormat Meta/Format that should be used in the alert.
	    * @param aFinalResult The final result value which is reported to remote server.
	    * @param aCorrelator Correlator value used in the original exec command.
	    * @return KErrNone, if successful. System wide error otherwise.
	    */
		IMPORT_C TInt AddDMGenericAlertRequestL( const TDesC8& aMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, TInt aFinalResult, const TDesC8& aCorrelator ) const;
		
		// FOTA end
		/**
	    * Adds a Generic Alert to DM Agent if it is found.
	    * @param aCorrelator Correlator value used in the original exec command.
	    * @param aItemList Array of CNSmlDMAlertItem.
		* @return KErrNone, if successful. System wide error otherwise.
	    */

	    IMPORT_C TInt AddDMGenericAlertRequestL( const TDesC8& aCorrelator, RArray<CNSmlDMAlertItem>& aItemList ) const;

		/**
	    * Closes the connection with SyncML SOS Server
	    * @param -
	    * @return -
	    */
		IMPORT_C void Close();

	};
	
#endif
