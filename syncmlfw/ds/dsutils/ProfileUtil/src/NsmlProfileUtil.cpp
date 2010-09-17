/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used in Auto-Restart to read/write profile to cenrep, read from
*                settings database etc.
*
*/


// USER INCLUDES
#include "NsmlProfileUtil.h"
#include <nsmldebug.h>
		
#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

// CONSTANTS
const TInt KNsmlDsAutoStartProfileKey = 2; // 0x00000002 defined in DS cenrep
const TInt KNsmlDsIapId 			  = 5; // 0x00000005 defined in DS cenrep
const TInt KNsmlDsAutoStartSession    = 6; // 0x00000006 defined in DS cenrep
const TInt KNsmlDsAutoStartTaskIdKey  = 9; // 0x00000009 defined in DS cenrep
const TInt KNsmlMaxCenrepBufferSize   = 1024;
const TUid KRepositoryId              = { 0x2000CF7E };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CNsmlProfileUtil* CNsmlProfileUtil::NewLC()
{
    DBG_FILE(_S8("CNsmlProfileUtil::NewLC, BEGIN"));
    CNsmlProfileUtil* self=new (ELeave) CNsmlProfileUtil();
	CleanupStack::PushL(self);
	self->ConstructL();
	DBG_FILE(_S8("CNsmlProfileUtil::NewLC, END"));
	return self;
}

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CNsmlProfileUtil* CNsmlProfileUtil::NewL()
{
    DBG_FILE(_S8("CNsmlProfileUtil::NewL, BEGIN"));
	CNsmlProfileUtil* self=CNsmlProfileUtil::NewLC();
	CleanupStack::Pop();
	DBG_FILE(_S8("CNsmlProfileUtil::NewL, END"));
	return self;
}
	
