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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "testCSCPParamObject.h"

#include <SCPParamObject.h>
#include <SCPClient.h>
const TUint8 KMMCDriveLetter = 'e';
_LIT( MDriveColon, ":");
_LIT16(KFileName1,"\\testing\\data\\abc.txt");
_LIT16(KFileName2,"\\testing\\data\\config.txt");

//#include <SCPDebug.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CtestCSCPParamObject::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CtestCSCPParamObject::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CtestCSCPParamObject::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CtestCSCPParamObject::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove


        ENTRY( "NewL", CtestCSCPParamObject::NewL ),
        ENTRY( "NewLC", CtestCSCPParamObject::NewLC ),
        ENTRY( "DesCSCPParamObject", CtestCSCPParamObject::DesCSCPParamObject ),
        ENTRY( "GetParamLength", CtestCSCPParamObject::GetParamLengthL ),
        ENTRY( "GetParamIDDes", CtestCSCPParamObject::GetParamIDDesL ),
        ENTRY( "GetParamIDTInt", CtestCSCPParamObject::GetParamIDTIntL ),
        ENTRY( "SetParamIDDes", CtestCSCPParamObject::SetParamIDDesL),
        ENTRY( "SetParamIDTInt", CtestCSCPParamObject::SetParamIDTIntL ),
        ENTRY( "Unset", CtestCSCPParamObject::UnsetL ),
        ENTRY( "Reset", CtestCSCPParamObject::ResetL ),
        ENTRY( "Parse", CtestCSCPParamObject::ParseL ),
        ENTRY( "GetBuffer", CtestCSCPParamObject::GetBufferL ),
        ENTRY( "WriteToFileL", CtestCSCPParamObject::WriteToFileL ),
        ENTRY( "ReadFromFileL", CtestCSCPParamObject::ReadFromFileL ),
                
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KtestCSCPParamObject, "testCSCPParamObject" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KtestCSCPParamObject, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KtestCSCPParamObject, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::NewL(CStifItemParser& aItem)
{	
	__UHEAP_MARK;
	CSCPParamObject* pObject = CSCPParamObject::NewL();
	delete pObject;
	__UHEAP_MARKEND;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::NewLC(CStifItemParser& aItem)
{
	__UHEAP_MARK;
	CSCPParamObject* pObject = CSCPParamObject::NewLC();
	CleanupStack::PopAndDestroy(pObject);
	__UHEAP_MARKEND;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::DesCSCPParamObject(CStifItemParser& aItem)
{
	__UHEAP_MARK;
	CSCPParamObject* pObject = NULL;
	TRAPD(res, pObject = CSCPParamObject::NewL());
	if(res)
	{
		return KErrGeneral;
	}
	TRAPD(res2, delete pObject);
	if(res2)
	{
		return KErrGeneral;
	}
	__UHEAP_MARKEND;
	return KErrNone; 
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::GetParamLengthL(CStifItemParser& aItem)
{
	RArray<TInt> paramIdArray;
	RArray<TInt> valueArray;
	TInt paramId, value;
	TInt err1, err2 ;
	TBool NextArg = TRUE;
	
	TInt count;
	for ( count = 0; NextArg; count++)
	{
	 	err1 = aItem.GetNextInt( paramId);
    	err2 = aItem.GetNextInt( value);

	  	if(err1 || err2)
	  	{
	  		NextArg = false;
	  		break; 
	  	}	
	  	paramIdArray.Append(paramId);
	  	valueArray.Append(value);
	}
  
  	if( !count)
  		return KErrNotFound;
  	
  	TBuf<KSCPMaxIntLength> convBuf;
    
  	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	TInt ret;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		convBuf.Zero();	    
	    convBuf.AppendNum( valueArray[count] );
	    
		ret = pObject->Set(paramIdArray[count], convBuf);   
		if(ret)
		{
			delete pObject;
			return ret;
		}
	}


	//TBuf <KSCPMaxIntLength> retValue;
		
	TInt length;
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		//retValue.Zero();
		length = pObject->GetParamLength(paramIdArray[count]);
		
		TBuf<KSCPMaxIntLength> convBuf;
    
    	convBuf.Zero();
    	convBuf.AppendNum(valueArray[count]);
		
		TInt actualLength = convBuf.Length();
		if( actualLength != length)
		{
			delete pObject;
			return KErrGeneral; // length not equal
		}	
	}
	
	if( KErrNone != pObject->Reset())
	{
		delete pObject;
		return KErrGeneral;
	}
	length = pObject->GetParamLength(paramIdArray[0]);
	if(0 <= length)
	{
		delete pObject;
		return KErrGeneral;
	}
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::GetParamIDDesL(CStifItemParser& aItem)
{
	RArray<TInt> paramIdArray;
	RArray<TInt> valueArray;
	TInt paramId, value;
	TInt err1, err2 ;
	TBool NextArg = TRUE;
	
	TInt count;
	for ( count = 0; NextArg; count++)
	{
	 	err1 = aItem.GetNextInt( paramId);
    	err2 = aItem.GetNextInt( value);

	  	if(err1 || err2)
	  	{
	  		NextArg = false;
	  		break; // continue; // 
	  		//return KErrNotFound;
	  	}	
	  	paramIdArray.Append(paramId);
	  	valueArray.Append(value);
	}
  
  	if( !count)
  		return KErrNotFound;
  	
  	TBuf<KSCPMaxIntLength> convBuf;
    
  	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	TInt ret;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		convBuf.Zero();	    
	    convBuf.AppendNum( valueArray[count] );
	    
		ret = pObject->Set(paramIdArray[count], convBuf);   
		if(ret)
		{
			delete pObject;
			return ret;
		}
	}


	TBuf <KSCPMaxIntLength> retValue;
		
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		retValue.Zero();
		ret = pObject->Get(paramIdArray[count], retValue);
		if(ret)
		{
			delete pObject;
			return ret;
		}
		TBuf<KSCPMaxIntLength> convBuf;
    
    	convBuf.Zero();
    	convBuf.AppendNum(valueArray[count]);
		if( retValue.Compare(convBuf) )
		{
			delete pObject;
			return KErrGeneral;
		}
	}
	
	TInt Dummy = 1111;
	count = 2;
	TInt existingValue;
	
	pObject->Get(paramIdArray[count], existingValue);
	convBuf.Zero();
	convBuf.AppendNum(Dummy);
	pObject->Set(paramIdArray[count], convBuf);
		
	TInt value1;
	pObject->Get(paramIdArray[count], value1);
	
	if(value1 == existingValue || value1 != Dummy)
	{
		delete pObject;
		return KErrGeneral;
	}
	delete pObject;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::GetParamIDTIntL(CStifItemParser& aItem)
{
	RArray<TInt> paramIdArray;
	RArray<TInt> valueArray;
	TInt paramId, value;
	TInt err1, err2 ;
	TBool NextArg = TRUE;
	
	TInt count;
	for ( count = 0; NextArg; count++)
	{
	 	err1 = aItem.GetNextInt( paramId);
    	err2 = aItem.GetNextInt( value);

	  	if(err1 || err2)
	  	{
	  		NextArg = false;
	  		break; // continue; // 
	  		//return KErrNotFound;
	  	}	
	  	paramIdArray.Append(paramId);
	  	valueArray.Append(value);
	}
  
  	if( !count)
  		return KErrNotFound;
  
	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	TInt ret;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		ret = pObject->Set(paramIdArray[count], valueArray[count]);   
		if(ret)
		{
			delete pObject;
			return ret;
		}
	}
	
	
	TInt retValue;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		ret = pObject->Get(paramIdArray[count], retValue);
		if(ret)
		{
			delete pObject;
			return ret;
		}
		if(valueArray[count] != retValue)
		{
			delete pObject;
			return KErrGeneral;
		}
	}
	
	delete pObject;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::SetParamIDDesL(CStifItemParser& aItem)
{

	RArray<TInt> paramIdArray;
	RArray<TInt> valueArray;
	TInt paramId, value;
	TInt err1, err2 ;
	TBool NextArg = TRUE;
	
	TInt count;
	for ( count = 0; NextArg; count++)
	{
	 	err1 = aItem.GetNextInt( paramId);
    	err2 = aItem.GetNextInt( value);

	  	if(err1 || err2)
	  	{
	  		NextArg = false;
	  		break; // continue; // 
	  		//return KErrNotFound;
	  	}	
	  	paramIdArray.Append(paramId);
	  	valueArray.Append(value);
	}
  
  	if( !count)
  		return KErrNotFound;
  	
  	TBuf<KSCPMaxIntLength> convBuf;
    
  	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	TInt ret;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		convBuf.Zero();	    
	    convBuf.AppendNum( valueArray[count] );
	    
		ret = pObject->Set(paramIdArray[count], convBuf);   
		if(ret)
		{
			delete pObject;
			return ret;
		}
	}

	TInt retValue;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		ret = pObject->Get(paramIdArray[count], retValue);
		if(ret)
		{
			delete pObject;
			return ret;
		}
		if(valueArray[count] != retValue)
		{
			delete pObject;
			return KErrGeneral;
		}
	}
	
	TInt Dummy = 1111;
	count = 2;
	TInt existingValue;
	
	pObject->Get(paramIdArray[count], existingValue);
	convBuf.Zero();
	convBuf.AppendNum(Dummy);
	pObject->Set(paramIdArray[count], convBuf);
		
	TInt value1;
	pObject->Get(paramIdArray[count], value1);
	
	if(value1 == existingValue || value1 != Dummy)
	{
		delete pObject;
		return KErrGeneral;
	}
	delete pObject;
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::SetParamIDTIntL(CStifItemParser& aItem)
{
	TInt paramId;
	TInt value;	
    TInt err1 = aItem.GetNextInt( paramId);
    TInt err2 = aItem.GetNextInt( value);
  
  	if(err1 || err2)
  	{
  		return KErrNotFound;
  	}
  
	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	TInt ret = pObject->Set(paramId, value);   
	
	if(ret)
	{
		delete pObject;
		return ret;
	}
	
	TInt retValue;
	ret = pObject->Get(paramId, retValue);
	if(ret)
	{
		delete pObject;
		return ret;
	}
	if(value != retValue)
	{
		delete pObject;
		return KErrGeneral;
	}
	
	// Setting same paramId, overwrite value;
	value = 111;
	ret = pObject->Set(paramId, value);   
	
	if(ret)
	{
		delete pObject;
		return ret;
	}
	
	ret = pObject->Get(paramId, retValue);
	if(ret)
	{
		delete pObject;
		return ret;
	}
	
	if(retValue != value)
	{
		delete pObject;
		return KErrNotFound;
	}
	
	// Trying to test non-existing paramid, expected return error
	paramId = 999;
	
	ret = pObject->Get(paramId, retValue);
	if(!ret)
	{
		delete pObject;
		return ret;
	}
	delete pObject;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::UnsetL(CStifItemParser& aItem)
{
	RArray<TInt> paramIdArray;
	RArray<TInt> valueArray;
	TInt paramId, value;
	TInt err1, err2 ;
	TBool NextArg = TRUE;
	
	TInt count;
	for ( count = 0; NextArg; count++)
	{
	 	err1 = aItem.GetNextInt( paramId);
    	err2 = aItem.GetNextInt( value);

	  	if(err1 || err2)
	  	{
	  		NextArg = false;
	  		break; 
	  	}	
	  	paramIdArray.Append(paramId);
	  	valueArray.Append(value);
	}
  
  	if( !count)
  		return KErrNotFound;
  
	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	TInt ret;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		ret = pObject->Set(paramIdArray[count], valueArray[count]);   
		if(ret)
		{
			delete pObject;
			return ret;
		}
	}
	
	
	TInt retValue;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		ret = pObject->Get(paramIdArray[count], retValue);
		if(ret)
		{
			delete pObject;
			return ret;
		}
		if(valueArray[count] != retValue)
		{
			delete pObject;
			return KErrGeneral;
		}
	}

	TInt paramIndex = 3;
	TInt presentValue;
	ret = pObject->Get(paramIdArray[paramIndex], presentValue);
	if(ret)
	{
		delete pObject;
		return KErrGeneral; // Get failed
	}
	
	retValue = pObject->Unset(paramIdArray[paramIndex]);
	if(retValue)
	{
		delete pObject;
		return retValue; // Unset failed
	}
	
	TInt zero = 0;
	retValue = zero;
	ret = pObject->Get(paramIdArray[paramIndex], retValue);
	if(KErrNone == ret || presentValue == retValue)
	{
		delete pObject;
		return ret;
	}
	
	delete pObject;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::ResetL(CStifItemParser& aItem)
{
	RArray<TInt> paramIdArray;
	RArray<TInt> valueArray;
	TInt paramId, value;
	TInt err1, err2 ;
	TBool NextArg = TRUE;
	
	TInt count;
	for ( count = 0; NextArg; count++)
	{
	 	err1 = aItem.GetNextInt( paramId);
    	err2 = aItem.GetNextInt( value);

	  	if(err1 || err2)
	  	{
	  		NextArg = false;
	  		break; // continue; // 
	  		//return KErrNotFound;
	  	}	
	  	paramIdArray.Append(paramId);
	  	valueArray.Append(value);
	}
  
  	if( !count)
  		return KErrNotFound;
  
	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	TInt ret;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		ret = pObject->Set(paramIdArray[count], valueArray[count]);   
		if(ret)
		{
			delete pObject;
			return ret;
		}
	}
	
	
	TInt retValue;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		ret = pObject->Get(paramIdArray[count], retValue);
		if(ret)
		{
			delete pObject;
			return ret;
		}
		if(valueArray[count] != retValue)
		{
			delete pObject;
			return KErrGeneral;
		}
	}

	retValue = pObject->Reset();
	if( KErrNone != retValue )
	{
		delete pObject;
		return KErrGeneral; // Reset failed
	}
	TInt length = pObject->GetParamLength(paramIdArray[0]);
	if(0 <= length)
	{
		delete pObject;
		return KErrGeneral;
	}
	paramIdArray[0] = 15;
	retValue = pObject->Set(paramIdArray[0], 1111);	
	if(retValue)
	{
		delete pObject;
		return KErrGeneral; // Set failed
	}
	
	length = pObject->GetParamLength(paramIdArray[0]);
	if(4 != length)
	{
		delete pObject;
		return KErrGeneral;  // GetParamLength failed
	}
	

	retValue = pObject->Reset();
	if( KErrNone != retValue )
	{
		delete pObject;
		return KErrGeneral; // Reset failed
	}
	
	length = pObject->GetParamLength(paramIdArray[0]);
	if(0 <= length)
	{
		delete pObject;
		return KErrGeneral;  // Reset and GetParamLength failed
	}
	
	delete pObject;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::ParseL(CStifItemParser& aItem)
{
	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	//TDes8& buffer = ;
	_LIT8(KaBuffer,"");
	TBuf8<25> buffer( KaBuffer );
	
	TInt retValue = pObject->Parse(buffer);
	
	if(KErrNone != retValue)
	{
		delete pObject;
		return KErrGeneral;
	}

	RArray<TInt> paramIdArray;	
	TInt paramId;
	TInt count = 0;
	TBool NextArg = TRUE;
	TInt err;
	
	paramIdArray.Append(count);
	  	
	for ( ; NextArg; count++)
	{
	 	err = aItem.GetNextInt( paramId);

	  	if(err)
	  	{
	  		NextArg = false;
	  		break; 
	  	}	
	  	paramIdArray.Append(paramId);
	}
  
  	if( !count)
  		return KErrNotFound;
  
  	paramIdArray[0] = count;
  	
 // 	TBuf<TInt> buffer[count];  	
 /*	TBuf buffer(paramIdArray.count() * sizeof(TInt));
  	TBuf value;
  	
  	for (count=0; count < paramIdArray.count(); count++)
  	{
  		value.Num(paramIdArray[count]);
  		buffer.Append(value);
  	}
  	
*/	
/*
	CDesC8Array buffer;
	TBuf value;
	for(count=0; count < paramIdArray.count(); count++)
	{
		value.Num(paramIdArray[count]);
		buffer.AppendL(value);
	}
*/
	delete pObject;
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::GetBufferL(CStifItemParser& aItem)
{
	RArray<TInt> paramIdArray;
	RArray<TInt> valueArray;
	TInt paramId, value;
	TInt err1, err2 ;
	TBool NextArg = TRUE;
	TInt ret;
	
  	CSCPParamObject* pObject = CSCPParamObject::NewL();
  	HBufC8* buffer = NULL;
	ret = pObject->GetBuffer(buffer);
	if(ret)
	{
		delete pObject;
		return ret;
	}
	
	
	TInt count;
	for ( count = 0; NextArg; count++)
	{
	 	err1 = aItem.GetNextInt( paramId);
    	err2 = aItem.GetNextInt( value);

	  	if(err1 || err2)
	  	{
	  		NextArg = false;
	  		break; // continue; // 
	  		//return KErrNotFound;
	  	}	
	  	paramIdArray.Append(paramId);
	  	valueArray.Append(value);
	}
  
  	if( !count)
  		return KErrNotFound;
  	
  	TBuf<KSCPMaxIntLength> convBuf;
    
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		convBuf.Zero();	    
	    convBuf.AppendNum( valueArray[count] );
	    
		ret = pObject->Set(paramIdArray[count], convBuf);   
		if(ret)
		{
			delete pObject;
			return ret;
		}
	}

	ret = pObject->GetBuffer(buffer);
	if(ret)
	{
		delete pObject;
		return ret;
	}
	if(NULL == buffer)
	{
		delete pObject;
		return ret;
	}
	delete pObject;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::WriteToFileL(CStifItemParser& aItem)
{
	RArray<TInt> paramIdArray;
	RArray<TInt> valueArray;
	TInt paramId, value;
	TInt err1, err2 ;
	TBool NextArg = TRUE;
	
	TInt count;
	for ( count = 0; NextArg; count++)
	{
	 	err1 = aItem.GetNextInt( paramId);
    	err2 = aItem.GetNextInt( value);

	  	if(err1 || err2)
	  	{
	  		NextArg = false;
	  		break; // continue; // 
	  		//return KErrNotFound;
	  	}	
	  	paramIdArray.Append(paramId);
	  	valueArray.Append(value);
	}
  
  	if( !count)
  		return KErrNotFound;
  
	CSCPParamObject* pObject = CSCPParamObject::NewL();
	
	TInt ret;
	
	for(count = 0; paramIdArray.Count() != count; count++)
	{
		ret = pObject->Set(paramIdArray[count], valueArray[count]);   
		if(ret)
		{
			delete pObject;
			return ret;
		}
	}
	//_LIT16(KFileName,"c:\\data\\abc.txt");
  	TFileName configFileName; 
	configFileName.Zero();
	configFileName.Append(KMMCDriveLetter);
	configFileName.Append(MDriveColon);
	configFileName.Append(KFileName1);
  	RFs* rfs = NULL;

//	TFileName FileName2("c:\\data\\abc.txt");
	pObject->WriteToFileL(configFileName, rfs);
	delete pObject;
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CtestCSCPParamObject::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CtestCSCPParamObject::ReadFromFileL(CStifItemParser& aItem)
{
    TFileName configFileName; 
	configFileName.Zero();
	configFileName.Append(KMMCDriveLetter);
	configFileName.Append(MDriveColon);
	configFileName.Append(KFileName2);
  	RFs* rfs = NULL;
	
    CSCPParamObject* pObject = CSCPParamObject::NewL();
    CleanupStack::PushL( pObject );
    
    pObject->ReadFromFileL(configFileName, rfs);
    
	CleanupStack::Pop( pObject );         	
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CtestCSCPParamObject::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CtestCSCPParamObject::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
