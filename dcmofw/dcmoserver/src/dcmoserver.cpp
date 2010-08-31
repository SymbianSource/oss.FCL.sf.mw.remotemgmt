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
* Description:  DCMO Server implementation
*
*/

#include <e32svr.h>
#include <implementationproxy.h>
#include <ecom.h>
#include <e32base.h>
#include <centralrepository.h>
#include <stringresourcereader.h> 
#include <data_caging_path_literals.hrh>
#include <f32file.h> 
#include <hbtextresolversymbian.h>
#include "dcmoclientserver.h"
#include "dcmoconst.h"
#include "dcmointerface.h"
#include "dcmoserver.h"
#include "dcmosession.h"
#include "dcmogenericcontrol.h"
#include "dcmodebug.h"


_LIT( KdcmoResourceFileName, "deviceupdates_" );	
_LIT( KdcmoResourceFilePath, "z:/resource/qt/translations/" );	
const TInt KBufferSize = 256;

TInt CDCMOServer::iSessionCount = 0;
// Standard server startup code
// 
static void StartServerL()
	{
	RDEBUG("DCMOServer: Starting server...");
	
	// EPOC and EKA 2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(User::RenameThread(KDCMOServerName));	
	
	// create and install the active scheduler
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);

	// create the server (leave it on the cleanup stack)
	CDCMOServer::NewLC();

	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);

	// Ready to run
	CActiveScheduler::Start();

	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

// ----------------------------------------------------------------------------------------
// Server process entry-point
// ----------------------------------------------------------------------------------------
TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,StartServerL());
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}

// ----------------------------------------------------------------------------------------
// CDCMOServer::NewLC
// ----------------------------------------------------------------------------------------
CServer2* CDCMOServer::NewLC()
{
	RDEBUG("CDCMOServer::NewLC- begin");
	CDCMOServer* self=new(ELeave) CDCMOServer;
	CleanupStack::PushL(self);
	self->ConstructL();	
	RDEBUG("CDCMOServer::NewLC- end");
	return self;
}

// ----------------------------------------------------------------------------------------
// CDCMOServer::ConstructL
// ----------------------------------------------------------------------------------------
void CDCMOServer::ConstructL()
	{
	RDEBUG("CDCMOServer::ConstructL- begin");
	StartL(KDCMOServerName);	
	RDEBUG("CDCMOServer::ConstructL- end");
	}

// ----------------------------------------------------------------------------------------
// CDCMOServer::CDCMOServer() 
// ----------------------------------------------------------------------------------------

CDCMOServer::CDCMOServer() : CServer2(EPriorityStandard, EUnsharableSessions) /*CServer2(0)*/
	{	
		iStarter = EFalse;
		iMessageBox = NULL;
	}

// ----------------------------------------------------------------------------------------
// CDCMOServer::~CDCMOServer() 
// ----------------------------------------------------------------------------------------
CDCMOServer::~CDCMOServer()
{	
	RDEBUG("CDCMOServer::~CDCMOServer- begin");
  TInt count = idcmoArray.Count();  
  RDEBUG_2("CDCMOServer::~CDCMOServer; %d", count );
	if(count)
	{	
		for(TInt i=0; i< count; i++)
				delete idcmoArray[i].iCategoryName;
		idcmoArray.Reset();
		delete iMessageBox;
		iMessageBox = NULL;	
	}
	else
	{
		//Kill the server
		if( iSessionCount == 0)
			CActiveScheduler::Stop();	
	}
	
	REComSession::FinalClose();
	RDEBUG("CDCMOServer::~CDCMOServer- end");
}

// -----------------------------------------------------------------------------
// CDCMOServer::DropSession()
// -----------------------------------------------------------------------------

void CDCMOServer::DropSession()
	{
		RDEBUG("CDCMOServer::DropSession- begin");
		iSessionCount--;
		if ( iStarter ) 
  	{
  		RDEBUG("CDCMOServer::DropSession(): Starter");
  		SetStarter( EFalse );
  		return;                 
  	} 
  	if(CDCMOMessageBox::IsMsgBoxClosed())
  	{
  		CleanDcmoArray();
  	}  
		if( idcmoArray.Count() && ( iSessionCount == 0 ))
		{
			// A session is being destroyed		
			TRAPD( err, DoFinalizeL());		
			if ( !err )
			{
				RDEBUG_2("CDCMOServer::DropSession err =  %d", err );
			}						
		}		
		RDEBUG("CDCMOServer::DropSession- end");
	}