// -----------------------------------------------------------------------------
// CNsmlProfileUtil::~CNsmlProfileUtil
// Destructor.
// -----------------------------------------------------------------------------
CNsmlProfileUtil::~CNsmlProfileUtil() 
{

    DBG_FILE(_S8("CNsmlProfileUtil::~CNsmlProfileUtil, BEGIN"));
    // Free up buffers
    if ( iDisplayName ) 
    {
    	delete iDisplayName;
    }
    
    if ( iServerId ) 
    {
    	delete iServerId;
    }
    
    if ( iServerURI ) 
    {
    	delete iServerURI;
    }
    
    if ( iSyncMLUserName ) 
    {    
    	delete iSyncMLUserName;
    }
    
    if ( iSyncMLPassword ) 
    {
    	delete iSyncMLPassword;
    }
    
    if ( iSyncHTTPAuthUserName ) 
    {
    	delete iSyncHTTPAuthUserName;
    }
    
    if ( iSyncHTTPAuthPassword ) 
    {
    	delete iSyncHTTPAuthPassword;
    }

    // Free up content objects
    iContents.ResetAndDestroy();
    
    DBG_FILE(_S8("CNsmlProfileUtil::~CNsmlProfileUtil, END"));
}

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::ConstructL
// Second-phase constructor.
// -----------------------------------------------------------------------------
//
void CNsmlProfileUtil::ConstructL() 
{      
    // Nothing to do
}

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::WriteToStreamL
// 
// -----------------------------------------------------------------------------
void CNsmlProfileUtil::WriteToStreamL(RDesWriteStream& aStream, const TDesC& aString)
{
    DBG_FILE(_S8("CNsmlProfileUtil::WriteToStreamL, BEGIN"));
    TInt len = aString.Length();
	// Write string length
    aStream.WriteInt32L(len);   
   // Write content 
    if ( len == 0 )
    {
    	aStream << KNullDesC8;
    }
    else 
    {
    	aStream << aString;
    }
    DBG_FILE(_S8("CNsmlProfileUtil::WriteToStreamL, END"));
    
}

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::InternalizeFromProfileL
// 
// -----------------------------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::InternalizeFromSettingsDBL(TInt aProfileId)
{

    DBG_FILE(_S8("CNsmlProfileUtil::InternalizeFromSettingsDBL, BEGIN"));
    // Read the profile details from settings database
	CNSmlDSSettings* dsSettings = CNSmlDSSettings::NewL();	       
	CNSmlDSProfile* profile = dsSettings->ProfileL( aProfileId );
	
	iProfileId = aProfileId;	
	iDisplayName = 	profile->StrValue( EDSProfileDisplayName ).AllocL();
	iProtocolVer = static_cast<TSmlProtocolVersion>( profile->IntValue( EDSProfileProtocolVersion ) );	
	iTransportId=static_cast<TSmlTransportId>( profile->IntValue( EDSProfileTransportId ) );
	iIapId=profile->IntValue(EDSProfileIAPId);
	iSanRequest=static_cast<TSmlServerAlertedAction>(profile->IntValue(EDSProfileServerAlertedAction));
	iServerId = profile->StrValue( EDSProfileServerId ).AllocL();	
	iSyncMLUserName = profile->StrValue( EDSProfileSyncServerUsername ).AllocL();	
	iSyncMLPassword = profile->StrValue( EDSProfileSyncServerPassword ).AllocL();	
	iServerURI = profile->StrValue( EDSProfileServerURL ).AllocL();	
	iSyncHTTPAuthUserName = profile->StrValue( EDSProfileHttpAuthUsername ).AllocL();		
	iSyncHTTPAuthPassword = profile->StrValue( EDSProfileHttpAuthPassword ).AllocL();


	// Read content types
	for ( int i = 0; i < profile->iContentTypes->Count(); i++ ) 
	{
	    CNsmlContentData* contentData = CNsmlContentData::NewL();
	    
	    // From settings DB
	    CNSmlDSContentType* contentType = static_cast<CNSmlDSContentType*>(profile->iContentTypes->At(i));
	   
	    // Fill the local content data 			
	    contentData->SetServerDataSourceL(contentType->StrValue(EDSAdapterServerDataSource));		    
	    contentData->SetImplementationId(contentType->IntValue(EDSAdapterImplementationId));	    
	    contentData->SetContentEnabled(contentType->IntValue(EDSAdapterEnabled));
	    
	    //SyncType is same for all the contents from UI
	    iSyncType=static_cast<TSmlSyncType>(contentType->IntValue(EDSAdapterSyncType));
	    // Add to this object
	    iContents.AppendL(contentData);			
	}
	
	// Cleanup
	delete profile;
	delete dsSettings;
	DBG_FILE(_S8("CNsmlProfileUtil::InternalizeFromSettingsDBL, END"));
}

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::InternalizeFromCenrepL
// 
// -----------------------------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::InternalizeFromCenrepL()
{
    DBG_FILE(_S8("CNsmlProfileUtil::InternalizeFromCenrepL, BEGIN"));
	// Create a temporary buffer
    HBufC8*  buffer = HBufC8::NewLC(KNsmlMaxCenrepBufferSize);
    TPtr8 ptr = buffer->Des();
    
    // Read from cenrep
    CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Get(KNsmlDsAutoStartProfileKey, ptr);
    User::LeaveIfError(err);    
    CleanupStack::PopAndDestroy(repository);
    
    // Open a Read stream  
    RDesReadStream stream(ptr);
    stream.PushL();
    
    //Read the size of the stream
    TInt streamsize = ptr.Size();
    DBG_FILE_CODE(streamsize,_S8("CNsmlProfileUtil::InternalizeFromCenrepL, The Size of Stream is"));
    //BPSS-7GBFF3
    if(streamsize <= 2)
    {
    	iProfileId = -99;
    }
    else
    {
	    iProfileId = stream.ReadInt32L();
	    
	    iProtocolVer = static_cast<TSmlProtocolVersion>( stream.ReadInt32L() );
	    
	    iTransportId=static_cast<TSmlTransportId>( stream.ReadInt32L() );
	    
	    iIapId=stream.ReadInt32L();
	    
	    iSanRequest=static_cast<TSmlServerAlertedAction>( stream.ReadInt32L() );
	    
	    TInt size = stream.ReadInt32L();
	    iDisplayName = HBufC::NewL(stream, size);
	    
	    size = stream.ReadInt32L();
		iServerId = HBufC::NewL(stream, size);
		
		size = stream.ReadInt32L();
		iSyncMLUserName = HBufC::NewL(stream, size);
		
		size = stream.ReadInt32L();
		iSyncMLPassword = HBufC::NewL(stream, size);
		
		size = stream.ReadInt32L();
		iServerURI = HBufC::NewL(stream, size);
		
		size = stream.ReadInt32L();
		iSyncHTTPAuthUserName = HBufC::NewL(stream, size);
		
		size = stream.ReadInt32L();
		iSyncHTTPAuthPassword = HBufC::NewL(stream, size);
		    
	    // Read the content types
	    TInt numContentTypes = stream.ReadInt32L();
	    for ( int i = 0; i < numContentTypes; i++ )
	    {
	        // Create a new content object and fill contents
	        CNsmlContentData* data = CNsmlContentData::NewL();
	        
	        data->SetImplementationId(stream.ReadInt32L());
	        
	        size = stream.ReadInt32L();
	        HBufC* source = HBufC::NewL(stream, size);
	        data->SetServerDataSourceL(*source); 
	        delete source; // Memory allocate in SetServerDataSource 
	        
	        data->SetContentEnabled(stream.ReadInt32L()); 	
	        
	        iContents.AppendL(data);
	    }
	    
	    iSyncType=static_cast<TSmlSyncType>(stream.ReadInt32L());
    }
      
    // Close
    stream.Close();      
    CleanupStack::PopAndDestroy(2, buffer);
    
    DBG_FILE(_S8("CNsmlProfileUtil::InternalizeFromCenrepL, END"));
}

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::ExternalizeToCenrepL
// 
// -----------------------------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::ExternalizeToCenrepL()
{
   DBG_FILE(_S8("CNsmlProfileUtil::ExternalizeToCenrepL, BEGIN"));
	// Create a temporary buffer
    HBufC8*  buffer = HBufC8::NewLC(KNsmlMaxCenrepBufferSize);
    TPtr8 ptr = buffer->Des();
    
    //Opens the Write stream  
    RDesWriteStream stream(ptr);
    stream.PushL();
    
    stream.WriteInt32L(iProfileId);
    stream.WriteInt32L(iProtocolVer);
    stream.WriteInt32L(iTransportId);
    stream.WriteInt32L(iIapId);
    stream.WriteInt32L(iSanRequest);
    
    WriteToStreamL(stream, *iDisplayName);
    WriteToStreamL(stream, *iServerId);
    WriteToStreamL(stream, *iSyncMLUserName);
    WriteToStreamL(stream, *iSyncMLPassword);
    WriteToStreamL(stream, *iServerURI);
    WriteToStreamL(stream, *iSyncHTTPAuthUserName);
    WriteToStreamL(stream, *iSyncHTTPAuthPassword);    
    
    // Write the content types
    stream.WriteInt32L(iContents.Count());
    for ( int i = 0; i < iContents.Count(); i++ )
    {
    	stream.WriteInt32L(iContents[i]->ImplementationId());
    	WriteToStreamL(stream, *(iContents[i]->ServerDataSource()));
    	stream.WriteInt32L(iContents[i]->ContentEnabled());
    }
    
    stream.WriteInt32L(iSyncType);
    stream.CommitL();
    stream.Close();
    CleanupStack::PopAndDestroy();
    
	// Write to cenrep
    CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Set(KNsmlDsAutoStartProfileKey, ptr);
    User::LeaveIfError(err);

    // Cleanup    
    CleanupStack::PopAndDestroy(2, buffer);
    
     DBG_FILE(_S8("CNsmlProfileUtil::ExternalizeToCenrepL, END"));
}


