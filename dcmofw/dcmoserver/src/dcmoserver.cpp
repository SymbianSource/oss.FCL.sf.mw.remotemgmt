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
#include <dcmo.rsg> // Resource to be read header
#include <AknGlobalMsgQuery.h>
#include <data_caging_path_literals.hrh>
#include <f32file.h> 
#include "dcmoclientserver.h"
#include "dcmoconst.h"
#include "dcmointerface.h"
#include "dcmoserver.h"
#include "dcmosession.h"
#include "dcmogenericcontrol.h"
#include "dcmodebug.h"
#include "lawmodebug.h"
#include "amsmlhelper.h"
#include <lawmoadaptercrkeys.h>
#include <DevManInternalCRKeys.h>

#include    <e32property.h>
#include    <PSVariables.h>   // Property values
#include    <lawmointerface.h>
_LIT( KdcmoResourceFileName, "z:dcmo.rsc" );	
const TInt KBufferSize = 256;
const TInt KWipeSuccess = 1201;
const TInt KWipeFailure = 1405;

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
	iwipeStatus = 0;
	iSessionIap = KErrNotFound;
	iCount = -1;
	StartL(KDCMOServerName);	
	RDEBUG("CDCMOServer::ConstructL- end");
	}

// ----------------------------------------------------------------------------------------
// CDCMOServer::CDCMOServer() 
// ----------------------------------------------------------------------------------------

CDCMOServer::CDCMOServer() : CServer2(EPriorityStandard, EUnsharableSessions) /*CServer2(0)*/
	{	
		iStarter = EFalse;
		iNotifier = NULL;
	}

