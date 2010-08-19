/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <SCPParamObject.h>

#include "SCPHistoryPlugin.h"
#include <scphistorypluginlang.rsg>
#include "SCP_IDs.h"
#include <SCPServerInterface.h>

// ============================= LOCAL FUNCTIONS  =============================

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::NewL
// Two-phased contructor
// (static, may leave)
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPHistoryPlugin* CSCPHistoryPlugin::NewL()
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::NewL()" ) ) );
	CSCPHistoryPlugin* self = new ( ELeave ) CSCPHistoryPlugin();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
    Dprint ( ( _L( "( 0x%x ) CSCPHistoryPlugin::NewL()" ), self ) );

    return self;
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::CSCPHistoryPlugin
// Constructor
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPHistoryPlugin::CSCPHistoryPlugin()
   
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::CSCPHistoryPlugin()" ) ) );
    return;
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::ConstructL
// 2nd phase construction
// (may leave)
// Status : Approved
// ----------------------------------------------------------------------------
//
void CSCPHistoryPlugin::ConstructL()
    {        
    Dprint ( ( _L( "CSCPHistoryPlugin::ConstructL()" ) ) );
    return;
    }
    
// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::HandleEvent
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPHistoryPlugin :: HandleEventL( TInt aID, CSCPParamObject& aParam, CSCPParamObject& aOutParam)
	{				
	Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent()" ) ) );
	
	if ( iFs == NULL )
	    {
	    return ; // Eventhandler not available
	    }	
		// check for Case
		
	switch ( aID )
        {

        case ( KSCPEventValidate ) :
            {            
           	// Obtain the paramValue
           	Dprint ( ( _L( "CSCPHistoryPlugin::KSCPEventValidate" ) ) );
			TInt passhistoryParamValue;
			passhistoryParamValue = GetHistoryCountParamValue();
			Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent passhistoryParamValue = %d" ), passhistoryParamValue ) );
			// if all required bounds are zero, there is nothing to do.
			if ( passhistoryParamValue != 0)
			    {
				// Get the configFile's path.
				// If this fails, there is something badly wrong(Private folder is not there)
				TInt errSCF = SetConfigFile ();
				if (errSCF != KErrNone)
				{
					break; // Break out from Case
				}

				// Get the historyItemCount, If the err is raised, the file is not there
				// This will lead to KSCPEventPasswordChanged event and new history file will
				// be created
				TInt historyItemCounter;
				TInt errHC = GetHistoryItemCount( historyItemCounter );
				Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent historyItemCounter = %d" ), historyItemCounter ) );
				if (errHC != KErrNone)
				{
					break; // Break out from Case
				}

				// continue with the KSCPEventValidate Check

				// Get the password from the paramObject
				TBuf<KSCPPasscodeMaxLength> seccode;
				if ( aParam.Get( KSCPParamPassword, seccode ) != KErrNone )
				{
					// Nothing to do anymore
					Dprint( (_L("CSCPHistoryPlugin::HandleEvent()\
					ERROR: KSCPEventValidate/KSCPParamPassword is != KErrNone") ));
					break; // Break out from Case
				}            
				
				// Hash  the securitycode	
				TBuf<KSCPPasscodeMaxLength> securityhash;
				iEventHandler->HashInput(seccode,securityhash);
			
				// get history
				CDesCArrayFlat* array = NULL;
				TInt errGH = KErrNone;
				
				array = new CDesCArrayFlat(1);			
				if ( array != NULL )
				    {
				    TRAPD( err2, errGH = GetHistoryArrayL( *array ) );
				    if ( err2 != KErrNone )
				        {
				        errGH = err2;
				        }
				    }
				else
				    {
				    errGH = KErrNoMemory;
				    }
			
				// If for some reason err is raised, break out
				// If the Historyonfig file get deleted on the fly ex
				if (errGH != KErrNone)
				    {
					array->Reset();
					delete array;
					break; // Break out from Case
				    }
				TInt correction;
				correction = 0;

				if ( array->Count() >=  passhistoryParamValue )
                {
                    correction =  array->Count() - passhistoryParamValue;
                }
				// check for match
				TBuf<KSCPPasscodeMaxLength> arrayItem;
				
				// Set the historyobject
				for (TInt i= 0 + correction; i < array->Count(); i++)
				    {
					arrayItem =  array->MdcaPoint(i);
					if (arrayItem.Compare(securityhash) == KErrNone)
					    {
                            aOutParam.Set( KSCPParamStatus, KErrSCPInvalidCode );
                            Dprint ( ( _L( "EDeviceLockHistoryBuffer Failed" ) ) );
                            aOutParam.AddtoFailedPolices(EDeviceLockHistoryBuffer);
                            break;
                        } // End of compare IF
                    } // End of For
					
                // kill the local
                array->Reset();
                delete array;

                } // passhistoryParamValue
            break;
            } // end of KSCPEventValidate
        // Someone has changed the Seccode and I need to include it to history
         case ( KSCPEventPasswordChanged ) :
			{																
			// Get the configFile's path.
			Dprint ( ( _L( "CSCPHistoryPlugin::KSCPEventPasswordChanged" ) ) );
			TInt errSCF = SetConfigFile ();
			if (errSCF != KErrNone)
			    {
				break; // Break out from the case
			    }
			
			// Get the password from the paramObject
     		TBuf<KSCPPasscodeMaxLength> securitycode;
            if ( aParam.Get( KSCPParamPassword, securitycode ) != KErrNone )
                {
            	// Nothing to do anymore
               	Dprint( (_L("CSCPHistoryPlugin::HandleEvent()\
               	ERROR: KSCPEventPasswordChanged/KSCPParamPassword is  != KErrNone") ));
				break; // Break out from the Case
                }          

			// Hash  the securitycode	
			TBuf<KSCPPasscodeMaxLength> securityhash;
			iEventHandler->HashInput(securitycode,securityhash);

			// Get the historyItemCount, If error occures, File is not there yet, Make one
			TInt historyItemCounter;
			TInt errHC = GetHistoryItemCount( historyItemCounter );
			Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent historyItemCounter = %d" ), historyItemCounter ) );
			if (errHC != KErrNone)
			    {
				// The file does not exist yet (should not happen)
				// Make the ParamObject,  Set the New historyData with count of 1
				CSCPParamObject* historyObject = NULL;
				TRAPD( err, historyObject = CSCPParamObject::NewL() );
		 		if ( err == KErrNone )
		 		    {
			 		historyObject->Set(KHistoryCounterParamID,1);
    				historyObject->Set(KHistoryItemParamBase,securityhash );
                    
                    TRAPD( errWC, historyObject->WriteToFileL( iCfgFilenamepath, iFs ) );
					if ( errWC != KErrNone )
    					{
						Dprint( (_L("CSCPHistoryPlugin::HandleEvent(): WARNING:\
						failed to write plugin configuration: %d"), errWC ));
						break; // Break out from the Case
	    				}
					delete historyObject;
		 		    }
			    }
			// There are passwords avail.
			else
			    {
				// Append the new passwords
				TInt err = KErrNone;
				TRAPD( err2, err = AppendAndWriteSecurityCodeL( securityhash  ) );
				if ( ( err != KErrNone ) || ( err2 != KErrNone ) )
				    {
					break; // Break out from the Case						
				    }										
			    }    					
			break;
			} // end of KSCPEventPasswordChanged
          
    	case ( KSCPEventConfigurationQuery ):
            {            
            Dprint ( ( _L( "CSCPHistoryPlugin::KSCPEventConfigurationQuery" ) ) );
        	TInt paramID = -1; 
            // Get the ID from the paramObject      
            if ( aParam.Get( KSCPParamID, paramID ) != KErrNone )
                {
                // Nothing to do anymore
                break;
                }            
                    
            // 1011
            if ( paramID == (RTerminalControl3rdPartySession::EPasscodeHistoryBuffer))
                {
				// All of our params are TInts
				TInt paramValue;
				if ( aParam.Get( KSCPParamValue, paramValue ) != KErrNone )
				    {
					aOutParam.Set( KSCPParamStatus, KErrGeneral );
					break;
				    }
            	            
					TInt retStatus = KErrNone;
					switch ( paramID )
					    {
				
						case ( RTerminalControl3rdPartySession::EPasscodeHistoryBuffer ):
						    {
							// Bounds are be be
							if ( ( paramValue < KPasscodeHistoryBufferMinValue ) 
							|| ( paramValue > KPasscodeHistoryBufferMaxValue ) )                                 
							    {
								// This is not a valid valuerange
								retStatus = KErrArgument;
						        }     
							           
						    break;				
						    } // end of case EPasscodeHistoryBuffer
					    } // end of switch ( paramID )
								
			        aOutParam.Set( KSCPParamStatus, retStatus );
                    }
			 break;	            
	         } //End of KSCPEventConfigurationQuery Case
            
            
          case ( KSCPEventReset ):
              {
              Dprint ( ( _L( "CSCPHistoryPlugin::KSCPEventReset" ) ) );
              // Reset the configuration for this plugin.
              TRAP_IGNORE( FlushConfigFileL() );
              
              break;
              }
                          
          } // End of  switch ( aID )
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::SetEventHandler
// SetEventHandler
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPHistoryPlugin::SetEventHandler( MSCPPluginEventHandler* aHandler )
	{
	Dprint ( ( _L( "CSCPHistoryPlugin::SetEventHandler()" ) ) );
	iEventHandler = aHandler;
	
	iFs = &(iEventHandler->GetFsSession());
	}	

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::~CSCPHistoryPlugin
// Destructor
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPHistoryPlugin::~CSCPHistoryPlugin()
    {
	Dprint( (_L("CSCPHistoryPlugin::~CSCPHistoryPlugin()") ));
    
	iRf.Close();   
	    
	return;
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::GetHistoryItemCount
// GetHistoryItemCount
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPHistoryPlugin::GetHistoryItemCount( TInt& aHistoryCount )
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::GetHistoryItemCount()" ) ) );
    // Make the ParamObject for success ack, 
    CSCPParamObject* historyObject = NULL;
    TRAPD( err, historyObject = CSCPParamObject::NewL() );
    if ( err != KErrNone )
        {
        return err;
        }

    TRAP( err, historyObject->ReadFromFileL( iCfgFilenamepath, iFs ) );
    if (  err != KErrNone  )
        {
    	// Something is wrong with the config file
    	// it is missing, caller will create new file
    	Dprint( (_L("CSCPHistoryPlugin::SetConfigFile:\
    	ERROR: Failed to ReadFromFileL: %d"), err ));
        }
    else
        {
    	historyObject->Get(KHistoryCounterParamID,aHistoryCount);
        }
	Dprint ( ( _L( "CSCPHistoryPlugin::GetHistoryItemCount aHistoryCount = %d" ), aHistoryCount ) );
    delete historyObject;
		             
	return err;
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::SetConfigFile
// SetConfigFile
// Status : Approved
// ----------------------------------------------------------------------------
//

TInt CSCPHistoryPlugin::SetConfigFile()
    {	
    Dprint ( ( _L( "CSCPHistoryPlugin::SetConfigFile()" ) ) );
	TRAPD( err, iEventHandler->GetStoragePathL( iCfgFilenamepath ) );
	if ( err != KErrNone )
	    {
		Dprint( (_L("CSCPHistoryPlugin::SetConfigFile:\
		ERROR: Failed to get storage path: %d"), err ));             
		return err;
	    }
	iCfgFilenamepath.Append( KConfigFile );
	return err;
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::AppendSecurityCode
// AppendSecurityCode
// Status : Approved
// ----------------------------------------------------------------------------
//

TInt CSCPHistoryPlugin::AppendAndWriteSecurityCodeL ( TDes& aSecuritycode )
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::AppendAndWriteSecurityCodeL()" ) ) );
	TInt err;
	err = KErrNone;
		
	TInt passhistoryParamValue;
	passhistoryParamValue = GetHistoryCountParamValue();
	Dprint ( ( _L( "CSCPHistoryPlugin::AppendAndWriteSecurityCodeL passhistoryParamValue = %d" ), passhistoryParamValue ) );
	if ( passhistoryParamValue == 0 )
	    {
	    // We must still save the currect password..
	    passhistoryParamValue = 1;
	    }

	// Make the ParamObject
	CSCPParamObject* historyObject = CSCPParamObject::NewL();
	CleanupStack::PushL( historyObject );
	
	// get history
	CDesCArrayFlat*  array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( array );
	
	err = GetHistoryArrayL( *array );
	// If for some reason err is raised, break out
	if (err != KErrNone)
	    {
		array->Reset();
		CleanupStack::PopAndDestroy( array );
		CleanupStack::PopAndDestroy( historyObject );
		return err;
	    }

	TBuf<KSCPPasscodeMaxLength> arrayItem;
	
  	// append the new pass to history to last index.  	
	array->AppendL( aSecuritycode );
	
	TInt correction;
	correction = 0;

	if ( array->Count() >=  passhistoryParamValue )
        {
        correction =  array->Count() - passhistoryParamValue;
        }    

  	// Set the historyobject
	for ( TInt i = 0 + correction; i <  array->Count(); i++ )
		{
        arrayItem =  array->MdcaPoint(i);
        historyObject->Set( KHistoryItemParamBase + i - correction, arrayItem );
        arrayItem.Zero();
		}
					
	// Set the historycount
	historyObject->Set(KHistoryCounterParamID, array->Count() - correction );

	// Write
	TRAP( err, historyObject->WriteToFileL( iCfgFilenamepath, iFs ) );
	if (  err != KErrNone  )
	    {
		Dprint( (_L("CSCPHistoryPlugin::AppendSecurityCode(): WARNING:\
		failed to write plugin configuration: %d"), err )); 
	    }
	
	// Kill the local array
	array->Reset();
	CleanupStack::PopAndDestroy( array );
	
	CleanupStack::PopAndDestroy( historyObject );
	
	return err;	
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::GetHistoryArray
// Reads the historyconfig file and retrieves the history data
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPHistoryPlugin::GetHistoryArrayL ( CDesCArrayFlat& array )
    {   
	Dprint( (_L("CSCPHistoryPlugin::GetHistoryArray()") ));
	
    TBuf<KSCPPasscodeMaxLength> arrayItem;
    TInt historyCount;
	 
	// Make the ParamObject
	CSCPParamObject* historyObject = CSCPParamObject::NewL();
	CleanupStack::PushL( historyObject );

	TRAPD( err, historyObject->ReadFromFileL( iCfgFilenamepath, iFs ) );
	if ( err != KErrNone ) 
	    {
		// Reading from history fails. 
		Dprint( (_L("CSCPHistoryPlugin::GetHistoryArray():\
		failed to read plugin configuration: %d"), err )); 
		CleanupStack::PopAndDestroy( historyObject );
		return err;
	    }

	// Get the historyCount
	historyObject->Get(KHistoryCounterParamID,historyCount);
	Dprint ( ( _L( "CSCPHistoryPlugin::GetHistoryArrayL historyCount = %d" ), historyCount ) );
	// Loop them into array
	
	for (TInt i = 0 ; i < historyCount ; i ++)
    	{
		historyObject->Get(KHistoryItemParamBase+i,arrayItem);
		array.AppendL(arrayItem);
		arrayItem.Zero();
	    }

    CleanupStack::PopAndDestroy( historyObject );
	return err;	
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::GetHistoryCountParamValue
// GetHistoryCountParamValue
// Status : Approved
// ----------------------------------------------------------------------------
//

TInt CSCPHistoryPlugin::GetHistoryCountParamValue()
    {
	Dprint( (_L("CSCPHistoryPlugin::GetHistoryCountParamValue()") ));
	// Get required params for bounds for EPasscodeHistoryBuffer
	CSCPParamObject& config = iEventHandler->GetParameters();	

	// These are the dault values for ThisPlugIn's functions
	TInt passcodehistorycount;

	// Get Values with ID's
	if ( config.Get( ( RTerminalControl3rdPartySession::EPasscodeHistoryBuffer), 
	                   passcodehistorycount ) !=  KErrNone )
	    {
	    passcodehistorycount = 0;
	    }	
	Dprint ( ( _L( "CSCPHistoryPlugin::GetHistoryCountParamValue passcodehistorycount = %d" ), passcodehistorycount ) );
	return passcodehistorycount;
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::FlushConfigFile
// Remove all the other passwords from the file, except the last one (current)
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPHistoryPlugin::FlushConfigFileL()
    {			
    Dprint ( ( _L( "CSCPHistoryPlugin::FlushConfigFileL()" ) ) );
	// Make the ParamObject
	CSCPParamObject* historyObject = CSCPParamObject::NewL();
	CleanupStack::PushL( historyObject );
	
	// get history
	CDesCArrayFlat*  array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( array );
	
	TInt err = GetHistoryArrayL( *array );
	// If for some reason err is raised, break out
	if (err != KErrNone)
	    {
		array->Reset();
		CleanupStack::PopAndDestroy( array );
		CleanupStack::PopAndDestroy( historyObject );
		return err;
	    }
	
	// Set the parameters
	TBuf<KSCPPasscodeMaxLength> arrayItem = array->MdcaPoint( array->Count() - 1 );
	
	historyObject->Set( KHistoryCounterParamID, 1 );			
	historyObject->Set( KHistoryItemParamBase, arrayItem );
	
	// Write
	TRAP( err, historyObject->WriteToFileL( iCfgFilenamepath, iFs ) );
	if (  err != KErrNone  )
	    {
		Dprint( (_L("CSCPHistoryPlugin::AppendSecurityCode(): WARNING:\
		failed to write plugin configuration: %d"), err )); 
	    }
	
	// Kill the local array
	array->Reset();
	CleanupStack::PopAndDestroy( array );
	
	CleanupStack::PopAndDestroy( historyObject );
	
	return err;			
    } 
// End of File
