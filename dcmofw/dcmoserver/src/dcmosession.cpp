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
* Description:  DCMO Session implementation
*
*/

#include <e32svr.h>

#include "dcmosession.h"
#include "dcmoclientserver.h"
#include "dcmodebug.h"
#include "lawmodebug.h"

// ----------------------------------------------------------------------------------------
// CDCMOSession::CDCMOSession
// ----------------------------------------------------------------------------------------
CDCMOSession::CDCMOSession()
	{
		RDEBUG("CDCMOSession::CDCMOSession");
	}

// ----------------------------------------------------------------------------------------
// CDCMOSession::Server
// ----------------------------------------------------------------------------------------
CDCMOServer& CDCMOSession::Server()
	{
		RDEBUG("CDCMOSession::Server");	
		return *static_cast<CDCMOServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// ----------------------------------------------------------------------------------------
// CDCMOSession::~CDCMOSession
// ----------------------------------------------------------------------------------------
CDCMOSession::~CDCMOSession()
	{
		RDEBUG("CDCMOSession::~CDCMOSession");
		Server().DropSession();
	}

// ----------------------------------------------------------------------------------------
// CDCMOSession::ServiceL
// ----------------------------------------------------------------------------------------
void CDCMOSession::ServiceL(const RMessage2& aMessage)
    {
			RDEBUG("CDCMOSession::ServiceL");
    	TRAPD(err,DispatchMessageL(aMessage));
    	if(err)    		
    		RDEBUG_2("CDCMOSession::ServiceL - err %d", err );      	
			aMessage.Complete(err);
    }

// ----------------------------------------------------------------------------------------
// CDCMOSession::DispatchMessageL
// ----------------------------------------------------------------------------------------
void CDCMOSession::DispatchMessageL(const RMessage2& aMessage)
	{
	RDEBUG_2("CDCMOSession::DispatchMessageL; %d", aMessage.Function() );
  TInt value; 
  TDCMOStatus statusValue;
  TLawMoStatus lstatusValue;
  TDCMONode nodeValue;
  TPckg<TDCMOStatus> status(statusValue);
  TPckg<TLawMoStatus> lstatus(lstatusValue);
  TPckg<TDCMONode> node(nodeValue);
  TPckg<TInt> intvalue(value);
  
  HBufC*     category  = HBufC::NewLC(KDCMOMaxStringSize);
  TPtr       categoryptr  = category->Des(); 
  aMessage.Read (0, categoryptr);           
  aMessage.Read (1, node);          		
	switch(aMessage.Function())
	    {
			case EDcmoGetAttrInt:
					{	                 
            		statusValue = Server().GetIntAttributeL(categoryptr, nodeValue, value);            
            		aMessage.Write (2, intvalue);
            		aMessage.Write (3, status);   
            		RDEBUG_2("CDCMOSession::DispatchMessageL - EDcmoGetAttrInt status %d", statusValue );            		        
					}
					break;
			case EDcmoGetAttrStr:
					{ 	
            		HBufC*     buf  = HBufC::NewLC(KDCMOMaxStringSize);
            		TPtr       bufptr  = buf->Des();         
            		statusValue = Server().GetStrAttributeL(categoryptr, nodeValue, bufptr);
            		aMessage.Write (2, bufptr);            		
            		aMessage.Write (3, status);               		
            		CleanupStack::PopAndDestroy(); //buf
            		RDEBUG_2("CDCMOSession::DispatchMessageL - EDcmoGetAttrStr status %d", statusValue );            		
					}
					break;
			case EDcmoSetAttrInt:
					{     
								if ( KStarterUid == aMessage.SecureId()) 
								{
									RDEBUG("CDCMOSession:: DispatchMessageL client is starter");
									Server().SetStarter( ETrue );
								}       
            		else if ( !aMessage.HasCapability( ECapabilityDiskAdmin ) )
        				{
        					statusValue = EDcmoAccessDenied;
        					aMessage.Write (3, status); 
        					CleanupStack::PopAndDestroy(category); //category
        					return;        
        				}          
            		aMessage.Read (2, intvalue);            
           			statusValue = Server().SetIntAttributeL(categoryptr, nodeValue, value);           			           		
            		aMessage.Write (3, status);   
            		RDEBUG_2("CDCMOSession::DispatchMessageL - EDcmoSetAttrInt status %d", statusValue );
					}
					break;
			
			case EDcmoSetAttrStr:
					{	
           			if ( KStarterUid == aMessage.SecureId() ) 
								{
									RDEBUG("CDCMOSession:: DispatchMessageL client is starter");
									Server().SetStarter( ETrue );
								}       
            		else if ( !aMessage.HasCapability( ECapabilityDiskAdmin ) )
        				{
        					statusValue = EDcmoAccessDenied;
        					aMessage.Write (3, status); 
        					CleanupStack::PopAndDestroy( ); //category
        					return;        
        				}
            		HBufC*     buf  = HBufC::NewLC(KDCMOMaxStringSize);
            		TPtr       bufptr  = buf->Des(); 
            		aMessage.Read (2, bufptr);            
            		statusValue = Server().SetStrAttributeL(categoryptr, nodeValue, bufptr); 
            		aMessage.Write (3, status);             		
            		CleanupStack::PopAndDestroy(); // buf
            		RDEBUG_2("CDCMOSession::DispatchMessageL - EDcmoSetAttrStr status %d", statusValue );
					}		
					break;	
	    case EDcmoSearchAdapter:
				{   
						HBufC*     buf  = HBufC::NewLC(KDCMOMaxStringSize);
            TPtr       bufptr  = buf->Des();
						Server().SearchAdaptersL(categoryptr, bufptr); 
						aMessage.Write (1, bufptr);
						CleanupStack::PopAndDestroy(); //buf
						RDEBUG("CDCMOSession::DispatchMessageL - EDcmoSearchAdapter status " );
	    	}
	    	break;	
	    case EWipe:
	        {
            //HBufC*     item  = HBufC::NewLC(KDCMOMaxStringSize);
            //TPtr       itemPtr  = category->Des(); 
            //aMessage.Read (0, itemPtr); 
	        //aMessage.Complete(KErrNone);
	        //RDEBUG("CDCMOSession::EWipe force complete the request");
	        lstatusValue = Server().WipeItem(); 
            aMessage.Write (0, lstatus);
            //CleanupStack::PopAndDestroy();
            RLDEBUG_2("CDCMOSession::DispatchMessageL EWipe %d", lstatusValue );
	        }
	        break;
	        
	    case EWipeAll:
	        {
	        //TInt aForce(1);
	        lstatusValue = Server().WipeAllItem(); 
	        aMessage.Write (0, lstatus);
            RLDEBUG_2("CDCMOSession::DispatchMessageL EWipeAll %d", lstatusValue );
	        }
	        break;
	        
	    case EListItemName_Get:
	        {
	        HBufC*     buf1  = HBufC::NewLC(KDCMOMaxStringSize);
	        TPtr       bufPtr1  = buf1->Des();
	        HBufC*     item  = HBufC::NewLC(KDCMOMaxStringSize);
	        TPtr       itemPtr  = item->Des(); 
	        aMessage.Read (0, itemPtr); 
	        lstatusValue = Server().GetListItemL(itemPtr, bufPtr1); 
	        aMessage.Write (1, bufPtr1);
	        aMessage.Write (2, lstatus);
	        CleanupStack::PopAndDestroy(2);
	        RLDEBUG_2("CDCMOSession::DispatchMessageL EListItemName_Get %d", lstatusValue );
	        }
	        break;
	        
	    case EToBeWiped_Get:
	        {
	        HBufC*     item  = HBufC::NewLC(KDCMOMaxStringSize);
            TPtr       itemPtr  = item->Des();
            HBufC*     buf1  = HBufC::NewLC(KDCMOMaxStringSize);
            TPtr       bufPtr1  = buf1->Des();
            aMessage.Read (0, itemPtr);
            lstatusValue = Server().GetToBeWipedL(itemPtr, bufPtr1);
            aMessage.Write (1, bufPtr1);
            aMessage.Write (2, lstatus);
            CleanupStack::PopAndDestroy(2);
            RLDEBUG_2("CDCMOSession::DispatchMessageL EToBeWiped_Get %d", lstatusValue );
	        }
	        break;
	            
	      case EToBeWiped_Set:
            {
            //TLex lex;
            //TInt val;
            //TBuf<255> wipeValue;
            HBufC*     item  = HBufC::NewLC(KDCMOMaxStringSize);
            TPtr       itemPtr  = item->Des();
            aMessage.Read (0, itemPtr);
            TInt val = aMessage.Int1();
            //aMessage.Read (1, wipeValue); 
            RLDEBUG_2("Tobewiped category %s",itemPtr.PtrZ());
            //lex.Assign( wipeValue );
            //TInt err = lex.Val( val );
            RLDEBUG_2("Tobewiped value,int %d", val );
            //if(err==KErrNone)
            //    {
            //    RLDEBUG("SetToBeWiped call" );
                lstatusValue = Server().SetToBeWipedL(itemPtr, val);
            //    }
            //else
            //    {
            //    RLDEBUG("SetToBeWiped not" );
            //    lstatusValue = ELawMoFail;
            //    }
            aMessage.Write (2, lstatus);
            CleanupStack::PopAndDestroy();  
            RLDEBUG_2("CDCMOSession::DispatchMessageL EToBeWiped_Set %d", lstatusValue );
            }
            break;
	    default :
	    		RDEBUG("CDCMOSession::DispatchMessageL- Case Not Found" );
	    }	  

		CleanupStack::PopAndDestroy(); //category  
  	RDEBUG("CDCMOSession::DispatchMessageL end" );
}
// ----------------------------------------------------------------------------------------
// CDCMOSession::ServiceError
// Handle an error from CHelloWorldSession::ServiceL()
// ----------------------------------------------------------------------------------------
void CDCMOSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
		RDEBUG_2("CDCMOSession::ServiceError %d", aError);
		CSession2::ServiceError(aMessage,aError);
	}

// ----------------------------------------------------------------------------------------
// End of file