// -----------------------------------------------------------------------------
// CNsmlProfileUtil::ProfileId
//
// -----------------------------------------------------------------------------
EXPORT_C TSmlProfileId CNsmlProfileUtil::ProfileId()
{
	return iProfileId;
}

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::CompareIntValues
//
// -----------------------------------------------------------------------------

TBool CNsmlProfileUtil::CompareIntValues(TInt aNewValue,TInt aCurrentValue)
{
	if(aNewValue==aCurrentValue)
	   return ETrue;
	else
	   return EFalse;
} 

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::CompareStringValues
//
// -----------------------------------------------------------------------------

TBool CNsmlProfileUtil::CompareStringValues(TPtr16 aNewValue, TPtr16 aCurrentValue)
{

	if(aNewValue.Compare(aCurrentValue))
	  return EFalse;
	else
	  return ETrue;
}

// -----------------------------------------------------------------------------
// CNsmlProfileUtil::ProfileId
//
// -----------------------------------------------------------------------------
EXPORT_C TBool CNsmlProfileUtil::IsSame(CNsmlProfileUtil& aNewProfile)
{
    
    DBG_FILE(_S8("CNsmlProfileUtil::IsSame, BEGIN"));
    RArray<TBool> array;
    TBool chk;
    TInt i=0;
    if(aNewProfile.iProfileId==this->iProfileId)
    {
          //Do the comparision for all the fields stored in the Cenrep and the settings DB
          
          chk=CompareStringValues(aNewProfile.iDisplayName->Des(),iDisplayName->Des());
          array.Insert(chk,i++);
	      chk =CompareIntValues(aNewProfile.iProtocolVer,iProtocolVer);
	      array.Insert(chk,i++);
	      chk= CompareIntValues(aNewProfile.iTransportId,iTransportId);
	      array.Insert(chk,i++);
	      chk=CompareIntValues(aNewProfile.iIapId,this->iIapId);
	      DBG_FILE_CODE(aNewProfile.iIapId , _S8("CNsmlProfileUtil::IsSame, The New Profile Accessss Point ID is:"));                
	      DBG_FILE_CODE(this->iIapId , _S8("CNsmlProfileUtil::IsSame, The Last used Accessss Point ID is:"));                
	      if(!chk)
	      {
	      	TInt accesspointId;
	      	TInt err(KErrNone);
	      	TRAP(err, AccessPointIdL(accesspointId));
	      	DBG_FILE_CODE(accesspointId , _S8("CNsmlProfileUtil::IsSame, The Access Point selected by the user is:"));                
	      	chk=CompareIntValues(aNewProfile.iIapId,accesspointId);
	      }
	      array.Insert(chk,i++);
	      chk=CompareStringValues(aNewProfile.iServerURI->Des(),iServerURI->Des());
	      array.Insert(chk,i++);
	      chk =CompareIntValues(aNewProfile.iServerPort,iServerPort);
	      array.Insert(chk,i++);
	      chk=CompareStringValues(aNewProfile.iSyncMLUserName->Des(),iSyncMLUserName->Des()); 
	      array.Insert(chk,i++);
	      chk=CompareStringValues(aNewProfile.iSyncMLPassword->Des(),iSyncMLPassword->Des()); 
	      array.Insert(chk,i++);
	      chk=CompareIntValues(aNewProfile.iSanRequest,iSanRequest);
	      array.Insert(chk,i++);
	      chk=CompareStringValues(aNewProfile.iSyncHTTPAuthUserName->Des(),iSyncHTTPAuthUserName->Des()); 
	      array.Insert(chk,i++);
	      chk=CompareStringValues(aNewProfile.iSyncHTTPAuthPassword->Des(),iSyncHTTPAuthPassword->Des());
	      array.Insert(chk,i++); 
	      chk=CompareIntValues(aNewProfile.iSyncType,iSyncType);
	      array.Insert(chk,i++);
	    
	      //loop through all the contents for current profile
	      if(iContents.Count() == aNewProfile.iContents.Count())
	      {
	        for(TInt k=0;k<this->iContents.Count();k++)
	        {
	           chk=EFalse;
	      	    for(TInt p=0;p< aNewProfile.iContents.Count(); p++)
	      	    {
	      	      if(iContents[k]->ImplementationId()==aNewProfile.iContents[p]->ImplementationId()&& 
	      	              iContents[k]->ContentEnabled()==aNewProfile.iContents[p]->ContentEnabled())
	      	        {
	      	             HBufC *buff1=iContents[k]->ServerDataSource();
	      	             HBufC *buff2=aNewProfile.iContents[p]->ServerDataSource();
	      	             chk=CompareStringValues(buff1->Des(),buff2->Des());
	      	   	         array.Insert(chk,i++);
	      	   	         break;
	      	        }
	      	     
	      	     }
	      	    if(!chk)
	      	       {
	      	    	 array.Insert(chk,i++);
	      	    	 break;
	      	       }
	         }
	      }
	      else
	      {
	      	chk=EFalse;
	      	array.Insert(chk,i++);
	      }
   
      }
     
    for(TInt j=0;j<array.Count();j++)
    {
    	if(array[j]==EFalse)
    	{
    	    return EFalse;
    	}
    	    
    	
    }
    array.Close();
    DBG_FILE(_S8("CNsmlProfileUtil::IsSame, END"));
    return(ETrue);
    
   
}


