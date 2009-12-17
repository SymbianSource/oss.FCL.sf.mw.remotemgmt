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
#include <featmgr.h>

#include "SCPPatternPlugin.h"
#include <SCPPatternPluginLang.rsg>
#include "SCP_IDs.h"




// ============================= LOCAL FUNCTIONS  =============================

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::NewL
// Two-phased contructor
// (static, may leave)
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPPatternPlugin* CSCPPatternPlugin::NewL()
    {
	CSCPPatternPlugin* self = new ( ELeave ) CSCPPatternPlugin();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
    Dprint ( ( _L( "( 0x%x ) CSCPPatternPlugin::NewL()" ), self ) );

    return self;
    }

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::CSCPPatternPlugin
// Constructor
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPPatternPlugin::CSCPPatternPlugin()   
    {
    Dprint ( ( _L( "CSCPPatternPlugin::CSCPPatternPlugin()" ) ) );
    return;
    }

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::ConstructL
// 2nd phase construction
// (may leave)
// Status : Approved
// ----------------------------------------------------------------------------
//
void CSCPPatternPlugin::ConstructL()
    {       
    FeatureManager::InitializeLibL();
	if(!FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
	{
    	FeatureManager::UnInitializeLib();
   		User::Leave( KErrNotSupported );
  	}
   	FeatureManager::UnInitializeLib();    
    Dprint ( ( _L( "CSCPPatternPlugin::ConstructL()" ) ) );
    return;
    }
    

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::GetResource
// GetResource, opens the localisation filesystem
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPPatternPlugin::GetResource()
    {
 	Dprint( (_L("CSCPPatternPlugin::GetResource()") ));
	// The resource has to be loaded manually since it is not an application.        
	
	if ( iFs == NULL )
	    {
	    return KErrNotReady;
	    }

    // can't use resource here because it is not added yet....
    TFileName resourceFile;
    resourceFile.Append( KDriveZ );
    resourceFile.Append( SCPPatternPluginSrcFile );
    BaflUtils::NearestLanguageFile( *iFs, resourceFile );
    
    TRAPD( err, 
        iRf.OpenL( *iFs, resourceFile );
        iRf.ConfirmSignatureL();
        );
        
    return err;
    }

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::LoadResourceLC
// LoadResourceLC, loads the localisation resource
// Status : Approved
// ----------------------------------------------------------------------------
//
HBufC16* CSCPPatternPlugin::LoadResourceLC( TInt aResId )
    {
	Dprint ( ( _L( "CSCPPatternPlugin::LoadResourceLC()" ) ) );

	// load the actual resource
    HBufC8* readBuffer = iRf.AllocReadLC( aResId );
    // as we are expecting HBufC16...
    const TPtrC16 ptrReadBuffer( (TText16*) readBuffer->Ptr(),
                                 ( readBuffer->Length() + 1 ) >> 1 );
    HBufC16* textBuffer=HBufC16::NewL( ptrReadBuffer.Length() );    
    *textBuffer=ptrReadBuffer;
    CleanupStack::PopAndDestroy( readBuffer ); // readBuffer
    CleanupStack::PushL( textBuffer );
  	return textBuffer;
    }


// ----------------------------------------------------------------------------
// CSCPPatternPlugin::~CSCPPatternPlugin
// Destructor
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPPatternPlugin::~CSCPPatternPlugin()
    {
    Dprint( ( _L( "CSCPPatternPlugin::~CSCPPatternPlugin()" ) ) ) ;
    
	// Closes the resource file reader. 
	// This function is called after finishing reading all resources
    iRf.Close();   
	
    return;
    }
// ----------------------------------------------------------------------------
// CSCPPatternPlugin::HandleEvent
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
CSCPParamObject* CSCPPatternPlugin::HandleEvent( TInt aID, CSCPParamObject& aParam )
	{	
	
	Dprint ( ( _L( "CSCPPatternPlugin::HandleEvent()" ) ) );
	
	// Make the ParamObject for success ack, Delete later
	CSCPParamObject* retParams = NULL;
		
	TBool errRaised;
	errRaised = EFalse;
	
	TBool isInvalid = EFalse;
	
	// check for Case
    switch ( aID )
        {
        case ( KSCPEventValidate ):
            {
		     // Get required params for bounds
			CSCPParamObject& config = iEventHandler->GetParameters();	

			TBool argumentError = EFalse;
			
			// These are the dault values for ThisPlugIn's functions
			TInt passcodeminlength, passcodemaxlength, passcodemaxrepeatedcharacters, passcodeminspecialcharacters;
			TBool passcoderequireupperandlower, passcoderequirecharsandnumbers, passcodeconsecutivenumbers, passcodedisallowsimple;
			
			// Get Values with ID's
			if ( config.Get( ( RTerminalControl3rdPartySession::EPasscodeMinLength), 
				passcodeminlength ) !=  KErrNone )
					passcodeminlength = KSCPPasscodeMinLength;
			
			if ( config.Get( (RTerminalControl3rdPartySession::EPasscodeMaxLength),
				 passcodemaxlength ) !=  KErrNone )
					passcodemaxlength = KSCPPasscodeMaxLength;
			
			if ( config.Get( (RTerminalControl3rdPartySession::EPasscodeMaxRepeatedCharacters), 
				 passcodemaxrepeatedcharacters	 ) !=  KErrNone )
					passcodemaxrepeatedcharacters = 0;
			
			if ( config.Get( (RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers), 
				 passcoderequirecharsandnumbers ) !=  KErrNone )
					passcoderequirecharsandnumbers = EFalse;

			if ( config.Get( (RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower), 
				 passcoderequireupperandlower ) !=  KErrNone )
					passcoderequireupperandlower = EFalse;
					
			if ( config.Get( (RTerminalControl3rdPartySession::EPasscodeConsecutiveNumbers), 
				 passcodeconsecutivenumbers ) !=  KErrNone )
					passcodeconsecutivenumbers = EFalse;
					
            if ( config.Get( (RTerminalControl3rdPartySession::EPasscodeMinSpecialCharacters), 
                 passcodeminspecialcharacters   ) !=  KErrNone )
                    passcodeminspecialcharacters = 0;
            		
            if ( config.Get( (RTerminalControl3rdPartySession::EPasscodeDisallowSimple), 
                 passcodedisallowsimple ) !=  KErrNone )
                    passcodedisallowsimple = EFalse;
			
            // Get the password from the paramObject
            TBuf<KSCPPasscodeMaxLength> password;
            if ( aParam.Get( KSCPParamPassword, password ) != KErrNone )
                {
                // Nothing to do anymore
                break;
                }            
            
        	// The first rule is to check if securitycode has any
        	// forbidden chars, like WhiteSpace etc...
        	// If so, there is a problem >> KErrArgument
			for (TInt c1=0; c1 < password.Length(); c1++)
			    {
			  TChar ch = static_cast<TChar>( password[c1] );
			  TChar::TCategory chCat = ch.GetCategory();
			  if ( ch.IsSpace() )
				    {   
					TRAPD( err, retParams  = CSCPParamObject::NewL() );
					if ( err == KErrNone )
					    {
					    retParams->Set( KSCPParamStatus, KErrArgument );    
					    }
					    					    
					argumentError = ETrue;
					break;
				    }
			    }			 
						
			// Argument failure, not supposed to be! Out!
        	if (argumentError) break;
        	
        
			// if all required bounds are zero, there is nothing to do.
			if ( passcodemaxlength != 0 || 
				 passcodeminlength != 0 ||
				 passcodemaxrepeatedcharacters != 0 ||
				 passcoderequireupperandlower ||
				 passcoderequirecharsandnumbers || 
				 passcodeconsecutivenumbers ||
				 passcodeminspecialcharacters != 0 ||
				 passcodedisallowsimple)
			    {			
				// Get the filesystem for Resource
				// If fail, bail out
				TInt errgGR = GetResource();
				if (errgGR != KErrNone)
				    {
					errRaised = ETrue;
					break; // Break out 
				    }

				// Declare the Check flags										
				TBool istoosimilar = EFalse;					
				TBool consecutively = EFalse;
				TBool consecutivechars = ETrue;
				TBool singlerepeat = ETrue;
						
				TInt digitCount = 0;	// for passcoderequirecharsandnumbers
				TInt lowerCount = 0;	// for passcoderequireupperandlower
				TInt upperCount = 0;	// for passcoderequireupperandlower
				TInt alphaCount = 0;	// for passcoderequirecharsandnumbers
				TInt specialCount = 0;  // for passcodeminspecialcharacters
				TChar temp = '1';
			

				// Prompt buf, iNote can show only 97 chars,
				// without ... markings.
				HBufC* hbuf = NULL;
				
			 	 // Check for required check or not.
	        	if (passcodeminlength != 0)
	        	    {
		        	// check for Min lenght
		            if (  password.Length() < passcodeminlength )
		                {
		                isInvalid = ETrue;
		                TRAP_IGNORE(
		                    hbuf = LoadAndFormatResL( R_SET_SEC_CODE_MIN, &passcodeminlength ) 
		                    );
		                }
	                }

				if (!hbuf)
				    {
		        	// Check for required check or not.
		        	if (passcodemaxlength!=0)
		        	    {
		        		// Check for Max Lenght
	             	    if ( password.Length() > passcodemaxlength )
		             	    {
		                    isInvalid = ETrue;
		                    TRAP_IGNORE(
		                        hbuf = LoadAndFormatResL( R_SET_SEC_CODE_MAX, &passcodemaxlength );
		                        );
		             	    }
		           	    }
				    }
 	
				if (!hbuf)
				    {
		  			// Check for required check or not.
		  			if ( passcodemaxrepeatedcharacters != 0 )
		  			    {
			  			// Check for TooManySameChars
			  			TRAPD( err, istoosimilar = TooManySameCharsL(password,passcodemaxrepeatedcharacters) );
					  	if ( ( err == KErrNone ) && ( istoosimilar ) )
					  	    {					  		
					  		if ( passcodemaxrepeatedcharacters > 1 )
					  		    {
		                        isInvalid = ETrue;
		                        TRAP_IGNORE(
		                            hbuf = LoadAndFormatResL( R_SET_SEC_CODE_REPEATED,
					  		                              &passcodemaxrepeatedcharacters );
		                            );						  		    						  		    					  		    
					  		    }
					  		else // passcodemaxrepeatedcharacters == 1
					  		    {
		                        isInvalid = ETrue;
		                        TRAP_IGNORE(
		                            hbuf = LoadAndFormatResL( R_SET_SEC_CODE_REPEATED_ONCE );
		                            );					  		   
					  		    }						  								  				  						
					  	    }						  		
				  	    }
				    }
				    
   		   		if (!hbuf)
				    {
	   				// Check for Alphadigit
	   				if ( passcoderequirecharsandnumbers)
	   				    {
	   					for (TInt adcounter=0; adcounter < password.Length(); adcounter++)
			  			    {						
							if ( static_cast<TChar>( password[adcounter] ).IsDigit() ) 
						  		digitCount++;
							
							if ( static_cast<TChar>( password[adcounter] ).IsAlpha() )
						  		alphaCount++;
						    }
						
		  				if (digitCount >= password.Length() || alphaCount >= password.Length()
		  				           || digitCount == 0 || alphaCount == 0 )
		  				    {
		                    isInvalid = ETrue;
		                    TRAP_IGNORE(
		                        hbuf = LoadAndFormatResL( R_SET_SEC_CODE_LETTERS_NUMBERS );
		                        );		  				    				  				
		  				    }
	   				    }	// End of Alphadigit Check
				    }
				  	
				  							
				if (!hbuf)
			    	{
					// passcodeconsecutivenumbers
					if (passcodeconsecutivenumbers)
					    {
						consecutively = consecutivelyCheck(password);

						if ( consecutively )
						    {
		                    isInvalid = ETrue;
		                    TRAP_IGNORE(
		                        hbuf = LoadAndFormatResL( R_SET_SEC_CODE_CONSECUTIVE );
		                        );	
						    }
					    } 
				    }
        
        		/*
        		if (!hbuf)
				    {
		        	// Check for required check or not.
		        	if (passcoderequireupperandlower)
		        	    {				        	
		        		// Count the IsDigits first and miinus them from the lenghth!
		        		// Count the possible NUM's count first!
		        		for (TInt numcounter=0; numcounter < password.Length(); numcounter++)
			  			    {
			  				if ( static_cast<TChar>( password[numcounter] ).IsDigit() )
						  		numberCount++;
			  			    }

		   			// Check for Caps, both
		  			for (TInt capscounter=0; capscounter < password.Length(); capscounter++)
		  			    {
						if ( static_cast<TChar>( password[capscounter] ).IsUpper() )
					  		upperCount++;
						
						if ( static_cast<TChar>( password[capscounter] ).IsLower() )
					  		lowerCount++;
		  			    }
		  			    					  								  			
                        if (upperCount >= (password.Length()-numberCount ) )
                        	isallcaps = ETrue;
				  			
		  				if (lowerCount >= (password.Length() -numberCount) )
		  					isallsmall = ETrue;				  			
				
						if (isallsmall || isallcaps)
						    {	  
		                    isInvalid = ETrue;
		                    TRAP_IGNORE(
		                        hbuf = LoadAndFormatResL( R_SET_SEC_CODE_UPPER_LOWER );
		                        );			                          							    	  							    
						    }	  							    	  							    		
			        	}	// End of Caps check
				    }
				 */	   		   		

        		if (!hbuf)
				    {
		        	// Check for required check or not.
		        	if (passcoderequireupperandlower)
		        	    {
		        	    for (TInt counter=0; counter < password.Length(); counter++)
		  			    {
						if ( static_cast<TChar>( password[counter] ).IsAlpha() )
							{
							if ( static_cast<TChar>( password[counter] ).IsUpper() )
					  		upperCount++;
						
							if ( static_cast<TChar>( password[counter] ).IsLower() )
					  		lowerCount++;
							}
		  			    }
		  			    
		  			    if ( upperCount == 0 || lowerCount == 0)
		  			    	{
		  			    	isInvalid = ETrue;
		                    TRAP_IGNORE(
		                        hbuf = LoadAndFormatResL( R_SET_SEC_CODE_UPPER_LOWER );
		                        );		
		  			    	}
		        	    }
				    }
		        	    
		        	    				        	
        		
        		if (!hbuf)
        		   {
                    // Check for required check or not.
                    if (passcodeminspecialcharacters != 0)
                        {
                        for (TInt specialcounter=0; specialcounter< password.Length(); specialcounter++)
                            {
                            if ( ! (static_cast<TChar>( password[specialcounter] ).IsAlpha() ) )
                                specialCount++;
                            }
                        
                        if ( specialCount < passcodeminspecialcharacters )
                            {
                            isInvalid = ETrue;
                            TRAP_IGNORE(
                                    hbuf = LoadAndFormatResL( R_SET_SEC_CODE_MIN_SPECIAL_CHARS, 
                                    &passcodeminspecialcharacters );
                            );
                            }
                        
                    	}
        		   }
        		                
        		if (!hbuf)
        		   {
                    if (passcodedisallowsimple)
                        {
                        for (TInt counter=0; counter< (password.Length()-1); counter++)
                            {
                            if ( (static_cast<TChar>(password[counter])) != (static_cast<TChar>(password[counter+1])) )
                                {
                                singlerepeat = EFalse;
                                break;
                                }
                            }
                        if (singlerepeat)
                            {
                            isInvalid = ETrue;
                            TRAP_IGNORE(
                                    hbuf = LoadAndFormatResL( R_SET_SEC_CODE_SINGLE_REPEAT ) );                            
                            }
                        if (!hbuf)
                            {
                            for (TInt counter=0; counter< (password.Length()-1); counter++)
                                {
                                //The consecutivity to be checked with only Alphanumeric characters.
                                //The check is being made for only the increasing order. Decreasing order is left for
                                //future implementation if needed. Right now it is left out as this will hit the
                                //usability of device lock a lot.
                                if ( (static_cast<TChar>( password[counter] ).IsAlphaDigit())
                                        && (static_cast<TChar>( password[counter+1] ).IsAlphaDigit()))
                                    {
                                    if ( (static_cast<TChar>(password[counter+1])) - (static_cast<TChar>(password[counter])) != 1 )                      
                                        {
                                        consecutivechars = EFalse;
                                        break;
                                        }
                                    }
                                else
                                    {
                                    consecutivechars = EFalse;
                                    break;
                                    }
                                }
                            if (consecutivechars)
                                {
                                isInvalid = ETrue;
                                TRAP_IGNORE(
                                        hbuf = LoadAndFormatResL( R_SET_SEC_CODE_CONSECUTIVE_CHARS ) );
                                }
                            }
                        }
                    }
				  	
			  	
				if ( isInvalid )
				    {	    							
			    	// Create the result-object to return
				    TRAPD( err, retParams  = CSCPParamObject::NewL() );
                    
                    if ( err == KErrNone )
				        {
			            retParams->Set( KSCPParamStatus, KErrSCPInvalidCode );
    		            retParams->Set( KSCPParamAction, KSCPActionShowUI );
	    	            retParams->Set( KSCPParamUIMode, KSCPUINote );
		                
		                if ( hbuf != NULL )
		                    {
		                    FormatResourceString(*hbuf);	
		                    TPtr ptr = hbuf->Des();
		                    retParams->Set( KSCPParamPromptText, ptr );
		                    delete hbuf;
		                    }
				        }
				    }
				    
                } // end of All Zero check
		    // All params were zero! no check!
            else 
                {
                retParams = NULL;
                }
                			
            break;  
            } // KSCPEventValidate
                
        case ( KSCPEventConfigurationQuery ):
            {            
            TInt paramID = -1; 
            
            // Get the ID from the paramObject      
            if ( aParam.Get( KSCPParamID, paramID ) != KErrNone )
                {
                // Nothing to do anymore
                break;
                }            
            
            if ( paramID == (RTerminalControl3rdPartySession::EPasscodeMinLength)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeMaxLength)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeMaxRepeatedCharacters)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeConsecutiveNumbers )
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeMinSpecialCharacters)
				||  paramID ==  (RTerminalControl3rdPartySession::EPasscodeDisallowSimple))
                {
                
                // OK, we're interested, check that the value is valid
                TRAPD( err, retParams  = CSCPParamObject::NewL() );
                
                if ( err != KErrNone )
                    {
                    break; // Fatal, cannot create paramObject
                    }
                
                // All of our params are TInts
                TInt paramValue;
                if ( aParam.Get( KSCPParamValue, paramValue ) != KErrNone )
                    {
                    retParams->Set( KSCPParamStatus, KErrGeneral );
                    break;
                    }
                                
                TInt retStatus = KErrNone;
                switch ( paramID )
                    {
                        case ( RTerminalControl3rdPartySession::EPasscodeMinLength ):
                        
                        	{
                        		TInt passcodemaxlength;  
                        		CSCPParamObject& config = iEventHandler->GetParameters();
                        		if ( config.Get( ( RTerminalControl3rdPartySession::EPasscodeMaxLength), 
                                             passcodemaxlength ) !=  KErrNone )
                                             passcodemaxlength = KSCPPasscodeMaxLength;
                                             
                        		if ( ( paramValue < KSCPPasscodeMinLength ) || 
                            	   ( paramValue > KSCPPasscodeMaxLength )|| 
                            	   ( paramValue > passcodemaxlength ) )
                        		{
                        			retStatus = KErrArgument;
                        		}
                        	}
                        break;
                        
                        case ( RTerminalControl3rdPartySession::EPasscodeMaxLength ):
                           
                           {
                           	    TInt passcodeminlength;  
                           	    CSCPParamObject& config = iEventHandler->GetParameters();
                           	    if ( config.Get( ( RTerminalControl3rdPartySession::EPasscodeMinLength), 
                                             passcodeminlength ) !=  KErrNone )
                                             passcodeminlength = KSCPPasscodeMinLength;
                                             
                                             
                        		if ( ( paramValue < KSCPPasscodeMinLength ) || 
                            	   ( paramValue > KSCPPasscodeMaxLength )|| 
                            	   ( paramValue < passcodeminlength ) )
                        		{
                        			retStatus = KErrArgument;
                        		}
                           }
                        break;
                        // Flow through: similar values
                        case ( RTerminalControl3rdPartySession::EPasscodeRequireUpperAndLower ):
                        case ( RTerminalControl3rdPartySession::EPasscodeConsecutiveNumbers ):
                        case ( RTerminalControl3rdPartySession::EPasscodeRequireCharsAndNumbers ):
                        case ( RTerminalControl3rdPartySession::EPasscodeDisallowSimple ):
                            if ( ( paramValue < 0 ) || 
                                 ( paramValue > 1 ) )                                 
                                {
                                // This is not a valid value
                                retStatus = KErrArgument;
                                }
                        break;

                        case ( RTerminalControl3rdPartySession::EPasscodeMaxRepeatedCharacters ):
                            if ( ( paramValue < 0 ) || 
                                 ( paramValue > KSCPMaxRepeatAmount ) ) 
                                {
                                // This is not a valid value
                                retStatus = KErrArgument;
                                }
                        break;
                        
                        case ( RTerminalControl3rdPartySession::EPasscodeMinSpecialCharacters ):
                             if ( ( paramValue < 0 ) ||
                                  ( paramValue > KSCPMinSpecialAmount ) )
                                 {
                                 // This is not a valid value
                                 retStatus = KErrArgument;
                                 }
                        break;
                    }
                
                retParams->Set( KSCPParamStatus, retStatus );
                }
            else
                {
                retParams = NULL;
                }
               
            break;
            } //KSCPEventConfigurationQuery                         
        } //  switch ( aID )

   	// Check if Any errors were raised and handle it
    if (errRaised) 
        {
        if ( retParams != NULL )
            {
            delete retParams;
            }
        retParams = NULL;
        }
        
    // The caller will own this pointer from now on   
    return retParams; 
	}

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::SetEventHandler
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPPatternPlugin::SetEventHandler( MSCPPluginEventHandler* aHandler )
	{
	Dprint( ( _L( "CSCPPatternPlugin::SetEventHandler()" ) ) ) ;
	iEventHandler = aHandler;
	
	iFs = &(iEventHandler->GetFsSession());
	}	



