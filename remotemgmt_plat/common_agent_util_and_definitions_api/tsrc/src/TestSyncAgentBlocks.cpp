/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains all functions that can be called from cfg file.
*				 These functions call the necessary methods for getting the operation done and 
*				 writes the output to the Log.
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "TestSyncAgent.h"
#include "nsmlunicodeconverter.h"
#include <utf.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestSyncAgent::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestSyncAgent::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTestSyncAgent::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestSyncAgent::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        //ENTRY( "Example", CTestSyncAgent::ExampleL ),
        ENTRY( "GetSoftwareVersion", CTestSyncAgent::GetSoftwareVersion ),
        ENTRY( "GetPhoneData", CTestSyncAgent::GetPhoneDataL ),
        ENTRY( "UnicodeToUTF8", CTestSyncAgent::UnicodeToUTF8L ),
        ENTRY( "UTF8ToUnicode", CTestSyncAgent::UTF8ToUnicodeL ),
        //ADD NEW ENTRY HERE

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CTestSyncAgent::GetSoftwareVersion
// Method to print the Software Version of the Phone.
// -----------------------------------------------------------------------------
//
TInt CTestSyncAgent::GetSoftwareVersion( CStifItemParser& aItem )
{    	
		TPtrC8 version;
		//Getting Software Version and traping the same as it is a leaving function.
		TRAPD( result, version.Set( iPhoneInfo->SwVersionL() ) );
		if( result == KErrNone )
		{
	    iLog->Log( _L("Software Version is = ") );
	    iLog->Log( version );
	    return KErrNone;
		}
		else
		{
			iLog->Log( _L("Result: Failed with Error Code: %d"), result );
			return result;
		}
}

// -----------------------------------------------------------------------------
// CTestSyncAgent::GetPhoneData
// Method to print the Phone Data Information.
// -----------------------------------------------------------------------------
//
TInt CTestSyncAgent::GetPhoneDataL( CStifItemParser& aItem )
{
    iLog->Log( _L("Phone Data: ") );

		const TInt KMaxBufSize = 50;
		HBufC* phoneData = HBufC::NewLC( KMaxBufSize );
    TPtr phoneDataPtr = phoneData->Des(); 
    
    //Phone Manufacturer Info
    TRAPD( result1, iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneManufacturer, phoneDataPtr ) );
    if( result1 == KErrNone )
    {
	    iLog->Log( _L("Phone Manufacturer is = ") );
			iLog->Log( phoneDataPtr );
    }
		else
		{
	    iLog->Log( _L("Not able to get Phone Manufacturer Info, Error value is = %d"), result1 );
		}
  	
    //Phone Model ID Info
    TRAPD( result2, iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneModelId, phoneDataPtr ) );
    if( result2 == KErrNone )
    {
	    iLog->Log( _L("Phone Model ID is = ") );
			iLog->Log( phoneDataPtr );
    }
		else
		{
	    iLog->Log( _L(" Not able to get Phone Model ID Info, Error value is = %d"), result2 );
		}
   
    //Phone Revision Id Info
    TRAPD( result3, iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneRevisionId, phoneDataPtr ) );
    if( result3 == KErrNone )
    {
	    iLog->Log( _L("Phone Revision ID is = ") );
			iLog->Log( phoneDataPtr );
    }
		else
		{
	    iLog->Log( _L(" Not able to get Phone Revision ID Info, Error value is = %d"), result3 );
		}
		 
    //Phone Serial Number
    TRAPD( result4, iPhoneInfo->PhoneDataL( CNSmlPhoneInfo::EPhoneSerialNumber, phoneDataPtr ) );
    if( result4 == KErrNone )
    {
	    iLog->Log( _L("Phone Serial Number is = ") );
			iLog->Log( phoneDataPtr );
    }
		else
		{
	    iLog->Log( _L(" Not able to get Phone Serial Number, Error value is = %d"), result4 );
		}
		
		//Pops item from CleanUpStack, pushed by methods ending from LC
    CleanupStack::PopAndDestroy( phoneData );
    
  	//Verifying pass/fail criteria for all the above executed conditions
   	if( ( result1 | result2 | result3 | result4 ) == KErrNone )
   	{
   		return KErrNone; 
   	}
		else	//make it fail
		{		
			return KErrGeneral;	
		}		
}
  	
