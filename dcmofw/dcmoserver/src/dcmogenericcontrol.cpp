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
* Description:  DCMO GenericControl implementation
*
*/

#include <e32base.h>
#include <centralrepository.h>
#include "dcmogenericcontrol.h"
#include "dcmoserver.h"
#include "dcmoconst.h"
#include "dcmodebug.h"

const TInt KBufferSize = 30;
// ----------------------------------------------------------------------------------------
// CDCMOGenericControl::CDCMOGenericControl() 
// ----------------------------------------------------------------------------------------

CDCMOGenericControl::CDCMOGenericControl() 
	{	
	RDEBUG("CDCMOGenericControl::CDCMOGenericControl");
	}

// ----------------------------------------------------------------------------------------
// CDCMOGenericControl::~CDCMOGenericControl() 
// ----------------------------------------------------------------------------------------
CDCMOGenericControl::~CDCMOGenericControl()
{
	RDEBUG("CDCMOGenericControl::CDCMOGenericControl Destructor");
}

// ----------------------------------------------------------------------------------------
// CDCMOGenericControl::~CDCMOGenericControl() 
// ----------------------------------------------------------------------------------------
TInt CDCMOGenericControl::GetGroupValueL(TDes& aStrValue)
{
  RDEBUG("CDCMOGenericControl::GetGroupValueL(): begin");
  TInt ret(-1);
	if(aStrValue.Match(KDCMOHardware()) == KErrNone )
		ret = 0;
	else if( aStrValue.Match(KDCMOIO()) == KErrNone )
		ret = 1;
	else if( aStrValue.Match(KDCMOConnectivity()) == KErrNone )
		ret = 2;
	else if(	aStrValue.Match(KDCMOSoftware()) == KErrNone )
		ret = 3;
	else if(	aStrValue.Match(KDCMOService()) == KErrNone )
		ret = 4;
	
	RDEBUG("CDCMOGenericControl::GetGroupValueL(): end");
	return ret;		
}

// ----------------------------------------------------------------------------------------
// CDCMOGenericControl::GetIntAttributeL(TInt aCategotyNumber, TDCMONode aId, TInt& aValue)
// ----------------------------------------------------------------------------------------
TDCMOStatus CDCMOGenericControl::GetIntAttributeL(TInt aCategotyNumber, TDCMONode aId, TInt& aValue)
{
    RDEBUG("CDCMOGenericControl::GetIntAttributeL(): begin");
		TDCMOStatus err(EDcmoFail);
		TInt reterr(-1);
		switch(aId)
		{
				case EGroup: 
						{
							RDEBUG("CDCMOGenericControl::GetIntAttributeL(): In Group");
							TBuf<KBufferSize> group;
							CRepository * centrep = CRepository::NewLC( KCRUidDCMOServer );
							TUint32 centrepKey = aCategotyNumber + KDCMOGroupNumber;
							reterr = centrep->Get( centrepKey, group );
							CleanupStack::PopAndDestroy( centrep );
							aValue = GetGroupValueL(group);
						}
					break;
		
				case EAttached:
						{
							RDEBUG("CDCMOGenericControl::GetIntAttributeL(): In Attached");							
							aValue = 1; // Attached
							reterr = KErrNone;
						}
					break;
				case EEnabled:	
						{	
							RDEBUG("CDCMOGenericControl::GetIntAttributeL(): In Enabled");						
							CRepository * centrep = CRepository::NewLC( KCRUidDCMOServer );
							TUint32 centrepKey = aCategotyNumber + KDCMOMainKeyNumber;
							TInt centrepMainKey, centrepActualKey;
							reterr = centrep->Get( centrepKey, centrepMainKey );
							centrepKey = aCategotyNumber + KDCMOActualKeyNumber;							
							reterr = centrep->Get( centrepKey, centrepActualKey );
							TUid uidValue = {centrepMainKey};
							CRepository* centrep1 = CRepository::NewLC( uidValue );
							reterr = centrep1->Get( centrepActualKey, aValue );
							if( !aCategotyNumber ) // for Camera
							{
								if(aValue == 0)
								{
									aValue = 1;
								}
								else if(aValue == 1)
								{
									aValue = 0;
								} 
							}
							CleanupStack::PopAndDestroy( centrep1 );
							CleanupStack::PopAndDestroy( centrep );
						}
						break;
		
				case EDenyUserEnable:
						{
							RDEBUG("CDCMOGenericControl::GetIntAttributeL(): In DenyUserEnable");							
							aValue = 1; // DenyUserEnable
							reterr = KErrNone;
						}
						break;
				case ENotifyUser:
						{
							RDEBUG("CDCMOGenericControl::GetIntAttributeL(): In NotifyUser");							
							aValue = 1; // NotifyUser
							reterr = KErrNone;
						}
						break;
				default :
					{
						RDEBUG("CDCMOGenericControl::GetIntAttributeL(): Not Found");
						err = EDcmoNotFound;
					}
		}
		if(reterr == KErrNone)
			err = EDcmoSuccess;
	  RDEBUG("CDCMOGenericControl::GetIntAttributeL(): end");
	  return err;
}