// -----------------------------------------------------------------------------
// CNsmlContentData::NewLC
//
// -----------------------------------------------------------------------------
EXPORT_C CNsmlContentData* CNsmlContentData::NewLC() 
{
    DBG_FILE(_S8("CNsmlContentData::NewLC, BEGIN"));
    CNsmlContentData* self=new (ELeave) CNsmlContentData();
    CleanupStack::PushL(self);
    self->ConstructL();
    DBG_FILE(_S8("CNsmlContentData::NewLC, END"));
    return self;
}
       
// -----------------------------------------------------------------------------
// CNsmlContentData::NewL
//
// -----------------------------------------------------------------------------
EXPORT_C CNsmlContentData* CNsmlContentData::NewL() 
{   
    DBG_FILE(_S8("CNsmlContentData::NewL, BEGIN"));
    CNsmlContentData* self=CNsmlContentData::NewLC();
    CleanupStack::Pop();
    DBG_FILE(_S8("CNsmlContentData::NewL, END"));
    return self;
}

// -----------------------------------------------------------------------------
// CNsmlContentData::ConstructL
//
// -----------------------------------------------------------------------------
void CNsmlContentData::ConstructL() 
{
  //nothing to do
}

// -----------------------------------------------------------------------------
// CNsmlContentData::~CNsmlContentData
//
// -----------------------------------------------------------------------------
CNsmlContentData::~CNsmlContentData() 
{
    if ( iServerDataSource != NULL ) {
		delete iServerDataSource;
    }   
}       
       