// -----------------------------------------------------------------------------
// CTestSyncAgent::UnicodeToUTF8
// Method to Convert the data from Unicode to UTF8 format.
// -----------------------------------------------------------------------------
//
TInt CTestSyncAgent::UnicodeToUTF8L( CStifItemParser& aItem )
{
    //Data in UTF8
    _LIT(KDataInUTF8, "NokiaNokia");
    TBuf8<10> dataInUTF;
    dataInUTF.Copy(KDataInUTF8);
    
    //Converting data to Unicode using function "ConvertToUnicodeFromUtf8" of 
    //"CnvUtfConverter" class, to provide the Unicode data as an input for 
    //"HBufC8InUTF8LC" function (This function needs to be called for testing)
    TBuf<10> dataInUnicode;
    CnvUtfConverter::ConvertToUnicodeFromUtf8(dataInUnicode, dataInUTF);
    
    //Data in Unicode after Conversion
    iLog->Log( _L("Data in Unicode is:") );
    iLog->Log( dataInUnicode );    
    TInt sizeOfUnicodeData = dataInUnicode.Size();
    iLog->Log( _L("size of Unicode data: %d "), sizeOfUnicodeData );
		
		//Converting to UTF8 format
		HBufC8* dataInUTF8AfterConversion  = NULL;
    NSmlUnicodeConverter::HBufC8InUTF8LC( dataInUnicode, dataInUTF8AfterConversion );	
    
    //UTF8 data after conversion
    iLog->Log( _L("Data in UTF8 is:") );
    iLog->Log( *dataInUTF8AfterConversion );		
    TInt sizeOfUTF8Data = dataInUTF8AfterConversion->Size();
		iLog->Log( _L("size of UTF8 data: %d "), sizeOfUTF8Data );
		
    //Comparing the Converted data with Original data
    TInt compResult = dataInUTF8AfterConversion->Compare(dataInUTF);		
		
		CleanupStack::PopAndDestroy( dataInUTF8AfterConversion );
		
		//Comparing the size and also Verifying the result
    if ( ( ( sizeOfUTF8Data * 2 ) == sizeOfUnicodeData )  & !compResult )
    {
    	iLog->Log( _L("Data Converted from Unicode to UTF8 Sucessfully") );
    	return KErrNone;
    }
    else	//make it fail
    {
    	iLog->Log( _L("Data not Converted Sucessfully") );
    	return KErrGeneral;	
    }
}  	

// -----------------------------------------------------------------------------
// CTestSyncAgent::UTF8ToUnicode
// Method to Convert the data from UTF8 to Unicode format.
// -----------------------------------------------------------------------------
//
TInt CTestSyncAgent::UTF8ToUnicodeL( CStifItemParser& aItem )
{ 
		//Buffer to store converted data
	  HBufC* dataInUnicode = NULL;
	  //Data to convert from UTF8 to Unicode
	  _LIT(KDataInUTF8, "NokiaNokia");
    TBuf8<10> dataInUTF8;
    dataInUTF8.Copy(KDataInUTF8);    		
		
		//Calling "HBufC16InUnicodeL" function to convert data
    NSmlUnicodeConverter::HBufC16InUnicodeL( dataInUTF8, dataInUnicode );	
   
   	//Printing UTF8 data and size to Log
   	iLog->Log( _L("Data in UTF8 Format: ") );
    iLog->Log( dataInUTF8 );
    TInt sizeOfUTF8Data = dataInUTF8.Size();
    iLog->Log( _L("size of UTF8 data: %d "), sizeOfUTF8Data );
    
    //Printing Unicode (converted) data and size to Log
   	iLog->Log( _L("Data in UniCode Format: ") );
    iLog->Log( *dataInUnicode );
    TPtrC16 dataInUnicodePtr = dataInUnicode->Des();
    TInt sizeOfUnicodeData = dataInUnicodePtr.Size();
    iLog->Log( _L("size of Unicode: %d "), sizeOfUnicodeData );
    
    //Comparing the Converted data with Original data
    TPtrC16 ptrToDataInUTF8(KDataInUTF8);
    TInt compResult = ptrToDataInUTF8.Compare(dataInUnicodePtr);
    
    //Comparing the size and also Verifying the result
    if ( ( ( sizeOfUTF8Data * 2 ) == sizeOfUnicodeData )  & !compResult )
    {
    	iLog->Log( _L("Data Converted from UTF8 to Unicode Sucessfully") );
    	return KErrNone;
    }
    else
    {
    	iLog->Log( _L("Data not Converted Sucessfully") );
    	return KErrGeneral;
    }
}  	
	  	
//  End of File