// ----------------------------------------------------------------------------------------
// CDCMOServer::NewSessionL
// Create a new client session. This should really check the version number.
// ----------------------------------------------------------------------------------------
CSession2* CDCMOServer::NewSessionL(const TVersion& aVersion,const RMessage2&) const
	{
	RDEBUG("CDCMOServer::NewSessionL");

	// check we're the right version
	TVersion v(KDCMOServerMajorVersionNumber,
	           KDCMOServerMinorVersionNumber,
	           KDCMOServerBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		{
			RDEBUG("CDCMOServer::NewSessionL - KErrNotSupported");
			User::Leave(KErrNotSupported);
		}
	if( iSessionCount > 0 )	
	{
		RDEBUG("CDCMOServer::NewSessionL - KErrServerBusy");
		User::Leave(KErrServerBusy );		
	}
	else
		 iSessionCount++;
		 		
	return new (ELeave) CDCMOSession(); 
	}
	
void CleanupEComArray(TAny* aArray)
	{
	(static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
	(static_cast<RImplInfoPtrArray*> (aArray))->Close();
	}
// ----------------------------------------------------------------------------------------
// CDCMOServer::GetAdapterUidL
// Gets the plug-in adapter implementation uid if it present.
// ----------------------------------------------------------------------------------------
TUid CDCMOServer::GetAdapterUidL(const TDesC& aCategory)
{
	RDEBUG("CDCMOServer::GetDCMOAdapterUidL(): begin");
	
	TUid retUid = {0x0};
	
	RImplInfoPtrArray infoArray;
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	TCleanupItem cleanup(CleanupEComArray, &infoArray);
	CleanupStack::PushL(cleanup);
	REComSession::ListImplementationsL(KDCMOInterfaceUid, infoArray);

	// Loop through each info for each implementation			
	TBuf8<KBufferSize> buf;            
	for (TInt i=0; i< infoArray.Count(); i++)
	{
		buf = infoArray[i]->OpaqueData();
		TBuf8<KBufferSize> category;
		category.Copy(aCategory);
		if(category.Find(infoArray[i]->OpaqueData())!= KErrNotFound)
		{
			retUid = infoArray[i]->ImplementationUid();
			break;
		}
		buf.Zero();
	}
	CleanupStack::PopAndDestroy(); //cleanup
	
	RDEBUG("CDCMOServer::GetDCMOAdapterUidL(): end");
	return retUid;
}
// ----------------------------------------------------------------------------------------
// CDCMOServer::GetLocalCategoryL
// Gets the cenrep key value of the plug-in adapter if it is available.
// ----------------------------------------------------------------------------------------
TInt CDCMOServer::GetLocalCategoryL(const TDesC& aCategory)
{
	RDEBUG("CDCMOServer::GetLocalCategoryL(): begin");
	TInt ret(-1);	
	
	CRepository* centrep = NULL;
	TInt err(KErrNone);
	TRAP(err, centrep = CRepository::NewL( KCRUidDCMOServer ));
	if(err != KErrNone)
	{
		RDEBUG("CDCMOServer::GetLocalCategoryL(): DCMO central repository does not exist");
		return ret;
	}
	TUint32 centrepKey = 0;
	TInt iControl(0);
	TInt reterr = centrep->Get( centrepKey, iControl );	
	if(reterr!= KErrNone)
	{
			RDEBUG("CDCMOServer::GetLocalCategoryL(): centrep Get error");
			delete centrep;
			centrep = NULL;
	    return ret;
	}
	RDEBUG_2("CDCMOServer::GetLocalCategoryL local plug-in =  %d", iControl );
	TBuf<30> buffer;
	for(TInt i=0; i<iControl; i++)
	{   
        centrepKey = KDCMOKeyMaxNumber*i + KDCMOPropertyNumber ;
        reterr = centrep->Get( centrepKey, buffer );
	    	if(aCategory.Find(buffer)== KErrNone)
        {
        	ret = centrepKey-1;
        	break;
        }
	}
	delete centrep;
	centrep = NULL;
	RDEBUG("CDCMOServer::GetLocalCategoryL(): end");
	return ret;
}
// ----------------------------------------------------------------------------------------
// CDCMOServer::GetIntAttributeL
// Gets the integer attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus CDCMOServer::GetIntAttributeL(TDes& category, TDCMONode id, TInt& value)
{
	RDEBUG("CDCMOServer::GetIntAttributeL(): begin");
	TDCMOStatus err(EDcmoFail);
	TInt categotyNumber = GetLocalCategoryL(category);
	if(categotyNumber != -1)
	{
			RDEBUG("CDCMOServer::GetIntAttributeL(): LocalCategory");
			CDCMOGenericControl* iGenericControl = new(ELeave) CDCMOGenericControl;
			err = iGenericControl->GetIntAttributeL(categotyNumber, id, value);
			delete iGenericControl;
			iGenericControl = NULL;
	}
	else
	{
		RDEBUG("CDCMOServer::GetIntAttributeL(): Plug-in Adapter");
		TUid impluid = GetAdapterUidL(category);		
		if(impluid.iUid)
		{
			CDCMOInterface::TDCMOInterfaceInitParams initParms;
			initParms.uid = impluid;
			initParms.descriptor = &category;
			CDCMOInterface* ex = CDCMOInterface::NewL(initParms); //impluid);
			CleanupStack::PushL(ex);
			err = ex->GetDCMOPluginIntAttributeValueL(id,value);
			CleanupStack::PopAndDestroy(); //ex 	
		}
	}
	RDEBUG("CDCMOServer::GetIntAttributeL(): end");
	return err;
}
// ----------------------------------------------------------------------------------------
// CDCMOServer::GetStrAttributeL
// Gets the string attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus CDCMOServer::GetStrAttributeL(TDes& category, TDCMONode id, TDes& strValue) 
{
	RDEBUG("CDCMOServer::GetStrAttributeL(): begin");
	TDCMOStatus err(EDcmoFail);
	TInt categotyNumber = GetLocalCategoryL(category);
	if(categotyNumber != -1)
	{
			RDEBUG("CDCMOServer::GetStrAttributeL(): LocalCategory");
			CDCMOGenericControl* iGenericControl = new(ELeave) CDCMOGenericControl;
			err = iGenericControl->GetStrAttributeL(categotyNumber, id, strValue);	
			delete iGenericControl;
			iGenericControl = NULL;
	}
	else
	{
		RDEBUG("CDCMOServer::GetStrAttributeL(): Plug-in Adapter");
		TUid impluid = GetAdapterUidL(category);
		if(impluid.iUid)
		{
			CDCMOInterface::TDCMOInterfaceInitParams initParms;
		  initParms.uid = impluid;
		  initParms.descriptor = &category;
		  CDCMOInterface* ex = CDCMOInterface::NewL(initParms);			
			CleanupStack::PushL(ex);
			err = ex->GetDCMOPluginStrAttributeValueL(id,strValue); 
			CleanupStack::PopAndDestroy(); //ex
		}
	}
	RDEBUG("CDCMOServer::GetStrAttributeL(): end");
	return err;
}
// ----------------------------------------------------------------------------------------
// CDCMOServer::SetIntAttributeL
// Sets the integer attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus CDCMOServer::SetIntAttributeL(TDes& category, TDCMONode id, TInt value)
{
	RDEBUG("CDCMOServer::SetIntAttributeL(): begin");
	TDCMOStatus err(EDcmoFail);
	HBufC* stringHolder( NULL );
	TInt categotyNumber = GetLocalCategoryL(category);
	struct dcmoInfoList dcmoList;
	if(categotyNumber != -1)
	{
			RDEBUG("CDCMOServer::SetIntAttributeL(): LocalCategory");
			CDCMOGenericControl* iGenericControl = new(ELeave) CDCMOGenericControl;
			err = iGenericControl->SetIntAttributeL(categotyNumber, id, value);	
			
			TBool result = HbTextResolverSymbian::Init(KdcmoResourceFileName, KdcmoResourceFilePath );					

			TPtrC buf;
			dcmoList.iUid = categotyNumber;
			if(categotyNumber == 0)
			    {
					_LIT(KTextCamera, "txt_device_update_info_camera");
					stringHolder = HbTextResolverSymbian::LoadL(KTextCamera);
			    } 
			else
			    {	
					_LIT(KTextFOTA, "txt_device_update_info_firmware_update");
					stringHolder = HbTextResolverSymbian::LoadL(KTextFOTA);
			    }		
  	 delete iGenericControl;
		 iGenericControl = NULL;
	}
	else
	{
		RDEBUG("CDCMOServer::SetIntAttributeL(): Plug-in Adapter");
		TUid impluid = GetAdapterUidL(category);
		dcmoList.iUid = impluid.iUid;
		if(impluid.iUid)
		{
			CDCMOInterface::TDCMOInterfaceInitParams initParms;
		  initParms.uid = impluid;
		  initParms.descriptor = &category;
		  CDCMOInterface* ex = CDCMOInterface::NewL(initParms); 
			CleanupStack::PushL(ex);
			err = ex->SetDCMOPluginIntAttributeValueL(id,value);	
			RDEBUG("CDCMOServer::SetIntAttributeL - SetDCMOPluginIntAttributeValueL(): end");				
			ex->GetLocalizedNameL(stringHolder);
			RDEBUG("CDCMOServer::SetIntAttributeL - GetLocalizedNameL(): end");				
			CleanupStack::PopAndDestroy(); //ex 	

		}
	}
	if ( iStarter )   			
   {
  		RDEBUG("CDCMOServer::SetIntAttributeL(): Starter");
  		SetStarter ( EFalse );
  		delete stringHolder;
			stringHolder = NULL;
  		return err;                
   }   
	if((err == EDcmoSuccess) && (id == EEnable) ) 
	{
		TInt arrayCount = idcmoArray.Count();
		RDEBUG_2("CDCMOServer::SetIntAttributeL arrayCount = %d", arrayCount );
		TBool insert = EFalse; 		
		if( arrayCount > 0)
		{
			for(TInt i = 0; i< arrayCount ; i++)
			{       	
      	if (idcmoArray[i].iUid == dcmoList.iUid)
      	{
      		idcmoArray[i].iStatus = (TBool) value;	
      		insert = ETrue;
      		break;
      	}
      }
    }
		if ( !insert )
		{
			dcmoList.iCategoryName = stringHolder->AllocL();
			dcmoList.iStatus = (TBool) value;		
			idcmoArray.AppendL(dcmoList);
			RDEBUG("CDCMOServer::SetIntAttributeL - dcmoList added ");		
		}
	}
	delete stringHolder;
	stringHolder = NULL;
	RDEBUG("CDCMOServer::SetIntAttributeL(): end");
	return err;
}
// ----------------------------------------------------------------------------------------
// CDCMOServer::SetStrAttributeL
// Sets the string attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus CDCMOServer::SetStrAttributeL(TDes& category, TDCMONode id, const TDes& strValue)
{
	RDEBUG("CDCMOServer::SetStrAttributeL(): begin");
	TUid impluid = GetAdapterUidL(category);
	TDCMOStatus err(EDcmoFail);
	if(impluid.iUid)
	{
		CDCMOInterface* ex = CDCMOInterface::NewL(impluid);
		CleanupStack::PushL(ex);
		err = ex->SetDCMOPluginStrAttributeValueL(id,strValue);
		CleanupStack::PopAndDestroy(); //ex 
	}
	RDEBUG("CDCMOServer::SetStrAttributeL(): end");	
	return err;
}
// ----------------------------------------------------------------------------------------
// CDCMOServer::DoFinalizeL()
// Calls when all the operations are done. Shows the notification
// ----------------------------------------------------------------------------------------
void CDCMOServer::DoFinalizeL()
{
	RDEBUG("CDCMOServer::DoFinalizeL(): begin");	   

	HBufC* content  = HBufC::NewLC(KDCMOMaxStringSize);
  TPtr   contentptr  = content->Des(); 
	HBufC* enableContent  = HBufC::NewLC(KDCMOMaxStringSize);
	TPtr   enableContentptr  = enableContent->Des(); 
	HBufC* disableContent  = HBufC::NewLC(KDCMOMaxStringSize);
	TPtr   disableContentptr  = disableContent->Des(); 

	TBool enable ( EFalse );
	TBool disable ( EFalse );
	TInt arrayCount = idcmoArray.Count(); 
	_LIT(KNewLine, "\n");
		
	if( arrayCount > 0)
	{
		RDEBUG_2("CDCMOServer::DoFinalizeL arrayCount= %d", arrayCount );
		for(TInt i = 0; i< arrayCount ; i++)
		{       	
      	if (idcmoArray[i].iStatus )
      	{
      		enableContentptr.Append(KNewLine());
      		enableContentptr.Append( idcmoArray[i].iCategoryName->Des() );
      		enable = ETrue;
      	}
      	else
      	{
      		disableContentptr.Append(KNewLine());
      		disableContentptr.Append( idcmoArray[i].iCategoryName->Des() );
      		disable = ETrue;
      	}	
		}	  
  
		TBool result = HbTextResolverSymbian::Init(KdcmoResourceFileName, KdcmoResourceFilePath );
		if ( enable )
		{
			_LIT(KTextEnabled, "txt_device_update_title_enabled_by_the_system_admi");
			HBufC* buf = HbTextResolverSymbian::LoadL(KTextEnabled);
			contentptr.Append(buf->Des());
	 		contentptr.Append(enableContentptr);
	 		delete buf;
		}
		if ( disable )
		{
	 		_LIT(KTextDisabled, "txt_device_update_title_disabled_by_the_system_adm");
			HBufC* buf = HbTextResolverSymbian::LoadL(KTextDisabled);
	 		if( enable )
	 			contentptr.Append(KNewLine());	 		
			contentptr.Append(buf->Des());
	 		contentptr.Append(disableContentptr);
	 		delete buf;
		}
	
		if( !iMessageBox )
		{		
			iMessageBox = CDCMOMessageBox::NewL();					
		}
		iMessageBox->ShowMessageL(contentptr);

	  CleanupStack::PopAndDestroy(3); //disableContent, enableContent, content
	}	
	RDEBUG("CDCMOServer::DoFinalizeL(): end");
}
// ----------------------------------------------------------------------------------------
// CDCMOServer::SearchAdaptersL
// Gets all the plug-in adapter names
// ----------------------------------------------------------------------------------------
void CDCMOServer::SearchAdaptersL(TDes& /* category */, TDes& aAdapterList)
{
	RDEBUG("CDCMOServer::SearchAdaptersL(): begin");
	
	CRepository* centrep = NULL;
	TInt err(KErrNone);
	TInt count(0);
	TBool flag(EFalse);
	TRAP(err, centrep = CRepository::NewL( KCRUidDCMOServer ));
	if(err == KErrNone)
	{
      TUint32 centrepKey = 0;	    
	    TInt reterr = centrep->Get( centrepKey, count );  
	    RDEBUG_2("CDCMOServer::SearchAdaptersL count= %d", count );  
	    if(reterr == KErrNone)
	    {       
          TBuf<KBufferSize> buffer;
	        for(TInt i=0; i<count; i++)
	        {   
              centrepKey = KDCMOKeyMaxNumber*i + KDCMOPropertyNumber ;
	            reterr = centrep->Get( centrepKey, buffer );
	            if( reterr== KErrNone )
	            {   
                    if((count > 1) && flag)
                        aAdapterList.Append(_L(", ")); 
                    aAdapterList.Append(buffer); 
                    buffer.Zero(); 
                    flag = ETrue;
	            }
	        }
	    } 
	}
	delete centrep;
	centrep = NULL;
	
	RImplInfoPtrArray infoArray;
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	TCleanupItem cleanup(CleanupEComArray, &infoArray);
	CleanupStack::PushL(cleanup);
	REComSession::ListImplementationsL(KDCMOInterfaceUid, infoArray);

	// Loop through each info for each implementation			
	TBuf<KBufferSize> buf;   
	count =  infoArray.Count();
	RDEBUG_2("CDCMOServer::SearchAdaptersL plug-in count= %d", count );          
	for (TInt i=0; i< count; i++)
	{
		if(( i != 0) || flag )
			aAdapterList.Append(_L(", "));		
		buf.Copy(infoArray[i]->OpaqueData());	
		aAdapterList.Append(buf);
		buf.Zero();		
	}
	CleanupStack::PopAndDestroy(); //cleanup
	
	RDEBUG("CDCMOServer::SearchAdaptersL(): end");	
}

// ----------------------------------------------------------------------------------------
// CDCMOServer::SetStarter
// Sets the iStarter value
// ----------------------------------------------------------------------------------------
void CDCMOServer::SetStarter(TBool aValue)
{
	RDEBUG("CDCMOServer::SetStarter(): begin");
	iStarter = aValue;
	RDEBUG("CDCMOServer::SetStarter(): end");
}

// ----------------------------------------------------------------------------------------
// CDCMOServer::CleanDcmoArray
// Sets the iStarter value
// ----------------------------------------------------------------------------------------
void CDCMOServer::CleanDcmoArray()
{
	RDEBUG("CDCMOServer::CleanDcmoArray(): begin");
	TInt count = idcmoArray.Count();  
  RDEBUG_2("CDCMOServer::CleanDcmoArray; %d", count );
	if(count)
	{	
		for(TInt i=0; i< count; i++)
				delete idcmoArray[i].iCategoryName;
		idcmoArray.Reset();
	}
	CDCMOMessageBox::SetMsgBoxStatus(EFalse);
	RDEBUG("CDCMOServer::CleanDcmoArray(): end");
}