// ----------------------------------------------------------------------------------------
// CDCMOGenericControl::GetStrAttributeL(TInt aCategotyNumber, TDCMONode aId, TDes& aStrValue)
// ----------------------------------------------------------------------------------------
TDCMOStatus CDCMOGenericControl::GetStrAttributeL(TInt aCategotyNumber, TDCMONode aId, TDes& aStrValue) 
{
	RDEBUG("CDCMOGenericControl::GetStrAttributeL(): begin");
	TDCMOStatus err(EDcmoFail);
	TInt reterr(-1);
	switch(aId)
	{
				case EProperty:
						{
							RDEBUG("CDCMOGenericControl::GetStrAttributeL(): In Property");
							CRepository * centrep = CRepository::NewLC( KCRUidDCMOServer );
							TUint32 centrepKey = aCategotyNumber + KDCMOPropertyNumber;
							reterr = centrep->Get( centrepKey, aStrValue );
							CleanupStack::PopAndDestroy( centrep );
						}
						break;
		
				case EDescription:
						{
							RDEBUG("CDCMOGenericControl::GetStrAttributeL(): In Description");
							CRepository * centrep = CRepository::NewLC( KCRUidDCMOServer );
							TUint32 centrepKey = aCategotyNumber + KDCMODescriptionNumber;
							reterr = centrep->Get( centrepKey, aStrValue );
							CleanupStack::PopAndDestroy( centrep );
						}	
						break;
				default :
						{
							RDEBUG("CDCMOGenericControl::GetStrAttributeL(): Not Found");
							err = EDcmoNotFound;
					  }
	
	}	
	if(reterr == KErrNone)
		err = EDcmoSuccess;		
	
	RDEBUG("CDCMOGenericControl::GetStrAttributeL(): end");
	return err;
}

// ----------------------------------------------------------------------------------------
// CDCMOGenericControl::SetIntAttributeL(TInt aCategotyNumber, TDCMONode aId, TInt aValue)
// ----------------------------------------------------------------------------------------
TDCMOStatus CDCMOGenericControl::SetIntAttributeL(TInt aCategotyNumber, TDCMONode aId, TInt aValue)
{
	RDEBUG("CDCMOGenericControl::SetIntAttributeL(): begin");
	TDCMOStatus err(EDcmoFail);
	TInt reterr(-1);
	switch(aId)
	{
				case EDenyUserEnable:
					{
							RDEBUG("CDCMOGenericControl::SetIntAttributeL(): In DenyUserEnable");	
							err = EDcmoNotSupported;
					}
					break;
				case ENotifyUser:
					{
							RDEBUG("CDCMOGenericControl::SetIntAttributeL(): In NotifyUser");	
							err = EDcmoNotSupported;
					}
					break;
				case EEnable:	
					{
							RDEBUG("CDCMOGenericControl::SetIntAttributeL(): In Enable");	
							if((aValue == 0)|| (aValue == 1))
							{
								CRepository * centrep = CRepository::NewLC( KCRUidDCMOServer );
								TUint32 centrepKey = aCategotyNumber + KDCMOMainKeyNumber;
								TInt centrepMainKey, centrepActualKey;
								reterr = centrep->Get( centrepKey, centrepMainKey );
								centrepKey = aCategotyNumber + KDCMOActualKeyNumber;							
								reterr = centrep->Get( centrepKey, centrepActualKey );
								TUid uidValue = {centrepMainKey};
								CRepository * centrep1 = CRepository::NewLC( uidValue );
								if( aCategotyNumber == 0) // this is only if the category is Camera
								{
									if(aValue == 0)
										aValue = 1;
									else if (aValue == 1)
										aValue = 0; 	
								}
								reterr = centrep1->Set( centrepActualKey, aValue );
								CleanupStack::PopAndDestroy( centrep1 );
								CleanupStack::PopAndDestroy( centrep );							
							}	
					}	
					break;
					default :
						{
							RDEBUG("CDCMOGenericControl::SetIntAttributeL(): Not Found");
							err = EDcmoNotFound;
					  }
	}
	if(reterr == KErrNone)
		err = EDcmoSuccess;			
	
	RDEBUG("CDCMOGenericControl::SetIntAttributeL(): end");
	return err;
}

// ----------------------------------------------------------------------------------------
// CDCMOGenericControl::SetStrAttributeL(TInt /*aCategotyNumber*/, TDCMONode /*aId*/, const TDes& /*aStrValue*/) 
// ----------------------------------------------------------------------------------------
TDCMOStatus CDCMOGenericControl::SetStrAttributeL(TInt /*aCategotyNumber*/, TDCMONode /*aId*/, const TDes& /*aStrValue*/)
{
	RDEBUG("CDCMOGenericControl::SetStrAttributeL(): begin");
	TDCMOStatus err(EDcmoFail);
	err = EDcmoNotSupported;
	RDEBUG("CDCMOGenericControl::SetStrAttributeL(): end");	
	return err;
}
