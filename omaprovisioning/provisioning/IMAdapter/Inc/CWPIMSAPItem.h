/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Settings item for wireless village settings, handles also the saving of
*     Settings item. 
*
*/


#ifndef CWPIMSAPITEM_H
#define CWPIMSAPITEM_H

// INCLUDES
#include <e32base.h>
#include <impssapsettings.h>
#include "ProvisioningDebug.h"

// FORWARD DECLARATIONS
class CCommsDatabase;
class CIMPSSAPSettings;
class CIMPSSAPSettingsStore;

// CLASS DECLARATION

/**
 * CWPIMSAPItem handles saving of wireless village settings item
 *
 * @lib WPIMAdapter
 * @since 2.0
 */ 
class CWPIMSAPItem : public CBase
    {
    public: // construction / destruction
        
        /**
        * Two-phased constructor.
        */
        static CWPIMSAPItem* NewLC( TIMPSAccessGroup aAccessGroup );

        /**
        * Destructor.
        */
        virtual ~CWPIMSAPItem();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CWPIMSAPItem( TIMPSAccessGroup aAccessGroup );
        /**
        * 
        * @param aText
        * @return None
        */
        static void IncrementNameL(TDes& aText);
        
        
        /**
        * 
        * @param aText
        * @param aMaxLength
        * @return None
        */
        static void IncrementNameL(TDes& aText, TInt aMaxLength);
        
    public: // new methods:

        /**
        * Sets the settings name to save item.
        * @param aName Settings name
        */
        void SetSettingsNameL(const TDesC& aName);

        /**
        * Sets the User id to save item.
        * @param aUserId User Id
        */
        void SetUserIDL(const TDesC& aUserId);

        /**
        * Sets the password to save item.
        * @param aPassword Password
        */
        void SetPasswordL(const TDesC& aPassword);

        /**
        * Sets the URI of SAP to save item.
        * @param aURI URI of SAP
        */
        void SetSAPURIL(const TDesC& aURI);

        /**
        * Sets the id of IAP to save item.
        * @param aUid the uid of IAP.
        */
        void SetIAPIdL(TUint32 aUid);

        /**
        * Saves the service access point to SAPSettings db.
        * @return Id of saved service access point.
        */
        TUint32 StoreL();

		/**
		* Deletes the service access point from SAPSettings db with given id.
		* @param aUid Id of the service access point to be deleted.
		*/
		void DeleteL( TUint32 aUid );

        /**
        * Checks does a server with given name already exists. 
        * @param aServerName The name of the server. 
        * @return TBool Was there already server with same name. 
        */
        TBool IsServerNameUniqueL( const TDesC& aServerName );
        
        /**
        * Sets the Service access point with given id as default.
        * @param aUid The id of service access point
        */
        static void SetAsDefaultL(TUint32 aUid, TIMPSAccessGroup aAccessGroup );
        
    private: // data
                        
        /// Owns: Sap item containing data.
        CIMPSSAPSettings*       iWVSAPSettings;
        /// Owns: Sap settings store used for saving SAP.
        CIMPSSAPSettingsStore*  iSAPStore;
        TIMPSAccessGroup        iAccessGroup;

	private: // for testing purpose
        friend class T_CWPIMSAPItem;
    };

#endif // CWPIMSAPItem_H

// end of file