// ========================= OTHER INTERNAL FUNCTIONS =========================

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::TooManySameCharsL
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
TBool CSCPPatternPlugin::TooManySameCharsL ( TDes& aParam, TInt  aMaxRepeatedCharacters)
    {
	Dprint( ( _L( "CSCPPatternPlugin::TooManySameChars()" ) ) );
             
    // Make a copy from param
    // find the first's index's char from the param.
    // so that the found char get counter incresed and gets 
    // replaced with KNullDesC

    // Make copy from securitypass
    HBufC* hbuf = HBufC::NewLC( aParam.Length());
    hbuf->Des().Copy( aParam );
    	
	// Declare the loacl vars.
	TChar firstChar; 		// char to search for
	TInt charIndex = 0;		// locate() index
	TInt found = 0;			// Count the specific char's occurenaces
	TBool flag = EFalse;	// if there is a char's too maney, break out.
	TBool sameCharTooMany = EFalse;
	
	while (hbuf->Des().Length() >= 1 )
		{
		// Get the first char 
		firstChar = hbuf->Des()[0];
			while (charIndex != KErrNotFound)
			{
					charIndex = hbuf->Des().Locate(firstChar);
				    if (charIndex != KErrNotFound)
				    {
				    	hbuf->Des().Replace( charIndex, 1, KNullDesC );
				    	charIndex=0;
				    	found++;

					    // So the First char is loopt through and deleted from buf
						// Check now for immidate fail case
					    if (found > aMaxRepeatedCharacters )
					    {
					    	flag = ETrue;
					    	break;
					    }
				    }
				    else if (charIndex == KErrNotFound)
				    {
				    	// Reset the variables to Reuse.
				    	charIndex = 0;
				    	found = 0;
				    	break;
				    } // End of IF
				} // end of While
					

				if (flag)
				{
					// Rule TooManyChars occured. OUT!
					sameCharTooMany = ETrue;
					break;
				}
  	  	}// End of While

  	// Delete the hbuf
  	CleanupStack::PopAndDestroy( hbuf );
    return sameCharTooMany;
    }

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::consecutivelyCheck
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
TBool CSCPPatternPlugin::consecutivelyCheck ( TDes& aParam )
    {
	Dprint( ( _L( "CSCPPatternPlugin::consecutivelyCheck()" ) ) );
    TBool charTooManyInRow = EFalse;
	TInt counter = 0;
	
    while ( counter < (aParam.Length()-1 ) )
        {
    	if ( ( static_cast<TChar>( aParam[counter] ).IsDigit() ) &&
    		  ( static_cast<TChar>( aParam[counter+1] ).IsDigit() ) ) 
    	    {
    	    charTooManyInRow = ETrue;
    		break;				
    	    }
    	    
    	counter++;
        }
        
    return charTooManyInRow;
    }
    

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::LoadAndFormatResL
// Load the given resouce, and format the string according to the TInt parameters
// if given.
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
HBufC* CSCPPatternPlugin::LoadAndFormatResL( TInt aResId, TInt* aParam1, TInt* aParam2 )
    {
    HBufC16* resource = NULL;
    HBufC* hbuf = NULL;
    
    resource = LoadResourceLC( aResId );
    FormatResourceString(*resource);
    TInt allocLen = 0;
    if ( aParam1 != NULL )
        {
        allocLen += KSCPMaxIntLength;
        }
    if ( aParam2 != NULL )
        {
        allocLen += KSCPMaxIntLength;
        }
                
	hbuf = HBufC::NewL( resource->Length() + allocLen );
	
	if ( ( aParam1 == NULL ) && ( aParam2 == NULL ) )
	    {
	    hbuf->Des().Copy( resource->Des() );
	    }
	else
	    {
	    if ( aParam1 == NULL )
	        {
	        hbuf->Des().Format( resource->Des(), *aParam2 );
	        }
	    else if ( aParam2 == NULL )
	        {
	        hbuf->Des().Format(resource->Des(), *aParam1 );
	        }
	    else
	        {
	        hbuf->Des().Format(resource->Des(), *aParam1, *aParam2 );
	        }	    
	    }
								    
	CleanupStack::PopAndDestroy( resource );
	return hbuf;
    }

// ----------------------------------------------------------------------------
// CSCPPatternPlugin::FormatResourceString
// The buffer that is passed is formatted to have only %i as a format specifier instead of %N or %0N etc.
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
void CSCPPatternPlugin::FormatResourceString(HBufC16 &aResStr)
{
		TInt pos = 0;
		TInt flag = 0;
        TPtr16 bufPtr = aResStr.Des();
        _LIT (mess1, "%N");
        _LIT (mess2, "%i");
        _LIT (mess3, "%0N");
        _LIT (mess4, "%1N");
                              
        while ((pos = bufPtr.Find(mess1)) !=KErrNotFound)
        {
              bufPtr.Replace(pos,2,mess2); 
              flag = 1;
              break;                    
        }
               
        if(flag == 0)
        {
              while ((pos = bufPtr.Find(mess3)) != KErrNotFound)
              {
              		bufPtr.Replace(pos,3,mess2);
              }
               		
              while ((pos = bufPtr.Find(mess4)) != KErrNotFound)
              {
                	bufPtr.Replace(pos,3,mess2);
              }
        }	
}    

// End of File