// -----------------------------------------------------------------------------
// CNsmlContentData::ServerDataSource
//
// -----------------------------------------------------------------------------
EXPORT_C HBufC* CNsmlContentData::ServerDataSource() 
{
    return iServerDataSource;
}
       
// -----------------------------------------------------------------------------
// CNsmlContentData::SetServerDataSource
//
// -----------------------------------------------------------------------------
EXPORT_C void CNsmlContentData::SetServerDataSourceL(const TDesC& aServerDataSource)
{
    iServerDataSource = aServerDataSource.AllocL();
}
       
// -----------------------------------------------------------------------------
// CNsmlContentData::ImplementationId
//
// -----------------------------------------------------------------------------
EXPORT_C TInt CNsmlContentData::ImplementationId() 
{
    return iImplementationId;
}
       
// -----------------------------------------------------------------------------
// CNsmlContentData::SetImplementationId
//
// -----------------------------------------------------------------------------
EXPORT_C void CNsmlContentData::SetImplementationId(const TInt aImplementationId)
{
    iImplementationId = aImplementationId;
}
   
// -----------------------------------------------------------------------------
// CNsmlContentData::SetContentEnabled
//
// -----------------------------------------------------------------------------
EXPORT_C void CNsmlContentData::SetContentEnabled(const TInt aContentEnabled) 
{
    iEnabled = aContentEnabled;
}    
 
// -----------------------------------------------------------------------------
// CNsmlContentData::ContentEnabled
//
// -----------------------------------------------------------------------------
EXPORT_C TInt CNsmlContentData::ContentEnabled() 
{
    return iEnabled;
}

// ---------------------------------------------------------
// CNsmlProfileUtil::WriteInterruptFlagL()
// Write the Interrupt Flag in the CenRep
// ---------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::WriteInterruptFlagL(TInt aFlag)
{
	DBG_FILE(_S8("CNsmlProfileUtil::WriteInterruptFlagL() begins"));
   
   	// Write to cenrep
	CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Set(KNsmlDsAutoStartSession, aFlag);
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(repository);
    
    DBG_FILE(_S8("CNsmlProfileUtil::WriteInterruptFlagL() ends"));
}

// ---------------------------------------------------------
// CNsmlProfileUtil::IsValidResumeL()
// Gets Interrupt Flag from the CenRep
// ---------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::IsValidResumeL(TBool& aValue)
{
	DBG_FILE(_S8("CNsmlProfileUtil::IsValidResumeL() begins"));
   	
   	TInt IsInterrupt = 0;
   
    // Read from cenrep
    CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Get(KNsmlDsAutoStartSession, IsInterrupt);
    User::LeaveIfError(err);    
    CleanupStack::PopAndDestroy(repository);
    
     
    if(IsInterrupt == -1)
    {
    	aValue = ETrue;    	
    }    	
    else
    {
    	aValue = EFalse;
    }
    
    DBG_FILE(_S8("CNsmlProfileUtil::IsValidResumeL() ends"));
}

