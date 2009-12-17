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
* Description:  Symbian OS Server source.
*
*/


#include <ecom.h>
#include <implementationinformation.h>

#include "nsmlsosserver.h"

// --------------------------------------------------------------------------
// CNSmlProfile::CNSmlProfile( TSmlUsageType aUsageType )
// --------------------------------------------------------------------------
//
CNSmlProfile::CNSmlProfile( TSmlUsageType aUsageType, CNSmlSOSSession& aSession ) 
: iProfId(KNSmlNullId), iUsageType(aUsageType), iSession(aSession), iLocked(EFalse), 
iDataPtr(NULL, 0, 0) 
    {
    }

// --------------------------------------------------------------------------
// CNSmlProfile::~CNSmlProfile()
// --------------------------------------------------------------------------
//
CNSmlProfile::~CNSmlProfile()
    {
    delete iBuffer;
    }

// --------------------------------------------------------------------------
// TInt CNSmlProfile::ProfileId() const
// --------------------------------------------------------------------------
//
TInt CNSmlProfile::ProfileId() const
    {
    return iProfId;
    }

// --------------------------------------------------------------------------
// void CNSmlProfile::SetLocked()
// --------------------------------------------------------------------------
//
void CNSmlProfile::SetLocked()
    {
    iLocked = ETrue;
    }

// --------------------------------------------------------------------------
// TBool CNSmlProfile::IsLocked()
// --------------------------------------------------------------------------
//
TBool CNSmlProfile::IsLocked()
    {
    return iLocked;
    }

// --------------------------------------------------------------------------
// void CNSmlProfile::SetCreatorId( const TInt aCreatorId )
// --------------------------------------------------------------------------
//
void CNSmlProfile::SetCreatorId( const TInt aCreatorId )
    {
    iCreatorId = aCreatorId;
    }

// --------------------------------------------------------------------------
// TInt CNSmlProfile::FetchDataL( const TInt aId, TBool aAllowHidden )
// --------------------------------------------------------------------------
//
TInt CNSmlProfile::FetchDataL( const TInt aId, TBool aAllowHidden )
    {
    iProfId = aId;
    TInt ret(KErrNotFound);
    // Get profile data and pack it into aData.
    switch ( iUsageType )
        {
        case ESmlDataSync:
            {
            CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );

            if ( prof )
                {
                CleanupStack::PushL(prof);
                
                // check that hidden profiles are allowed
                if ( !aAllowHidden && prof->IntValue( EDSProfileHidden) )
                	{
                	ret = KErrAccessDenied;
                	}
                else
                	{
	                TInt namelen = prof->StrValue( EDSProfileDisplayName ).Size();
	                TInt unlen = prof->StrValue( EDSProfileSyncServerUsername ).Size();
	                TInt pwlen = prof->StrValue( EDSProfileSyncServerPassword ).Size();
	                TInt sidlen = prof->StrValue( EDSProfileServerId ).Size();
	                    
	                CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
	                CleanupStack::PushL( buffer );
	                RBufWriteStream stream( *buffer );  
	                CleanupClosePushL(stream);
	                
	                stream.WriteInt32L( namelen );
	                stream << prof->StrValue( EDSProfileDisplayName );

	                stream.WriteInt32L( unlen );
	                stream << prof->StrValue( EDSProfileSyncServerUsername );

	                stream.WriteInt32L( pwlen );
	                stream << prof->StrValue( EDSProfileSyncServerPassword );

	                stream.WriteInt32L( sidlen );
	                stream << prof->StrValue( EDSProfileServerId );

	                stream.WriteInt8L( prof->IntValue( EDSProfileServerAlertedAction ) );   
	                stream.WriteInt32L( prof->IntValue( EDSProfileCreatorId ) );   

	                stream.WriteInt8L( prof->IntValue( EDSProfileDeleteAllowed ) );
	                stream.WriteInt8L( prof->IntValue( EDSProfileProtocolVersion ) );

	            	stream.CommitL();
	                
	                CleanupStack::PopAndDestroy(&stream);    
	                
	                delete iBuffer;
	                iBuffer = buffer;
	                CleanupStack::Pop( buffer );   
	                ret = KErrNone;
                	}
                	
	            CleanupStack::PopAndDestroy(prof); 
                }
            break;
            }
        case ESmlDevMan:
            {
            CNSmlDMProfile* prof = iSession.DMSettings().ProfileL( iProfId );
            if ( prof )
                {
                CleanupStack::PushL(prof);

                if ( !aAllowHidden && prof->IntValue( EDMProfileHidden) )
                	{
                	ret = KErrAccessDenied;
                	}
                else
	                {
                    TInt namelen = prof->StrValue( EDMProfileDisplayName ).Size();
                    TInt sunlen = prof->StrValue( EDMProfileServerUsername ).Size();
                    TInt spwlen = prof->StrValue( EDMProfileServerPassword ).Size(); 
                    TInt sidlen = prof->StrValue( EDMProfileServerId ).Size();       
                    TInt pwlen = prof->StrValue( EDMProfileClientPassword ).Size();
                   
                    CBufFlat* buffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );
                    CleanupStack::PushL( buffer );
                    RBufWriteStream stream( *buffer );  
                    CleanupClosePushL(stream);
                    
                    stream.WriteInt32L( namelen );
                    stream << prof->StrValue( EDMProfileDisplayName );

                    stream.WriteInt32L( sunlen );
                    stream << prof->StrValue( EDMProfileServerUsername );

                    stream.WriteInt32L( spwlen );
                    stream << prof->StrValue( EDMProfileServerPassword );

                    stream.WriteInt32L( sidlen );
                    stream << prof->StrValue( EDMProfileServerId );

                    stream.WriteInt8L( prof->IntValue( EDMProfileServerAlertAction ) );
                    stream.WriteInt32L( prof->IntValue( EDMProfileCreatorId ) );  
                    stream.WriteInt8L( prof->IntValue( EDMProfileDeleteAllowed ) );
                    stream.WriteInt8L( prof->IntValue( EDMProfileLock ) );
                    stream.WriteInt32L( pwlen );
                    stream << prof->StrValue( EDMProfileClientPassword );

                	stream.CommitL();
                    
                    CleanupStack::PopAndDestroy(&stream); 
                    
                    delete iBuffer;
                    iBuffer = buffer;
                    CleanupStack::Pop( buffer );    
                    
                    ret = KErrNone;
	                }
                CleanupStack::PopAndDestroy(prof); 
                }
            break;
            }
        default:
            break;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// void CNSmlProfile::ConnectionListL( RArray<TInt>& aArray )
