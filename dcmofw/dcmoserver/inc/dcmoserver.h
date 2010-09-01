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
* Description:  DCMO Server
*
*/

#ifndef __DCMO_SERVER_H__
#define __DCMO_SERVER_H__

// INCLUDES

#include <e32base.h>
#include "dcmoclientserver.h"
#include "dcmoconst.h"
#include "dcmonotifieraob.h"
#include <lawmocallbackinterface.h>
// CONSTANTS
const TUid KCRUidDCMOServer={0x2001FE47};
const TInt KDCMOKeyMaxNumber = 16;
// FORWARD DECLARATIONS
// CLASS DECLARATION
class CLAWMOPluginInterface;
/**
*  CDCMOServer
*  Description.
*/


class CDCMOServer : public CServer2,
                    public MLawmoPluginWipeObserver
	{
	friend class CDCMOSession;
	friend class CDCMONotifierAob;

public:
	/**
	 * Structure to hold the plug-in details
	 */ 
	struct dcmoInfoList
	{
		HBufC* iCategoryName;
		TInt iUid;
		TBool iStatus;
	};
	/**
	 * Destructor
	 */
	~CDCMOServer();	
	
	/**
	 * Static method which creates the server instance
	 * @param None
	 * @return CServer2*
	 */ 
	static CServer2*    NewLC( );
	
	/**
	 * Locks the session, so that new session will not be started
	 * before the previous request completion
	 * @param None
	 * @return None
	 */
  void LockSession();
  
  /**
	 * UnLocks the session, so that new session can start later.
	 * @param None
	 * @return None
	 */
  void UnLockSession();  
    
	/**
	 * Stops the session and closes the server
	 * @param None
	 * @return None
	 */
	void DropSession();
public:
	
	// Callback from MLawmoPluginWipeObserver
	void HandleWipeCompleted(TInt status);
	//void SendGenericAlert();
	
private:
	/**
	 * Default constructor
	 */
	CDCMOServer();
	
	/**
	 * second phase constructor
	 */
	void ConstructL();
	
	/**
	 * Calls when a New session is getting created
	 * @param aVersion
	 * @param aMessage
	 * @return CSession2*
	 */
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage)  const;
	

protected:
    //
    // Services
    //   
  /**
	 * Get the interger type value
	 * @param aCategory
	 * @param aId
	 * @param aValue
	 * @return TDCMOStatus 
	 */ 
	TDCMOStatus GetIntAttributeL(TDes& aCategory, TDCMONode aId, TInt& aValue);
	
	/**
	 * Get the string type value
	 * @param aCategory
	 * @param aId
	 * @param aStrValue
	 * @return TDCMOStatus
	 */
	TDCMOStatus GetStrAttributeL(TDes& aCategory, TDCMONode aId, TDes& aStrValue);
	
	/**
	 * Set the interger type value
	 * @param aCategory
	 * @param aId
	 * @param aValue
	 * @return TDCMOStatus
	 */
	TDCMOStatus SetIntAttributeL(TDes& aCategory, TDCMONode aId, TInt aValue);
	
	/**
	 * Set the string type value
	 * @param aCategory
	 * @param aId
	 * @param aStrValue
	 * @return TDCMOStatus
	 */
	TDCMOStatus SetStrAttributeL(TDes& aCategory, TDCMONode aId, const TDes& aStrValue);	
	
	/**
	 * Finalize the Service
	 * @return TDCMOStatus
	 */
	void DoFinalizeL();	
	
	/**
	 * Get the list of all plug-in adapters
	 * @param aGroup
	 * @param aAdapterList
	 * @return 
	 */
	void SearchAdaptersL(TDes& aGroup, TDes& aAdapterList);	
	
	/**
	 * Set the iStarter value
	 * @param aValue, ETrue if it is Starter else EFalse
	 * @return 
	 */
	void SetStarter(TBool aValue);	
	
    TLawMoStatus WipeItem(TInt aValue=0);
	
    TLawMoStatus WipeAllItem();

	TLawMoStatus GetListItemL(TDesC& item, TDes& strValue);

	TLawMoStatus GetToBeWipedL(TDesC& item, TDes& wipeValue);

	TLawMoStatus SetToBeWipedL(TDesC& item, TInt wipeValue);
	
private:
	
	/**
	 * Get the uid of plug-in adapter
	 * @param aCategory
	 * @return the uid
	 */
	TUid GetAdapterUidL(const TDesC& aCategory, TBool aIsLawmo = EFalse);
	
	/**
	 * Finds whether the plug-in adapter is a generic category
	 * @param aCategory
	 * @return the integer
	 */
		TInt GetLocalCategoryL(const TDesC& aCategory);	
	
	void GetLawmoPluginUidsL();
	
	void StartDMNetworkMonitorL(TDesC& aServerId, TInt iapid=0);
private:
	RArray<dcmoInfoList> idcmoArray;
	static TInt iSessionCount;
	TBool iStarter;	
	CDCMONotifierAob* iNotifier;
	RArray<TUid> ilawmoPluginUidToBeWiped;
	RArray<CLAWMOPluginInterface*> ilawmoPlugins;
	TInt iwipeStatus;
    TInt iCount;
    TInt iSessionIap;
	};
	
#endif //__DCMO_SERVER_H__
