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

#include "SCPSpecificStringsPlugin.h"
#include <scphistorypluginlang.rsg>
#include "SCP_IDs.h"
#include <featmgr.h>
#include <SCPServerInterface.h>
// ============================= LOCAL FUNCTIONS  =============================

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::NewL
// Two-phased contructor
// (static, may leave)
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPSpecificStringsPlugin* CSCPSpecificStringsPlugin::NewL()
    {
	CSCPSpecificStringsPlugin* self = new ( ELeave ) CSCPSpecificStringsPlugin();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
    Dprint ( ( _L( "( 0x%x ) CSCPSpecificStringsPlugin::NewL()" ), self ) );

    return self;
    }

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::CSCPSpecificStringsPlugin
// Constructor
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPSpecificStringsPlugin::CSCPSpecificStringsPlugin()
   
    {
    Dprint ( ( _L( "CSCPSpecificStringsPlugin::CSCPSpecificStringsPlugin()" ) ) );
    return;
    }

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::ConstructL
// 2nd phase construction
// (may leave)
// Status : Approved
// ----------------------------------------------------------------------------
//
void CSCPSpecificStringsPlugin::ConstructL()
    {
    
    FeatureManager::InitializeLibL();
	if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{
    	FeatureManager::UnInitializeLib();
   		User::Leave( KErrNotSupported );
  	}
   	FeatureManager::UnInitializeLib();
    return;
    }
    
// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::HandleEvent
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPSpecificStringsPlugin :: HandleEventL( TInt aID, CSCPParamObject& aParam,CSCPParamObject& aOutParam )
	{	
		
	if ( iFs == NULL )
	    {
	    User::Leave(KErrGeneral);
	    }

	// check for Case
	switch ( aID )
	    {
		case ( KSCPEventValidate ) :
    		{   		
			TBool specificstringscheck;
			
			// Get params
			CSCPParamObject& config = iEventHandler->GetParameters();
			
			// Check if validation is required.
			if ( config.Get( ( RTerminalControl3rdPartySession::EPasscodeCheckSpecificStrings), 
				  specificstringscheck ) !=  KErrNone )
				   specificstringscheck = EFalse;

			 if ( specificstringscheck )
			    {
			 	// Check for specific strings			 	
			 	
				// Get the configFile's path.
				// If this fails, there is something badly wrong(Private folder is not there)
				TInt errSCF = SetConfigFile ();
				if (errSCF != KErrNone)
				    {
					break; // Break out from Case!
				    }
			 	
			 	// Get the password from the paramObject
	     		TBuf<KSCPPasscodeMaxLength> securitycode;
	            if ( aParam.Get( KSCPParamPassword, securitycode ) != KErrNone )
                    {
                	// Nothing to do anymore
                   	Dprint( (_L("CSCPSpecificStringsPlugin::HandleEvent()\
                   	ERROR: KSCPEventPasswordChanged/KSCPParamPassword is  != KErrNone") ));
					break; // Break out from the Case!;
                    }			 	
			 	
			 	TBool forbiddensecuritycode = EFalse;
			 	
			 	TInt errCSS = CheckSpecificStrings ( securitycode, forbiddensecuritycode );
			 	if (errCSS != KErrNone)
				    {
					break; // Break out from Case!
    				}
			 	
			 	if ( forbiddensecuritycode )
			    	{
			    	    aOutParam.Set( KSCPParamStatus, KErrSCPInvalidCode );
			    	    Dprint ( ( _L( "EDeviceLockDisallowSpecificStrings Failed" ) ) );
			    	    aOutParam.AddtoFailedPolices(EDeviceLockDisallowSpecificStrings);
			 	    }			 	    		
					
			    } // end of specificstringscheck

			break;
		    } // End of KSCPEventValidate

	    case ( KSCPEventRetrieveConfiguration ):
	        {
	        // This event is sent when a client wishes to retrieve the value for the given parameter.
	        // None of the parameters, which handle the string-buffer cannot be retrieved by anyone,
	        // so we'll return KErrNotSupported. Note that EPasscodeCheckSpecificStrings is OK to get.
	        
	        // Get the ID from the paramObject   
	        TInt paramID = -1;    
			if ( aParam.Get( KSCPParamID, paramID ) != KErrNone )
			    {
				// Nothing to do anymore
				break;
    			}   
			    
			if ( ( paramID ==  RTerminalControl3rdPartySession::EPasscodeDisallowSpecific )
				||  ( paramID ==  RTerminalControl3rdPartySession::EPasscodeAllowSpecific)
				||  ( paramID ==  RTerminalControl3rdPartySession::EPasscodeClearSpecificStrings) )
			    {
	                Dprint ( ( _L( "CSCPSpecificStringsPlugin::HandleEvent():\
	                    Get not supported for %d" ), paramID ) );
	                aOutParam.Set( KSCPParamStatus, KErrNotSupported );
	            }
	        break;
	        }

		// same for All plugins
		case ( KSCPEventConfigurationQuery ):
    		{   		
			TInt paramID = -1; 

			// Get the ID from the paramObject      
			if ( aParam.Get( KSCPParamID, paramID ) != KErrNone )
			    {
				// Nothing to do anymore
				break;
			    }            

			if ( paramID == (RTerminalControl3rdPartySession::EPasscodeCheckSpecificStrings)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeDisallowSpecific)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeAllowSpecific)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeClearSpecificStrings) )
			    {
				TInt retStatus = KErrNone;
				
			    if ( paramID != RTerminalControl3rdPartySession::EPasscodeCheckSpecificStrings )
			        {
        		    // Set the storage attribute, so the server won't save the value,
        		    // we'll do it ourselves.
        			aOutParam.Set( KSCPParamStorage, KSCPStoragePrivate );			        
			        }

				switch ( paramID )
				    {
					case ( RTerminalControl3rdPartySession::EPasscodeCheckSpecificStrings ):
					    {
						// 1015				
						TInt paramValue;
						if ( (aParam.Get( KSCPParamValue, paramValue ) != KErrNone) ||
							 ( paramValue < 0 ) || 	( paramValue > 1 ) ) 
						       retStatus = KErrArgument;
						
					    break;
					    }
					
					// 1016
					// Handle param change for strings to block
					case ( RTerminalControl3rdPartySession::EPasscodeDisallowSpecific ):
					    {
						// Get the params value's length
						// If param length is not correct, there's something very wrong. out
						TInt paramlength = aParam.GetParamLength( KSCPParamValue );
						if ( paramlength <= 0)
						    {
							retStatus = KErrArgument;
							break; // from case!
						    }

						HBufC* paramvaluebuf = NULL;
						TRAPD( err, paramvaluebuf = HBufC::NewL( paramlength ) );
						if ( err != KErrNone )
						    {
						    retStatus = err;
						    break;
						    }
						    
						TPtr paramptr = paramvaluebuf->Des();
						aParam.Get( KSCPParamValue, paramptr );

						// Check for basic test
						if ( IsNotValidWord ( paramptr ) )
						    {
							retStatus = KErrArgument;
							delete paramvaluebuf;
							break;
    						}
    						
						// Send them to parser and store them
						TInt errgPS = KErrNone;
						TRAPD( err2, errgPS = ParseAndStoreL( paramptr ) );
						if ( err2 != KErrNone )
						    {
						    errgPS = err2;
						    }
						
						delete paramvaluebuf;
						
						if (errgPS != KErrNone)
						    {
							if (errgPS == KErrArgument)
							    {
								retStatus = KErrArgument;
								break; // from case!
							    }	
							break; // Break out.
						    } 
					    break;
					    }
					

					// 1017
					// Handle param change for strings exclude from the list
					case ( RTerminalControl3rdPartySession::EPasscodeAllowSpecific ):
					    {
	
						// Get the params value's length
						// If param length is not correct, there's something very wrong. out
						TInt paramlength = aParam.GetParamLength( KSCPParamValue );
						if ( paramlength <= 0)
						    {
							retStatus = KErrArgument;
							break; // from case!
						    }
																		
						HBufC* paramvaluebuf = NULL;
						TRAPD( err, paramvaluebuf = HBufC::NewL( paramlength ) );
						if ( err != KErrNone )
						    {
						    retStatus = err;
						    break;
						    }
						    
						TPtr paramptr = paramvaluebuf->Des();
						aParam.Get( KSCPParamValue, paramptr );

						// Check for basic test
						if ( IsNotValidWord ( paramptr ) )
						    {
							retStatus = KErrArgument;
							delete paramvaluebuf;
							break;
						    }
						    
						// Send the word to remover and gets them off the list
						TInt errgPR = KErrNone;
						TRAPD( err2, errgPR = ParseAndRemoveL( paramptr ) );
						if ( err2 != KErrNone )
						    {
						    errgPR = err2;
						    }
						
						delete paramvaluebuf;
						
						if (errgPR != KErrNone)
						    {
							if (errgPR == KErrArgument)
							    {
							    retStatus = KErrArgument;
								break; // from case!
							    }
	
							break; // Break out.
						    } 												
						
					    break;
					    }												
					
					case ( RTerminalControl3rdPartySession::EPasscodeClearSpecificStrings ):
					    {					
						// 1018, Flush the config file

					    TRAP_IGNORE( FlushConfigFileL() );
						break;
					    }							
    				} // End of switch                          

				aOutParam.Set( KSCPParamStatus, retStatus );
			    }
			break;
		    } //End of KSCPEventConfigurationQuery Case

        case ( KSCPEventReset ):
              {
              // Reset the configuration for this plugin.
              TRAP_IGNORE( FlushConfigFileL() );
              
              break;
              }

	} // End of  switch ( aID )
	}

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::SetEventHandler
// SetEventHandler
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPSpecificStringsPlugin::SetEventHandler( MSCPPluginEventHandler* aHandler )
	{
	iEventHandler = aHandler;
	
	iFs = &(iEventHandler->GetFsSession());
	}	

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::~CSCPSpecificStringsPlugin
// Destructor
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPSpecificStringsPlugin::~CSCPSpecificStringsPlugin()
    {
	Dprint( (_L("CSCPSpecificStringsPlugin::~CSCPSpecificStringsPlugin()") ));
    
	iRf.Close();   
        	
	return;
    }

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::SetConfigFile
// SetConfigFile
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPSpecificStringsPlugin::SetConfigFile()
    {	
	Dprint( (_L("CSCPSpecificStringsPlugin::SetConfigFile()") ));
	TRAPD( err, iEventHandler->GetStoragePathL( iCfgFilenamepath ) );
	if ( err != KErrNone )
        {
		Dprint( (_L("CSCPSpecificStringsPlugin::SetConfigFile:\
		ERROR: Failed to get storage path: %d"), err ));             
		return err;
	    }
	iCfgFilenamepath.Append( KConfigFileSpecific );
	return err;
    }

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::CheckSpecificStrings
// CheckSpecificStrings
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPSpecificStringsPlugin::CheckSpecificStrings( TDes& aSecuritycode, 
													  TBool& aForbiddenSecurityCode )
    {
	Dprint( (_L("CSCPSpecificStringsPlugin::CheckSpecificStrings()") ));
	// Default is not blocklistedword
	aForbiddenSecurityCode = EFalse;
	
	// get GetSpecificStringsArray
	CDesCArrayFlat*  array = new CDesCArrayFlat(1);
	TInt err = KErrNone;
	
	if ( array == NULL )
	    {
	    err = KErrNoMemory;
	    }
	else
	    {
	    TRAPD( err2, err = GetSpecificStringsArrayL( *array ) );
	    if ( err2 != KErrNone )
	        {
	        err = err2;
	        }
	    }
	
	if (err != KErrNone)
	    {
		// Kill local array
		if ( array != NULL )
		    {
		    array->Reset();    
		    delete array;
		    }
		    		
		return err;
	    }

	// check for match
	TBuf<KSCPPasscodeMaxLength> arrayItem;
	// We use the uppercase version of the given string while checking
	TBuf<KSCPPasscodeMaxLength> secCodeBuf;
	secCodeBuf = aSecuritycode;
	secCodeBuf.UpperCase();

	// Set the historyobject
	for (TInt i=0; i < array->Count(); i++)
    	{
		arrayItem.Zero();
		arrayItem = array->MdcaPoint(i);
		arrayItem.UpperCase();
		// Check if this string causes the password to be invalid
		// the strings are matched by a case-insensitive substring match.
		if ( secCodeBuf.Find( arrayItem ) != KErrNotFound )
		    {
			aForbiddenSecurityCode = ETrue;
			break; // Out from for
		    }
    	}
    	
	// Kill local array
	array->Reset();
	delete array;
	return err; 
    }

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::ParseAndStoreL
// ParseAndStore ForbiddenSecurityCodes
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPSpecificStringsPlugin::ParseAndStoreL( TDes& aForbiddenSecurityCodes )
    {
	Dprint( (_L("CSCPSpecificStringsPlugin::ParseAndStore()") ));
	// Set The file environment
	TInt errSC = SetConfigFile ();
	if (errSC != KErrNone) return errSC;
	

	// Make array for data
	CDesCArrayFlat*  array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( array );

	// Read the previous wordlist.
	// Reading might fall for NotFound, don't care
	TInt errGS = GetSpecificStringsArrayL( *array );
	
	if (errGS != KErrNone && errGS != KErrNotFound  )
	    {
		// Kill local array
		array->Reset();
		CleanupStack::PopAndDestroy( array );
		return errGS;
	    }
		
	// Parse the ; separated values into array
	TLex16 lex (aForbiddenSecurityCodes);
	lex.Mark();
	while(!lex.Eos())
	    {
		while(lex.Peek() != ';' && !lex.Eos() ) lex.Inc();

		array->AppendL(lex.MarkedToken());
		lex.Inc();
		lex.Mark();
		}
		
		
	// Check for duplicates!
	RemoveDuplicateWords( *array );

	// Store Array
	// Make the ParamObject
	CSCPParamObject* forbiddensecuritycodeobject = CSCPParamObject::NewL();
	CleanupStack::PushL( forbiddensecuritycodeobject );
	TBuf<KSCPPasscodeMaxLength> arrayItem;

	// Append the Data 
	for (TInt i=0 ; i < array->Count(); i++)
		{
		arrayItem =  array->MdcaPoint(i);
		forbiddensecuritycodeobject->Set( KSpecificStringsParamBase+i,arrayItem);
		arrayItem.Zero();
		}

	// Set the Counter param
	forbiddensecuritycodeobject->Set( KSpecificStringsCounterParamID,array->Count());
			
	//Write
	TRAPD( err, forbiddensecuritycodeobject->WriteToFileL( iCfgFilenamepath, iFs ) );
	if (  err != KErrNone  )
	    {
		Dprint( (_L("CSCPSpecificStringsPlugin::ParseAndStore(): WARNING:\
		failed to write plugin configuration: %d"), err )); 
    	}

    CleanupStack::PopAndDestroy( forbiddensecuritycodeobject );
    
    // Kill the array
	array->Reset();
	CleanupStack::PopAndDestroy( array );
    
	return err;
}


// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::FlushConfigFileL
// FlushConfigFile
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPSpecificStringsPlugin::FlushConfigFileL()
    {
	Dprint( (_L("CSCPSpecificStringsPlugin::FlushConfigFile()") ));

	SetConfigFile();

	CFileMan* fileMan = CFileMan::NewL( *iFs );
	CleanupStack::PushL( fileMan );

	TInt ret= fileMan->Delete( iCfgFilenamepath, 0 );

	CleanupStack::PopAndDestroy( fileMan ); // fileMan
	return ret;
    }


// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::GetSpecificStringsArrayL
// Reads the historyconfig file and retrieves the forbidden data
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPSpecificStringsPlugin::GetSpecificStringsArrayL( CDesCArrayFlat& array )
{   

	Dprint( (_L("CSCPSpecificStringsPlugin::GetSpecificStringsArray()") ));
	
	 TBuf<KSCPPasscodeMaxLength> arrayItem;
	 TInt counter;
	 TInt err = KErrNone;
	 
	// Make the ParamObject
	CSCPParamObject* forbiddensecuritycodeobject = CSCPParamObject::NewL();
	CleanupStack::PushL( forbiddensecuritycodeobject );

	TRAP( err, forbiddensecuritycodeobject->ReadFromFileL( iCfgFilenamepath, iFs ) );
	if ( err != KErrNone ) 
	{
			// Reading from history fails. 
			Dprint( (_L("CSCPHistoryPlugin::GetHistoryArray():\
			failed to read plugin configuration: %d"), err )); 
			CleanupStack::PopAndDestroy( forbiddensecuritycodeobject );
			return err;
	}

	// Get the historyCount
	forbiddensecuritycodeobject->Get(KSpecificStringsCounterParamID,counter);

	// Loop them into array
	for (TInt i = 0 ; i < counter ; i ++)
	{
		forbiddensecuritycodeobject->Get(KSpecificStringsParamBase+i,arrayItem);
		array.AppendL(arrayItem);
		arrayItem.Zero();
	}

    CleanupStack::PopAndDestroy( forbiddensecuritycodeobject );
	return err;
	
}

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::IsNotValidWord
// Checks wheater the given buf contains any illegal chars like ., <space>
// ; char is granted due to a delimiter role. ETrue means buf is no good!
// Status : Approved
// ----------------------------------------------------------------------------
//
TBool CSCPSpecificStringsPlugin::IsNotValidWord ( TDes& aForbiddenSecurityCodes )
{
	
	Dprint( (_L("CSCPSpecificStringsPlugin::IsNotValidWord()") ));
	// If so, there is a problem ETrue!!
	TBool ret;
	TChar now,previous;		// at hand & prevous char
	ret = EFalse;
	now = 0;
	previous = 0;
	
	// Go through the word
	for (TInt i=0; i < aForbiddenSecurityCodes.Length(); i++)
	{
		
		// Get the current Char to examine
		now =  static_cast<TChar>( aForbiddenSecurityCodes[i] );
		// And the previous
		if ( i >= 1 ) 
		    {
		    previous = static_cast<TChar>( aForbiddenSecurityCodes[i-1] );
		    }
	
		// Check for Sapce!
		if ( now.IsSpace() || now < 1 || now > 126 )
		{
			// Space or Invalid character found, bad, out!
			ret = ETrue;
			break;
			
		}
		
		
		/*if (! now.IsAlphaDigit() )
		{
				// Was not alphadigit. Anyhow ; must come through
				if ( now != ';'  )
				{
					// bad, out!
					ret = ETrue;
					break;
				}
				
				if ( now == ';' && previous == ';'  )
				{
					// bad, out!
					ret = ETrue;
					break;
				}
		
		}*/

	} // EOFor
	return ret;
}

// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::RemoveDuplicateWords
// Checks wheater there are duplicate words in the array, removes them!
// Status : Approved
// ----------------------------------------------------------------------------
//
void CSCPSpecificStringsPlugin::RemoveDuplicateWords( CDesCArrayFlat& array )
{
	Dprint( (_L("CSCPSpecificStringsPlugin::RemoveDuplicateWords()") ));
	// check for match
	TBuf<KSCPPasscodeMaxLength> arrayItemAt;
	TBuf<KSCPPasscodeMaxLength> arrayItemNext;
	TInt arraycounter =0;
	while (arraycounter < array.Count())
	{
		// Get the item from array.
		arrayItemAt =  array.MdcaPoint(arraycounter);	
		
		// search the rest for match
		for (TInt n = 1 + arraycounter ; n < array.Count() ; n++)
		{		
			arrayItemNext = array.MdcaPoint(n);
			if (arrayItemAt.Compare(arrayItemNext) == KErrNone)
			{
				// Delete 
				array.Delete(n);
				n--; 
				
			}
		}
		arraycounter++;
	
	}

}


// ----------------------------------------------------------------------------
// CSCPSpecificStringsPlugin::ParseAndRemoveL
// Parses And Removes ForbiddenSecurityCodes from ini file
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPSpecificStringsPlugin::ParseAndRemoveL( TDes& aForbiddenSecurityCodes )
    {
	Dprint( (_L("CSCPSpecificStringsPlugin::ParseAndRemoveL()") ));
	// Set The file environment
	TInt errSC = SetConfigFile ();
	if (errSC != KErrNone) return errSC;


	// Make array for data
	CDesCArrayFlat*  arraycurrent = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( arraycurrent );
	
	// Read the previous wordlist.
	// Reading might fall for NotFound, get out, nothing to do.
	if ( GetSpecificStringsArrayL( *arraycurrent ) == KErrNotFound  )
	    {
	    arraycurrent->Reset();
	    CleanupStack::PopAndDestroy( arraycurrent );
	    return KErrNone;
	    }

	// Make array for data for wordlist to be removed
	CDesCArrayFlat*  arrayremoved = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( arrayremoved );
	
	// Parse the ; separated values into array from given wordlist
	TLex16 lex (aForbiddenSecurityCodes);
	lex.Mark();
	while(!lex.Eos())
	{
		while(lex.Peek() != ';' && !lex.Eos() ) lex.Inc();

			arrayremoved->AppendL(lex.MarkedToken());
			lex.Inc();
			lex.Mark();
		}
		
	// Remove possible duplicates in wordlist
	RemoveDuplicateWords( *arrayremoved );
	
	// Make the ParamObject
	CSCPParamObject* forbiddensecuritycodeobject = CSCPParamObject::NewL();
	CleanupStack::PushL( forbiddensecuritycodeobject );
	
	TBuf<KSCPPasscodeMaxLength> arrayItemcurrent;
	TBuf<KSCPPasscodeMaxLength> arrayItemremove;
	TInt addedcounter,removedcounter,currentItemCounter,err;
	TBool itemremovedexlude,itemremovecurrent;
	itemremovedexlude = EFalse;
	itemremovecurrent = EFalse;
	addedcounter = 0;
	removedcounter = 0;
	err = 0;
	
	// Remember the current situation before exluding.
	currentItemCounter = arraycurrent->Count();
	
	// Loop the main array
	while (addedcounter < arraycurrent->Count())
	{
		// Get the item from current spesific wordarray.
		arrayItemcurrent =  arraycurrent->MdcaPoint(addedcounter);	
		while (removedcounter < arrayremoved->Count())
		{
			
			arrayItemremove =  arrayremoved->MdcaPoint(removedcounter);
			if(arrayItemcurrent.CompareC(arrayItemremove) == KErrNone)
			{
				arrayremoved->Delete(removedcounter);
				arraycurrent->Delete(addedcounter);
				itemremovedexlude = ETrue;
				itemremovecurrent = ETrue;
			}
			if (itemremovedexlude) itemremovedexlude = EFalse;
			else removedcounter++;
	
		}
	
		if (itemremovecurrent) itemremovecurrent = EFalse;
		else addedcounter++;
		removedcounter = 0;
	}
	
	// Check wheather there has been eny changes in the current array (removes)
	if (currentItemCounter != arraycurrent->Count() )
	    {
		// All of items were removed, flush the file
		if ( arraycurrent->Count() == 0 )
		    {
		    TRAP_IGNORE( FlushConfigFileL() );
		    }
		else
		    {
			// Reuse
			arrayItemcurrent.Zero();
			// Set the object with fixed array
			for (TInt n = 0; n < arraycurrent->Count() ; n++)
			{
				// Get the item from current spesific wordarray.
				arrayItemcurrent =  arraycurrent->MdcaPoint(n);	
				forbiddensecuritycodeobject->Set( KSpecificStringsParamBase+n,arrayItemcurrent);
			}
		
			// Set the Counter param
			forbiddensecuritycodeobject->Set( KSpecificStringsCounterParamID,arraycurrent->Count());

			//Write
			TRAP( err, forbiddensecuritycodeobject->WriteToFileL( iCfgFilenamepath, iFs ) );
			if (  err != KErrNone  )
			    {
				Dprint( (_L("CSCPSpecificStringsPlugin::ParseAndRemoveL(): WARNING:\
				failed to write plugin configuration: %d"), err )); 
			    }			
		    }		
	    }
	
	
	CleanupStack::PopAndDestroy( forbiddensecuritycodeobject );
	
	// Kill arrays
	arrayremoved->Reset();
	CleanupStack::PopAndDestroy( arrayremoved );
	
	arraycurrent->Reset();
	CleanupStack::PopAndDestroy( arraycurrent );			    
 
	return err;
    }
    
// End of File