// ---------------------------------------------------------
// CNsmlContentData::WriteAccessPointIdL()
// Writes the AccessPointID in to the CenRep
// ---------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::WriteAccessPointIdL(TInt aIAPId)
{
	DBG_FILE(_S8("CNsmlProfileUtil::WriteAccessPointIdL() begins"));
   
   	// Write to cenrep
	CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Set(KNsmlDsIapId, aIAPId);
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy(repository);
    
    DBG_FILE(_S8("CNsmlProfileUtil::WriteAccessPointIdL() ends"));
}

// ---------------------------------------------------------
// CNsmlProfileUtil::AccessPointIdL()
// Return the AccessPointId
// ---------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::AccessPointIdL(TInt& aIAPId)
{
	DBG_FILE(_S8("CNsmlProfileUtil::AccessPointIdL() begins"));
   
    // Read from cenrep
    CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Get(KNsmlDsIapId, aIAPId);
    User::LeaveIfError(err);    
    CleanupStack::PopAndDestroy(repository);
    
    DBG_FILE(_S8("CNsmlProfileUtil::AccessPointIdL() ends"));
}

// ---------------------------------------------------------
// CNsmlProfileUtil::InternalizeTaskIdL()
// Read the TaskIds' from the Cenrep
// ---------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::InternalizeTaskIdL(RArray<TSmlTaskId>& aTaskIdArray)
{
	DBG_FILE(_S8("CNsmlProfileUtil::InternalizeTaskIdL, BEGIN"));
	
	aTaskIdArray.Reset();
	
	// Create a temporary buffer
    HBufC8*  buffer = HBufC8::NewLC(KNsmlMaxCenrepBufferSize);
    TPtr8 ptr = buffer->Des();
    
    // Read from cenrep
    CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Get(KNsmlDsAutoStartTaskIdKey, ptr);
    User::LeaveIfError(err);    
    CleanupStack::PopAndDestroy(repository);
    
    // Open a Read stream  
    RDesReadStream stream(ptr);
    stream.PushL();
    
    // Read the Task Id
    DBG_FILE(_S8("CNsmlProfileUtil::InternalizeTaskIdL, Reading the TaskID"));
    TInt numTaskId = stream.ReadInt32L();
    for ( TInt i = 0; i < numTaskId; i++ )
    {
        aTaskIdArray.Append(static_cast<TSmlTaskId>(stream.ReadInt32L())); 	
    }
          
    // Close
    stream.Close();      
    CleanupStack::PopAndDestroy(2, buffer);
    
    DBG_FILE(_S8("CNsmlProfileUtil::InternalizeTaskIdL, END"));
    
}

// ---------------------------------------------------------
// CNsmlProfileUtil::ExternalizeTaskIdL()
// Write the TaskIds' from the Cenrep
// ---------------------------------------------------------
EXPORT_C void CNsmlProfileUtil::ExternalizeTaskIdL(RArray<TSmlTaskId> aTaskIdArray)
{
	DBG_FILE(_S8("CNsmlProfileUtil::ExternalizeTaskIdL, BEGIN"));
	
	// Create a temporary buffer
    HBufC8*  buffer = HBufC8::NewLC(KNsmlMaxCenrepBufferSize);
    TPtr8 ptr = buffer->Des();
    
    //Opens the Write stream  
    RDesWriteStream stream(ptr);
    stream.PushL();
    
    // Write the content types
    stream.WriteInt32L(aTaskIdArray.Count());
    for ( TInt i = 0; i < aTaskIdArray.Count(); i++ )
    {
    	stream.WriteInt32L(aTaskIdArray[i]);
    }
    
    stream.CommitL();
    stream.Close();
    
    CleanupStack::PopAndDestroy();
    
	// Write to cenrep
	DBG_FILE(_S8("CNsmlProfileUtil::ExternalizeTaskIdL, Writting the TaskID"));
    CRepository* repository = CRepository::NewLC(KRepositoryId);
    TInt err = repository->Set(KNsmlDsAutoStartTaskIdKey, ptr);
    User::LeaveIfError(err);
    
    // Cleanup    
    CleanupStack::PopAndDestroy(2, buffer);
    
    DBG_FILE(_S8("CNsmlProfileUtil::ExternalizeTaskIdL, END"));	
}


//End of file