// ----------------------------------------------------------------------------------------
// CDCMOServer::~CDCMOServer() 
// ----------------------------------------------------------------------------------------
CDCMOServer::~CDCMOServer()
{	
	RDEBUG("CDCMOServer::~CDCMOServer- begin");
  TInt count = idcmoArray.Count();  
	RDEBUG_2("~~CDCMOServer::~CDCMOServer; %d", count );
	if(count)
	{	
		for(TInt i=0; i< count; i++)
				delete idcmoArray[i].iCategoryName;
		idcmoArray.Reset();
		delete iNotifier;
		iNotifier = NULL;	
	}
	
    RLDEBUG("CDCMOServer::wipe done delete pluginuid()");
    TInt pluginObjectCount = ilawmoPlugins.Count();           
    while(pluginObjectCount>0)
	{
        RLDEBUG("plugin to be deleted");
        delete ilawmoPlugins[pluginObjectCount-1];
        RLDEBUG("plugin deleted");
        pluginObjectCount--;
	}
    RLDEBUG("CDCMOServer::close all RArrays()");
    ilawmoPluginUidToBeWiped.Close();
	
    RLDEBUG("CDCMOServer::closeD all RArrays()");
    ilawmoPlugins.Close();
		
    RDEBUG("CDCMOServer::call FinalClose");	
	REComSession::FinalClose();
	RDEBUG("~~~CDCMOServer::~CDCMOServer- end");
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
		if( idcmoArray.Count() && ( iSessionCount == 0 ))
		{
			// A session is being destroyed		
            RDEBUG("CDCMOServer::DropSession- dofinalise ?");
			TRAPD( err, DoFinalizeL());		
			iCount = 0;
			if ( !err )
			{
				RDEBUG_2("CDCMOServer::DropSession err =  %d", err );
			}						
		}		
		if((iSessionCount == 0)&&(iCount== -1))
		    {
            RDEBUG("DropSession kill server, only when no session and no wipe pending");
            CActiveScheduler::Stop();
            RDEBUG("DropSession kill server");
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
TUid CDCMOServer::GetAdapterUidL(const TDesC& aCategory, TBool aIsLawmo)
{
	RDEBUG("CDCMOServer::GetDCMOAdapterUidL(): begin");
    RLDEBUG("CDCMOServer::GetAdapterUidL(): begin");
	TUid retUid = {0x0};
	TEComResolverParams resolverParams;
	RImplInfoPtrArray infoArray;
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	TCleanupItem cleanup(CleanupEComArray, &infoArray);
	CleanupStack::PushL(cleanup);
	if(aIsLawmo)
	    {
	    REComSession::ListImplementationsL(KLAWMOPluginInterfaceUid, resolverParams, KRomOnlyResolverUid, infoArray);
	    RLDEBUG("CDCMOServer::GetAdapterUidL(): listImpl");
	    }
	else
	REComSession::ListImplementationsL(KDCMOInterfaceUid, infoArray);

	// Loop through each info for each implementation			
	TBuf8<KBufferSize> buf;            
    RLDEBUG("CDCMOServer::GetAdapterUidL(): for loop");
	for (TInt i=0; i< infoArray.Count(); i++)
	{
		buf = infoArray[i]->OpaqueData();
		TBuf8<KBufferSize> category;
		category.Copy(aCategory);
		if(category.Find(infoArray[i]->OpaqueData())!= KErrNotFound)
		{
			retUid = infoArray[i]->ImplementationUid();
                    RLDEBUG("CDCMOServer::GetAdapterUidL(): matched");
			break;
		}
		buf.Zero();
	}
	CleanupStack::PopAndDestroy(); //cleanup
    RLDEBUG("CDCMOServer::GetAdapterUidL():end");
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
			TFileName myFileName;
  		TParse parseObj;
  		parseObj.Set( KdcmoResourceFileName(), &KDC_RESOURCE_FILES_DIR,NULL );
 			myFileName = parseObj.FullName();
 			CStringResourceReader* test = CStringResourceReader::NewL( myFileName );
			TPtrC buf;
			dcmoList.iUid = categotyNumber;
			if(categotyNumber == 0)
			    {			    
			    buf.Set(test->ReadResourceString(R_DM_RUN_TIME_VAR_CAMERA));
			    stringHolder = buf.AllocL() ; 
			    } 
			else
			    {			   
			    buf.Set(test->ReadResourceString(R_DM_RUN_TIME_VAR_FIRMWARE_UPDATE));
			    stringHolder = buf.AllocL() ; 
			    }			
     delete test;
     test = NULL;
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
	TFileName myFileName;
  TParse parseObj;
  parseObj.Set( KdcmoResourceFileName(), &KDC_RESOURCE_FILES_DIR,NULL );
  myFileName = parseObj.FullName();
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
	  
  	CStringResourceReader* test = CStringResourceReader::NewL( myFileName );	  
		if ( enable )
		{
			TPtrC buf;
			buf.Set(test->ReadResourceString(R_DM_RUN_TIME_VAR_ENABLE)); 	    	
			contentptr.Append(buf);
	 		contentptr.Append(enableContentptr);
		}
		if ( disable )
		{
	 		TPtrC buf;
	 		buf.Set(test->ReadResourceString(R_DM_RUN_TIME_VAR_DISABLE));
	 		if( enable )
	 			contentptr.Append(KNewLine());
	 		contentptr.Append(buf);
	 		contentptr.Append(disableContentptr);
		}
		delete test;
		test = NULL;
		
		if( iNotifier )
		{
			iNotifier->Cancel();
		}
		else
		{			
			iNotifier = CDCMONotifierAob::NewL( );			
		}
		
	  iNotifier->ShowNotifierL(contentptr);    
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
// CDCMOServer::GetPluginUids
// Gets the plug-in adapter implementation uid if it present.
// ----------------------------------------------------------------------------------------
void CDCMOServer::GetLawmoPluginUidsL()
{
    RDEBUG("CDCMOServer::GetPluginUids(): begin");
    ilawmoPluginUidToBeWiped.Reset();
    RImplInfoPtrArray infoArray;
    TEComResolverParams resolverParams;
    // Note that a special cleanup function is required to reset and destroy
    // all items in the array, and then close it.
    TCleanupItem cleanup(CleanupEComArray, &infoArray);
    CleanupStack::PushL(cleanup);
    REComSession::ListImplementationsL(KLAWMOPluginInterfaceUid, resolverParams, KRomOnlyResolverUid, infoArray);
    RLDEBUG("CDCMOServer::GetPluginUids(): listImpl");
    // Loop through each info for each implementation           
    for (TInt i=0; i< infoArray.Count(); i++)
    {
    ilawmoPluginUidToBeWiped.Append(infoArray[i]->ImplementationUid());
    RLDEBUG("CDCMOServer::GetPluginUids(): for loop");
    }
    CleanupStack::PopAndDestroy(); //cleanup
    RLDEBUG("CDCMOServer::GetPluginUids():end");
    return;
}

TLawMoStatus CDCMOServer::WipeAllItem()
    {
    //Update ilawmopluginUid, so that all Node items are wiped.
    //WipeItem doesn only on Uid's in the RArray.
    TRAPD(error,GetLawmoPluginUidsL());
    if(error == KErrNone)
    return WipeItem();
    else
        return ELawMoWipeNotPerformed;
    }

TLawMoStatus CDCMOServer::WipeItem(TInt aValue)
    {
    RLDEBUG("CDCMOServer::WipeItem(): begin");
    TLawMoStatus lawmostat(ELawMoAccepted);
    iCount = 0;
    if(ilawmoPluginUidToBeWiped.Count() > 0)
        {
		RLDEBUG_2("CDCMOServer::WipeItem got uid(): %d",ilawmoPluginUidToBeWiped[iCount]);
        CLAWMOPluginInterface* obj;
		RLDEBUG("CDCMOServer::WipeItem(): NewL");
        TRAPD(err,obj = CLAWMOPluginInterface::NewL(ilawmoPluginUidToBeWiped[iCount], this));
        if(err == KErrNone)
            {
            RLDEBUG("CDCMOServer::WipeItem(): obj created");
            RLDEBUG_2("CDCMOServer::WipeItem plugin count: %d",ilawmoPlugins.Count());
            TRAP(err,obj->WipeL());
        RLDEBUG_2("CDCMOServer::WipeItem(): wipe called %d",err);
        ilawmoPlugins.Append(obj);
        // Get the IAP being used in the current session
        TRAP( err, SmlHelper::GetDefaultIAPFromDMProfileL( iSessionIap ) );
        RLDEBUG_2("CDCMOServer::HandleWipeCompleted(): get iap %d",iSessionIap);
            }
        if(err!=KErrNone)
        HandleWipeCompleted(KErrGeneral);
        }
    else
        {
        lawmostat = ELawMoWipeNotPerformed;
        }
    
    RLDEBUG("CDCMOServer::WipeItem(): End");
    return lawmostat;
    }

TLawMoStatus CDCMOServer::GetListItemL(TDesC& item, TDes& strValue)
{
    RLDEBUG("CDCMOServer::GetListItem(): begin");
    TEComResolverParams resolverParams;
	TLawMoStatus lawmostat(ELawMoSuccess);
    TBuf<KBufferSize> itemName;
    RImplInfoPtrArray infoArray;
    TCleanupItem cleanup(CleanupEComArray, &infoArray);
    CleanupStack::PushL(cleanup);
    REComSession::ListImplementationsL(KLAWMOPluginInterfaceUid, resolverParams, KRomOnlyResolverUid, infoArray);
    RLDEBUG("CDCMOServer::GetListItem(): listImpl");

    for (TInt i=0; i< infoArray.Count(); i++)
        {   
        RLDEBUG("CDCMOServer::GetListItem(): for loop");
        TBuf<KBufferSize> temp;
        temp.Copy(infoArray[i]->OpaqueData());
        RLDEBUG_2("CDCMOServer::GetListItem opaque data to compare is %s", temp.PtrZ());
        if(item == temp)
            {
                itemName.Copy(infoArray[i]->DisplayName());
                RLDEBUG("CDCMOServer::GetListItem(): matched");
                break;
            }
        }
    CleanupStack::PopAndDestroy(); //cleanup
    RLDEBUG_2("CDCMOServer::GetListItem display name is %s", itemName.PtrZ());
    strValue.Zero();
    strValue.Append(itemName);
    RLDEBUG("CDCMOServer::GetListItem(): End");
    return lawmostat;
}


TLawMoStatus CDCMOServer::GetToBeWipedL(TDesC& item, TDes& wipeValue)
{
    RLDEBUG("CDCMOServer::GetToBeWiped(): begin");
    TLawMoStatus lawmostat(ELawMoSuccess);
    TInt wipeVal(0);
    TUid impluid = GetAdapterUidL(item, ETrue);
    RLDEBUG_2("CDCMOServer::GetToBeWiped for uid(): %d", impluid);
    TInt afind = ilawmoPluginUidToBeWiped.Find(impluid);
    if(afind!=KErrNotFound)
        {
        wipeVal = 1;
        RLDEBUG("CDCMOServer::GetToBeWiped(): uid in Rarray");
        }
    wipeValue.Zero();
    wipeValue.Num(wipeVal);
    RLDEBUG("CDCMOServer::GetToBeWiped(): End");
    return lawmostat;
}

TLawMoStatus CDCMOServer::SetToBeWipedL(TDesC& item, TInt wipeValue)
{
    RLDEBUG("CDCMOServer::SetToBeWiped(): begin");
    TLawMoStatus lawmostat(ELawMoSuccess);
    TUid impluid = GetAdapterUidL(item, ETrue);
    RLDEBUG_2("CDCMOServer::SetToBeWiped for uid(): %d", impluid);
    TInt afind = ilawmoPluginUidToBeWiped.Find(impluid);
    // Add Node's corresponding plugin uid to list so that it can be wiped
    if(impluid.iUid)
        {
        if(wipeValue)
            {
            RLDEBUG("CDCMOServer::SetToBeWiped(): wipeVal true");
            if(afind==KErrNotFound)
                ilawmoPluginUidToBeWiped.Append(impluid);
            else
                RLDEBUG("CDCMOServer::SetToBeWiped() tobewiped already set");               
            }
        else
            {
            RLDEBUG("CDCMOServer::SetToBeWiped(): wipeVal false");        
            if(afind!=KErrNotFound)
                {
                ilawmoPluginUidToBeWiped.Remove(afind);
                RLDEBUG("CDCMOServer::SetToBeWiped() tobewiped unset");
                }
            else
                RLDEBUG("CDCMOServer::SetToBeWiped() tobewiped notset atall");                
            }
        }
    else
        {
        RLDEBUG("CDCMOServer::SetToBeWiped(): no such plugin found");
        lawmostat = ELawMoFail;
        }
    RLDEBUG("CDCMOServer::SetToBeWiped(): End");
    return lawmostat;
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

void CDCMOServer::HandleWipeCompleted(TInt status)
{
    RLDEBUG("CDCMOServer::HandleWipeCompleted(): begin");
    // whether wipe is performed or failed
    iwipeStatus = (iwipeStatus && status);
    RLDEBUG_2("CDCMOServer::HandleWipeCompleted wipestate: %d",iwipeStatus);
    iCount++;
    if(ilawmoPluginUidToBeWiped.Count()>iCount)
        {
        RLDEBUG("CDCMOServer::HandleWipeCompleted(): create obj");
        CLAWMOPluginInterface* obj;
        TRAPD(err, obj = CLAWMOPluginInterface::NewL(ilawmoPluginUidToBeWiped[iCount], this));
        if(err == KErrNone)
             {
             RLDEBUG("CDCMOServer::HandleWipeCompleted(): obj created");
             RLDEBUG_2("CDCMOServer::HandleWipeCompleted plugin count: %d",ilawmoPlugins.Count());
            TRAP(err,obj->WipeL());
        RLDEBUG_2("CDCMOServer::HandleWipeCompleted(): wipe called %d",err);
        ilawmoPlugins.Append(obj);
            }
        if(err!=KErrNone)
            HandleWipeCompleted(KErrGeneral);
        }
    else
        {
        // start DM session using the Monitor for generic alert
        RLDEBUG("CDCMOServer::HandleWipeCompleted(): done");
        TInt err;
        TBuf<KBufferSize> srvrid; 
        CRepository* crep;
        TRAP(err, crep = CRepository::NewLC( KCRUidDeviceManagementInternalKeys );
                         CleanupStack::Pop(crep));
        RLDEBUG("CDCMOServer::HandleWipeCompleted(): get srvrid");
        if(err == KErrNone)
            err = crep->Get( KLAWMOfactoryDmServerID, srvrid );
        RDEBUG_2("CDCMOServer::GetCurrentServerId() %d",err);
        RLDEBUG_2("CDCMOServer::HandleWipeCompleted(): srvrid %s",srvrid.PtrZ());
        if (err == KErrNone)
            {
            RLDEBUG("CDCMOServer::startNwMonitor()");
            TRAP(err,StartDMNetworkMonitorL(srvrid, iSessionIap));
            }
        
        if(crep)
			{
            delete crep; //crep
			crep = NULL;
			}
        
        // Only when session is started successfully, Set wipestatus
        if(err==KErrNone)
            {
            RLDEBUG("CDCMOServer::HandleWipeCompleted(): writing wipestatus to cenrep");
            CRepository* repository;
            TRAP(err, repository = CRepository::NewLC ( KCRUidLawmoAdapter );
                      CleanupStack::Pop(crep));
            
            if(iwipeStatus==KErrNone)
            iwipeStatus = KWipeSuccess;
            else
            iwipeStatus = KWipeFailure;
            
            repository->Set(KLawmoWipeStatus,iwipeStatus);
            RLDEBUG_2("CDCMOServer::HandleWipeCompleted wipestate: %d",iwipeStatus);
            if (repository)
				{
	            delete repository;
				repository = NULL;
				}
            }
        
        RLDEBUG_2("printing ilawmoPluginUidToBeWiped %d", ilawmoPluginUidToBeWiped.Count());
        RLDEBUG_2("printing ilawmoPlugins %d", ilawmoPlugins.Count());
        RLDEBUG_2("printing ilawmoPlugins %d", iCount);
        iCount = -1; // To indicate all wipe is done
        if( iSessionCount == 0)
           {
            CActiveScheduler::Stop();
            RLDEBUG("CDCMOServer::HandleWipeCompleted(): kill server");
            }        
        }    
    
    RLDEBUG("CDCMOServer::HandleWipeCompleted(): end");
}

// ------------------------------------------------------------------------------------------------
// CDCMOServer::StartDMNetworkMonitorL()
// ------------------------------------------------------------------------------------------------
void CDCMOServer::StartDMNetworkMonitorL(TDesC& aServerId, TInt iapid)
{
    TInt retryenabled = 1;
    _LIT( KNetMon,"\\dmnetworkmon.exe" );
    RLDEBUG("CDCMOServer::StartDMNetworkMonitorL(): start");
    TBuf8<KBufferSize> serverid;
    serverid.Copy(aServerId);
    // Enable DM Network Monitoring for retry of Generic alert in case of N/W loss
    
    CRepository *repository= CRepository::NewLC ( KCRUidDeviceManagementInternalKeys );
    repository->Set(KDevManEnableDMNetworkMon, retryenabled);
    repository->Set(KDevManServerIdKey, serverid);
    repository->Set(KDevManIapIdKey, iapid);
    RLDEBUG("CDCMOServer::StartDMNetworkMonitorL(): set rep keys");
    CleanupStack::PopAndDestroy();
    
    // create NetMon EXE
    RProcess rp;
    TInt err = rp.Create(KNetMon,KNullDesC);
    RLDEBUG("CDCMOServer::StartDMNetworkMonitorL():create rprocess");
    User::LeaveIfError(err);
    TRequestStatus stat;
    rp.Rendezvous(stat);
    RLDEBUG("CDCMOServer::StartDMNetworkMonitorL():rendezvous");
        
    if (stat!=KRequestPending)
        {
        RLDEBUG("CDCMOServer::StartDMNetworkMonitorL():abort srvr");
        rp.Kill(0);     // abort startup
        }
    else
        {
        RLDEBUG("CDCMOServer::StartDMNetworkMonitorL(): start server");
        rp.Resume();    // logon OK - start the server
        }
    User::WaitForRequest(stat);     // wait for start or death
    TInt r= rp.ExitType();
    TInt reqstat  = stat.Int();
    //TExitType a;
    RLDEBUG_2("CDCMOServer::StartDMNetworkMonitorL() exittype %d",r);
    RLDEBUG_2("CDCMOServer::StartDMNetworkMonitorL() reqstatus %d",reqstat);

    rp.Close();
}