// --------------------------------------------------------------------------
//
void CNSmlProfile::ConnectionListL( RArray<TInt>& aArray )
    {
    if ( iProfId < KMaxDataSyncID ) // DS profile
        {
        CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
        if ( prof )
            {
            // Currently there's only one connection per profile.
            aArray.AppendL( prof->IntValue( EDSProfileTransportId ) );
            delete prof;
            prof = NULL;
            }
        }
    else    // DM profile
       {
        CNSmlDMProfile* prof = iSession.DMSettings().ProfileL( iProfId );
        if ( prof )
            {
            aArray.AppendL( prof->IntValue( EDMProfileTransportId ) );
            delete prof;
            prof = NULL;
            }
       }            
    }

// --------------------------------------------------------------------------
// void CNSmlProfile::TaskListL( RArray<TInt>& aArray )
// --------------------------------------------------------------------------
//
void CNSmlProfile::TaskListL( RArray<TInt>& aArray )
    {
    CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
    if ( prof )
        {
        CleanupStack::PushL(prof);

        RImplInfoPtrArray implArray;
        CleanupStack::PushL(PtrArrCleanupItemRArr( CImplementationInformation, &implArray ) );    
        TUid ifUid = { KNSmlDSInterfaceUid };
        REComSession::ListImplementationsL( ifUid, implArray );
        for ( TInt i = 0 ; i < implArray.Count() ; i++ )
            {    
            CImplementationInformation* implInfo = implArray[i];
            CNSmlDSContentType* ctype = prof->ContentType( implInfo->ImplementationUid().iUid );
            if ( ctype )
                {            
                aArray.AppendL( ctype->IntValue( EDSAdapterTableId ) );
                }
            }
        REComSession::FinalClose();
        CleanupStack::PopAndDestroy(&implArray); 
        CleanupStack::PopAndDestroy(prof);
        }
    }

// --------------------------------------------------------------------------
// TInt CNSmlProfile::DataSize()
// --------------------------------------------------------------------------
//
TInt CNSmlProfile::DataSize()
    {
    return iBuffer->Size();
    }

// --------------------------------------------------------------------------
// const TPtr8& CNSmlProfile::ReadData()
// --------------------------------------------------------------------------
//
const TPtr8& CNSmlProfile::ReadData()
    {
     if ( iBuffer )
        {
        iDataPtr.Set(iBuffer->Ptr(0));    
        }    
    return iDataPtr;
    }

// --------------------------------------------------------------------------
// TInt CNSmlProfile::UpdateDataL( TInt& aId , const TDesC8& aData, TBool& aIsHidden )
// --------------------------------------------------------------------------
//
TInt CNSmlProfile::UpdateDataL( TInt& aId , const TDesC8& aData, TBool& aIsHidden )
    {    
    aIsHidden = EFalse;
    TInt ret(KErrNone);
    switch ( iUsageType )
        {        
        case ESmlDataSync:
            {            
            CNSmlDSProfile* prof;

            if ( iProfId == KNSmlNullId )
                {
                prof = iSession.DSSettings().CreateProfileL();
                }
            else
                {
                prof = iSession.DSSettings().ProfileL( iProfId );
                
                if ( !prof )
        			{
        			User::Leave( KErrNotFound );
        			}
                }
            User::LeaveIfNull(prof);

            CleanupStack::PushL(prof);

            aIsHidden = prof->IntValue( EDSProfileHidden );
            
            RDesReadStream stream;
            stream.Open(aData);
            CleanupClosePushL(stream);
          
            TInt maxlen(0);
            TInt pc(0);
         
            maxlen = stream.ReadInt32L();
            prof->SetStrValue( EDSProfileDisplayName, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
            maxlen = stream.ReadInt32L();
            prof->SetStrValue( EDSProfileSyncServerUsername, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
            maxlen = stream.ReadInt32L();
            prof->SetStrValue( EDSProfileSyncServerPassword, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
            maxlen = stream.ReadInt32L();
            prof->SetStrValue( EDSProfileServerId, HBufC::NewLC(stream,maxlen)->Des() ); pc++;

            prof->SetIntValue( EDSProfileServerAlertedAction, stream.ReadInt8L() );

            prof->SetIntValue( EDSProfileCreatorId, stream.ReadInt32L() );

            prof->SetIntValue( EDSProfileProtocolVersion, stream.ReadInt8L() );

            CleanupStack::PopAndDestroy(pc);

            ret = prof->SaveL();

            // read id after SaveL() and return to client.
            iProfId = prof->IntValue( EDSProfileId );

            aId = iProfId;

            CleanupStack::PopAndDestroy(&stream);
            CleanupStack::PopAndDestroy(prof);

            break;
            }
        case ESmlDevMan:
            {
            CNSmlDMProfile* prof;

            if ( iProfId == KNSmlNullId )
                {
                prof = iSession.DMSettings().CreateProfileL();
                }
            else
                {
                prof = iSession.DMSettings().ProfileL( iProfId );
                
                if ( !prof )
        			{
        			User::Leave( KErrNotFound );
        			}
                }
            User::LeaveIfNull(prof);

            CleanupStack::PushL(prof);

            RDesReadStream stream;
            stream.Open(aData);
            CleanupClosePushL(stream);
           
            TInt maxlen(0);
            TInt pc(0);

            maxlen = stream.ReadInt32L();
            prof->SetStrValue( EDMProfileDisplayName, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
            maxlen = stream.ReadInt32L();
            prof->SetStrValue( EDMProfileServerUsername, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
            maxlen = stream.ReadInt32L();
            prof->SetStrValue( EDMProfileServerPassword, HBufC::NewLC(stream,maxlen)->Des() ); pc++;
            maxlen = stream.ReadInt32L();
            HBufC* serverId = HBufC::NewLC(stream,maxlen); pc++; 
            prof->SetStrValue( EDMProfileServerId, *serverId);

            prof->SetIntValue( EDMProfileServerAlertAction, stream.ReadInt8L() );

            prof->SetIntValue(  EDMProfileCreatorId ,stream.ReadInt32L() );
            
            
            prof->SetIntValue( EDMProfileLock, stream.ReadInt8L() );
            
            maxlen = stream.ReadInt32L();
            prof->SetStrValue( EDMProfileClientPassword, HBufC::NewLC(stream,maxlen)->Des() ); pc++;

            
 
            if(iSession.DMSettings().ServerIdFoundL(*serverId,iProfId))
                {
                ret = KErrAlreadyExists;
                }
            else
                {
                ret = prof->SaveL();

                // read id after SaveL() and return to client.
                iProfId = prof->IntValue( EDMProfileId );

                aId = iProfId;
                }

            CleanupStack::PopAndDestroy(pc);
            
            CleanupStack::PopAndDestroy(&stream);
            CleanupStack::PopAndDestroy(prof);
            break;
            }
        default:
            ret = KErrNotFound;
            break;
        }    
    return ret;
    }
    
// --------------------------------------------------------------------------
// TBool CNSmlProfile::DeleteTaskL( const TInt aId )
// --------------------------------------------------------------------------
//
TBool CNSmlProfile::DeleteTaskL( const TInt aId )
    {
	TBool deleted = EFalse;
    CNSmlDSProfile* prof = iSession.DSSettings().ProfileL( iProfId );
    if ( prof )
        {
        CleanupStack::PushL(prof);
        deleted = prof->DeleteContentTypeL( aId );
        prof->SaveL();
        CleanupStack::PopAndDestroy(prof); 
        }
    
    return deleted;
    }

// --------------------------------------------------------------------------
// TSmlUsageType CNSmlProfile::GetUsageType()
// --------------------------------------------------------------------------
//
TSmlUsageType CNSmlProfile::GetUsageType()
	{
	return iUsageType;
	}

//  End of File 
