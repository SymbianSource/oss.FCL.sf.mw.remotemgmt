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
* Description:   ?Description
*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "TestStoreFormat.h"
//#include <nsmlunicodeconverter.h>
#include <NSmlContactsDataStoreFormat_1_2.rsg>
#include <data_caging_path_literals.hrh>
#include <barsc.h>
#include <bautils.h>
#include <s32mem.h>

_LIT(KNSmlContactsStoreFormatRsc_1_2,"z:NSmlContactsDataStoreFormat_1_2.rsc");
_LIT8(KTestString,"test");
enum TOption
		{
		EOptionHasMaxSize,
		EOptionHasMaxOccur,
		EOptionNoTruncate
		};


const TInt KNSml32BitNumLength = 12;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestStoreFormat::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestStoreFormat::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTestStoreFormat::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestStoreFormat::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CTestStoreFormat::ExampleL ),
        
        
        //test cases for CSmlDataStoreFormat
        ENTRY( "SetDisplayName" ,CTestStoreFormat::SetDisplayNameL),
        ENTRY( "SetAndGetSyncMask" ,CTestStoreFormat::SetAndGetSyncMaskL),
        ENTRY( "SetndGetMimeCount" ,CTestStoreFormat::SetndGetMimeCountL),
        ENTRY( "CreateDataFieldTest" ,CTestStoreFormat::CreateDataFieldTestL),
        ENTRY( "SetAndGetMimeFormatTxt" ,CTestStoreFormat::SetndGetMimeFomatTxL),
        ENTRY( "SetAndGetMimeFormatRx" ,CTestStoreFormat::SetndGetMimeFomatRxL),
        ENTRY( "SetndGetFlags" ,CTestStoreFormat::SetndGetFlagsL),
        ENTRY( "SetAndGetMaxSize" ,CTestStoreFormat::SetAndGetMaxSizeL),
        ENTRY( "SetAndGetMaxItems" ,CTestStoreFormat::SetAndGetMaxItemsL),
        ENTRY( "SetAndGetFolderProperty" ,CTestStoreFormat::SetAndGetFolderPropertyL),
        ENTRY( "DataFieldGetAndSetEnumValue" ,CTestStoreFormat::DataFieldGetAndSetEnumValueL),
        ENTRY( "SetAndGetFilterCapability" ,CTestStoreFormat::SetAndGetFilterCapabilityL),
        
        ///test cases for CSmlDataField
        ENTRY("CreateSmlDataField",CTestStoreFormat::CreateSmlDataFieldL),
        ENTRY( "DataFieldGetAndSetDisplayName" ,CTestStoreFormat::DataFieldGetAndSetL),
        ENTRY( "DataFieldGetAndSetDataType" ,CTestStoreFormat::DataFieldGetAndSetDataTypeL),
        ENTRY( "DataFieldGetAndSetEnumValue" ,CTestStoreFormat::DataFieldGetAndSetEnumValueL),
        ENTRY( "DataFieldGetAndSetName" ,CTestStoreFormat::DataFieldGetAndSetNameL),
        
        
        //test cases for CSmlDataPropertyParam
        ENTRY( "DataPropParamSetAndGetDataField" ,CTestStoreFormat::DataPropParamSetAndGetDataFieldL),
        ENTRY( "CreateMimeFormat" ,CTestStoreFormat::CreateMimeFormatL),
        ENTRY( "Externalize" ,CTestStoreFormat::ExternalizeL),
        ENTRY( "SetMimeProperties" ,CTestStoreFormat::SetMimePropertiesL),
        ENTRY( "GetMimeProperties" ,CTestStoreFormat::GetMimePropertiesL),
        ENTRY( "CreateDataProperty" ,CTestStoreFormat::CreateDataPropertyL),
        ENTRY( "SetDataProperty" ,CTestStoreFormat::SetDataPropertyL),
        ENTRY( "CreateFilterCapability" ,CTestStoreFormat::CreateFilterCapabilityL),
        ENTRY( "SetFilterCapability" ,CTestStoreFormat::SetFilterCapabilityL),





       
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CTestStoreFormat::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestStoreFormat::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KTestStoreFormat, "TestStoreFormat" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KTestStoreFormat, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KTestStoreFormat, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
    
// -----------------------------------------------------------------------------
// CTestStoreFormat::CreateMimeFormatL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
 	
 TInt CTestStoreFormat::CreateDataFieldL(CStifItemParser& aItem)
 	{
 	

    CSmlDataStoreFormat * testFormat = CSmlDataStoreFormat::NewLC();
 	CleanupStack::PopAndDestroy(testFormat);
 	
 	//from resource
 	RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader;
 	
 	ReadFormatResourceLC(reader);
 	CSmlDataStoreFormat* testFormat1 = CSmlDataStoreFormat::NewLC(stringPool, reader );
 	
  	CleanupStack::PopAndDestroy(testFormat1);
  	stringPool.Close();
  
  	
 	return KErrNone;
  	}
  	 
// -----------------------------------------------------------------------------
// CTestStoreFormat::CreateDataFieldTestL
// -----------------------------------------------------------------------------
//     	 
 	
  TInt CTestStoreFormat::CreateDataFieldTestL(CStifItemParser& aItem)
 	{
 	
 
     CSmlDataPropertyParam * testFormat = CSmlDataPropertyParam::NewLC();
 	 CleanupStack::PopAndDestroy(testFormat);
 	
 	  //from resource
     RStringPool stringPool;
 	 stringPool.OpenL();
 	 TResourceReader reader;
 	 
 	 ReadFormatResourceLC(reader);
 	 CSmlDataStoreFormat* testFormat1 = CSmlDataStoreFormat::NewLC(stringPool, reader );
 	 
 	 	 
 	 CleanupStack::PopAndDestroy(2);
  	 stringPool.Close();
   
 	 return KErrNone;
  	}	 	
  	
// -----------------------------------------------------------------------------
// CTestStoreFormat::SetDisplayNameL
// -----------------------------------------------------------------------------
// 
 
TInt CTestStoreFormat::SetDisplayNameL(CStifItemParser& aItem)
{
    TPtrC string;
	aItem.GetNextString ( string );
  	CSmlDataStoreFormat * testFormat = CSmlDataStoreFormat::NewLC();
  	testFormat->SetDisplayNameL(string);
  	if(string.CompareC(testFormat->DisplayName()))
  	      User::Leave(KErrNotFound);
  	CleanupStack::PopAndDestroy(testFormat);
  	return KErrNone;
  	
}
  
// -----------------------------------------------------------------------------
// CTestStoreFormat::SetAndGetSyncMaskL
// -----------------------------------------------------------------------------
//     
  
TInt CTestStoreFormat::SetAndGetSyncMaskL(CStifItemParser& aItem)
{
    TPtrC string;
	aItem.GetNextString ( string );
  	CSmlDataStoreFormat * testFormat = CSmlDataStoreFormat::NewLC();
  	TSmlSyncTypeMask iSyncMask;
  	testFormat->SetSyncTypeMask(iSyncMask);
  	testFormat->SyncTypes();
  	CleanupStack::PopAndDestroy(testFormat);
  	return KErrNone;
  	
}
  
// -----------------------------------------------------------------------------
// CTestStoreFormat::SetndGetMimeCountL
// -----------------------------------------------------------------------------
//  
  
TInt CTestStoreFormat::SetndGetMimeCountL(CStifItemParser& aItem)
{
    	
	RPointerArray<CSmlMimeFormat>* storeformat;
	
	CSmlDataStoreFormat * datastoreformat = CSmlDataStoreFormat::NewLC();
	CSmlMimeFormat *mimeformat=CSmlMimeFormat::NewLC();
	
  	
  	storeformat = new (ELeave) RPointerArray<CSmlMimeFormat>(10);
  	storeformat->AppendL(mimeformat);
   	datastoreformat->SetMimeFormatsL(*storeformat);
  	
  	TInt count=datastoreformat->MimeFormatCount();
  	if(count)
  	{
  		datastoreformat->MimeFormat(0);
  	}
  	  	
  	CleanupStack::Pop(2);
   	storeformat->ResetAndDestroy();
  	delete storeformat;
    
    return KErrNone;
  	
}
 
  
// -----------------------------------------------------------------------------
// CTestStoreFormat::SetndGetMimeFomatTxL
// -----------------------------------------------------------------------------
//  
 
TInt CTestStoreFormat::SetndGetMimeFomatTxL(CStifItemParser& aItem)
{
     CSmlDataStoreFormat * testFormat = CSmlDataStoreFormat::NewLC();
     testFormat->SetMimeFormatTxPref(0);
     TInt mime=testFormat->MimeFormatTxPref();
     if(mime!=0)
       User::Leave(KErrGeneral);
     CleanupStack::PopAndDestroy(testFormat);
     return KErrNone;
      	
}


// -----------------------------------------------------------------------------
// CTestStoreFormat::SetndGetMimeFomatRxL
// -----------------------------------------------------------------------------
//  
  
TInt CTestStoreFormat::SetndGetMimeFomatRxL(CStifItemParser& aItem)
{
     CSmlDataStoreFormat * testFormat = CSmlDataStoreFormat::NewLC();
     testFormat->SetMimeFormatRxPref(0);        
     TInt mime=testFormat->MimeFormatRxPref();
     if(mime!=0)
        User::Leave(KErrGeneral);
     CleanupStack::PopAndDestroy(testFormat);
     return KErrNone;
      	
}

// -----------------------------------------------------------------------------
// CTestStoreFormat::SetndGetFlagsL
// -----------------------------------------------------------------------------
//  
  
  
TInt CTestStoreFormat::SetndGetFlagsL(CStifItemParser& aItem)
{
     CSmlDataStoreFormat * testFormat = CSmlDataStoreFormat::NewLC();
     testFormat->SetSupportedOptions(0);        
     TInt  index;
     TBool chkSupported;
	 aItem.GetNextInt ( index );
  	/* if(index==0)
    	 chkSupported=testFormat->IsSupported(EOptionHasMaxSize);
     aItem.GetNextInt ( index );
     if(index==1)
     chkSupported=testFormat->IsSupported(EOptionHasMaxOccur);
     aItem.GetNextInt ( index );
     if(index==2)
     chkSupported=testFormat->IsSupported(EOptionNoTruncate);*/
     CleanupStack::PopAndDestroy(testFormat);
     return KErrNone;
       	
}

// -----------------------------------------------------------------------------
// CTestStoreFormat::SetAndGetMaxSizeL
// -----------------------------------------------------------------------------
//  
  
TInt CTestStoreFormat::SetAndGetMaxSizeL(CStifItemParser& aItem)
{
     
    TInt index;
    aItem.GetNextInt ( index );
    TInt maxsize;
  	CSmlDataStoreFormat * testFormat = CSmlDataStoreFormat::NewLC();
  	if(index==1)
  	{
  	  testFormat->SetMaxSize(1);
  	  if(testFormat->HasMaxSize())
  	      maxsize=testFormat->MaxSize();  	  
  	}
  	aItem.GetNextInt ( index );
  	if(index==0)
  	{
  	  testFormat->SetMaxSize(0);
  	  if(testFormat->HasMaxSize())
  	     maxsize=testFormat->MaxSize();
  	  
  	}
  	 	  
  	 CleanupStack::PopAndDestroy(testFormat);
     return KErrNone;    

}
  
// -----------------------------------------------------------------------------
// CTestStoreFormat::SetAndGetMaxItemsL
// -----------------------------------------------------------------------------
// 
TInt CTestStoreFormat::SetAndGetMaxItemsL(CStifItemParser& aItem)
{
  	 TInt index;
     aItem.GetNextInt ( index );
     TInt maxsize;
     CSmlDataStoreFormat * testFormat = CSmlDataStoreFormat::NewLC();
  	 if(index==1)
  	 {
  	   testFormat->SetMaxItems(1);
  	   if(testFormat->HasMaxItems())
  	     maxsize=testFormat->MaxItems();  	  
  	 }
  	 aItem.GetNextInt ( index );
  	 if(index==0)
  	 {
  	   testFormat->SetMaxItems(0);
  	   if(!testFormat->HasMaxItems())
  	       maxsize=testFormat->MaxItems();
  	 }
  	 	  
  	 CleanupStack::PopAndDestroy(testFormat);
     return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestStoreFormat::SetAndGetFolderPropertyL
// -----------------------------------------------------------------------------
//
  
TInt CTestStoreFormat::SetAndGetFolderPropertyL(CStifItemParser& aItem)
{
    	
	RPointerArray<CSmlDataProperty>* dataproparray;
	
	CSmlDataStoreFormat * datastoreformat = CSmlDataStoreFormat::NewLC();
	CSmlDataProperty *dataproperty=CSmlDataProperty::NewLC();
	
  	
  	dataproparray = new (ELeave) RPointerArray<CSmlDataProperty>(10);
  	dataproparray->AppendL(dataproperty);
   	datastoreformat->SetFolderPropertiesL(*dataproparray);
  	
  	TInt count=datastoreformat->FolderPropertyCount();
  	if(count)
  	{
  		datastoreformat->FolderProperty(0);
  	}
  	  	
  	CleanupStack::Pop(2);
   	dataproparray->ResetAndDestroy();
  	delete dataproparray;
    
    return KErrNone;
  	
}

// -----------------------------------------------------------------------------
// CTestStoreFormat::SetAndGetFilterCapabilityL
// -----------------------------------------------------------------------------
//

TInt CTestStoreFormat::SetAndGetFilterCapabilityL(CStifItemParser& aItem)
{
    	
	RPointerArray<CSmlFilterCapability>* filterarray;
	
	CSmlDataStoreFormat * datastoreformat = CSmlDataStoreFormat::NewLC();
	CSmlFilterCapability *filtercapability=CSmlFilterCapability::NewLC();
	
  	
  	filterarray = new (ELeave) RPointerArray<CSmlFilterCapability>(10);
  	filterarray->AppendL(filtercapability);
   	datastoreformat->SetFilterCapabilitiesL(*filterarray);
  	
  	TInt count=datastoreformat->FilterCapabilityCount();
  	if(count)
  	{
  		datastoreformat->FilterCapability(0);
  	}
  	  	
  	CleanupStack::Pop(2);
   	filterarray->ResetAndDestroy();
  	delete filterarray;
    
    return KErrNone;
  	
}
  
// -----------------------------------------------------------------------------
// CTestStoreFormat::ReadFormatResourceLC
// -----------------------------------------------------------------------------
//
  
void CTestStoreFormat::ReadFormatResourceLC(TResourceReader& aReader)
{
  	
 	
 	TFileName fileName;
	TParse parse;
    parse.Set( KNSmlContactsStoreFormatRsc_1_2, &KDC_RESOURCE_FILES_DIR, NULL );
    

	fileName = parse.FullName();

	RResourceFile resourceFile; 
	RFs iRfs;
	User::LeaveIfError( iRfs.Connect() );
	BaflUtils::NearestLanguageFile( iRfs, fileName );

	resourceFile.OpenL( iRfs, fileName );
	
	HBufC8* contactsDataFormat = resourceFile.AllocReadLC( NSML_CONTACTS_DATA_STORE_FORMAT );
	
	aReader.SetBuffer( contactsDataFormat );
	
	iRfs.Close();
 	
 	resourceFile.Close();
 	
}
  
// -----------------------------------------------------------------------------
// CTestStoreFormat::CreateSmlDataFieldL
// -----------------------------------------------------------------------------
//
    

TInt CTestStoreFormat::CreateSmlDataFieldL(CStifItemParser& aItem)
{
   	RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader;
 	
 	ReadFormatResourceLC(reader);
 	CSmlDataStoreFormat* testFormat1 = CSmlDataStoreFormat::NewLC(stringPool, reader );
   	CleanupStack::PopAndDestroy(2);
  	stringPool.Close();
  	
  	return KErrNone;
  
}
  
// -----------------------------------------------------------------------------
// CTestStoreFormat::DataFieldGetAndSetL
// -----------------------------------------------------------------------------
//
  
TInt CTestStoreFormat::DataFieldGetAndSetL(CStifItemParser& aItem)
{
  	CSmlDataField * testDataField = CSmlDataField::NewLC();
  	TPtrC string;
	aItem.GetNextString ( string );

    testDataField->SetDisplayNameL(string);
    
    if(string.Compare(testDataField->DisplayName()))
          User::Leave(KErrNotFound);
    
  	CleanupStack::PopAndDestroy(testDataField);
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestStoreFormat::DataFieldGetAndSetNameL
// -----------------------------------------------------------------------------
//
TInt CTestStoreFormat::DataFieldGetAndSetNameL(CStifItemParser& aItem)
{
    RString *datatype=new (ELeave) RString();;
    CSmlDataField * testDataField = CSmlDataField::NewLC();
    
    testDataField->SetNameL(*datatype);
    
    testDataField->Name();
   /* if(test==NULL)
         User::Leave(KErrNotFound);*/
    
    CleanupStack::PopAndDestroy(testDataField);
    datatype->Close();
    
    return KErrNone;  
}

// -----------------------------------------------------------------------------
// CTestStoreFormat::DataFieldGetAndSetDataTypeL
// -----------------------------------------------------------------------------
//
 	
TInt CTestStoreFormat::DataFieldGetAndSetDataTypeL(CStifItemParser& aItem)
{
    RString *datatype=new (ELeave) RString();;
    CSmlDataField * testDataField = CSmlDataField::NewLC();
    
    testDataField->SetDataTypeL(*datatype);
    
    testDataField->DataType();
   /* if(test==NULL)
         User::Leave(KErrNotFound);*/
    
    CleanupStack::PopAndDestroy(testDataField);
    datatype->Close();
    
    return KErrNone;    
      
}

// -----------------------------------------------------------------------------
// CTestStoreFormat::DataFieldGetAndSetEnumValueL
// -----------------------------------------------------------------------------
//
TInt CTestStoreFormat::DataFieldGetAndSetEnumValueL(CStifItemParser& aItem)
{
	
	RArray<RString> *enumarray;
	enumarray = new (ELeave) RArray<RString>(10);
	
	CSmlDataField * testDataField = CSmlDataField::NewLC();
	testDataField->SetEnumValuesL(*enumarray);
	TInt count=testDataField->EnumValueCount();
	if(count)
	    testDataField->EnumValue(0);
		
	enumarray->Reset();
	delete enumarray;
	CleanupStack::PopAndDestroy(testDataField);
	
	
	return KErrNone;
	
}

//test cases for CSmlDataPropertParam class
// -----------------------------------------------------------------------------
// CTestStoreFormat::DataPropParamSetAndGetDataFieldL
// -----------------------------------------------------------------------------
//
TInt CTestStoreFormat::DataPropParamSetAndGetDataFieldL(CStifItemParser& aItem)
{
  	CSmlDataPropertyParam * testDataField1 = CSmlDataPropertyParam::NewLC();
   	CSmlDataField *datafield1=CSmlDataField::NewLC();
    testDataField1->SetDataFieldL(*datafield1);
    testDataField1->Field();
	CleanupStack::PopAndDestroy(datafield1);
	CleanupStack::PopAndDestroy(testDataField1);
	return KErrNone;
	
}
  
  
// -----------------------------------------------------------------------------
// CTestStoreFormat::CreateMimeFormatL
// -----------------------------------------------------------------------------
//
    
 TInt CTestStoreFormat::CreateMimeFormatL()
 	{

	CSmlMimeFormat* testFormat = CSmlMimeFormat::NewLC();
 	CleanupStack::PopAndDestroy(testFormat);
  	return KErrNone;
  	}
  	
// -----------------------------------------------------------------------------
// CTestStoreFormat::ExternalizeL
// -----------------------------------------------------------------------------
//  
 
 TInt CTestStoreFormat::ExternalizeL()
 {
 	RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader;
 	
 	ReadFormatResourceLC(reader);
 	CSmlMimeFormat* testFormat1 = CSmlMimeFormat::NewLC(stringPool, reader );
 //	CSmlDataStoreFormat* testFormat1 = CSmlDataStoreFormat::NewLC(stringPool, reader );
  	HBufC8* store = HBufC8::NewLC(128);
 	TPtr8 p(store->Des());
 	RDesWriteStream wStream(p);
 	CleanupClosePushL(wStream);
 	testFormat1->ExternalizeL(wStream);
 	
 	CleanupStack::PopAndDestroy(4);
 	stringPool.Close();
 	
 	return KErrNone;
 }

// -----------------------------------------------------------------------------
// CTestStoreFormat::SetMimePropertiesL
// -----------------------------------------------------------------------------
//
TInt CTestStoreFormat::SetMimePropertiesL()
	{
	
	RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader;
 	
 	ReadFormatResourceLC(reader);
 	
 	TInt version( reader.ReadInt8() );					// BYTE version
	HBufC8* tmpMimetype = reader.ReadHBufC8L();
	RStringF mimeType = stringPool.OpenFStringL( *tmpMimetype );
	
	HBufC8* tmpMimeversion = reader.ReadHBufC8L();
	RStringF mimeVersion = stringPool.OpenFStringL( *tmpMimeversion );
	
	TInt number( reader.ReadInt16() );	
	
	RPointerArray<CSmlDataProperty> *propertyArray = new (ELeave)RPointerArray<CSmlDataProperty> (1); 
	CleanupStack::PushL(propertyArray);
	for ( TInt index = 0 ; index < number ; index++ )
		{
		CSmlDataProperty *temppoint = CSmlDataProperty::NewLC( stringPool, reader );	// STRUCT properties[] (SML_DATA_PROPERTY)
		propertyArray->AppendL( temppoint );
		CleanupStack::Pop( temppoint );
		}
	TBool field = reader.ReadInt8();
	
	CSmlMimeFormat* testFormat = CSmlMimeFormat::NewLC();
 	
 	testFormat->SetMimeTypeL(mimeType);
 	testFormat->SetMimeVersionL(mimeVersion);
 	testFormat->SetDataPropertiesL(*propertyArray);
 	testFormat->SetFieldLevel(field);
 	
  	CleanupStack::PopAndDestroy(testFormat);
 	
 	propertyArray->ResetAndDestroy();
 	CleanupStack::PopAndDestroy(2);
 	stringPool.Close();
 	
 	return KErrNone;
	
	}
// -----------------------------------------------------------------------------
// CTestStoreFormat::GetMimePropertiesL
// -----------------------------------------------------------------------------
//

TInt CTestStoreFormat::GetMimePropertiesL()
	{
	RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader;
 	
 	ReadFormatResourceLC(reader);
 	
 	TInt version( reader.ReadInt8() );					// BYTE version
	HBufC8* tmpMimetype = reader.ReadHBufC8L();
	RStringF mimeType = stringPool.OpenFStringL( *tmpMimetype );
	
	HBufC8* tmpMimeversion = reader.ReadHBufC8L();
	RStringF mimeVersion = stringPool.OpenFStringL( *tmpMimeversion );
	
	TInt number( reader.ReadInt16() );	
	
	RPointerArray<CSmlDataProperty> *propertyArray = new (ELeave)RPointerArray<CSmlDataProperty> (1); 
	CleanupStack::PushL(propertyArray);
	for ( TInt index = 0 ; index < number ; index++ )
		{
		CSmlDataProperty *temppoint = CSmlDataProperty::NewLC( stringPool, reader );	// STRUCT properties[] (SML_DATA_PROPERTY)
		propertyArray->AppendL( temppoint );
		CleanupStack::Pop( temppoint );
		}
	TBool field = reader.ReadInt8();
	
	CSmlMimeFormat* testFormat = CSmlMimeFormat::NewLC();
 	
 	testFormat->SetMimeTypeL(mimeType);
 	testFormat->SetMimeVersionL(mimeVersion);
 	testFormat->SetDataPropertiesL(*propertyArray);
 	testFormat->SetFieldLevel(field);
   	
 	if (testFormat->MimeType().DesC().Compare(mimeType.DesC()))
 		{
 	 //	User::Leave(KErrGeneral);
 		}
 	if (testFormat->MimeVersion().DesC().Compare(mimeVersion.DesC()))
 		{
 	//	User::Leave(KErrGeneral);
 		}
 	if (testFormat->PropertyCount() != number)
 		{
 	//	User::Leave(KErrGeneral);
 		}
 	if (testFormat->FieldLevel() != field)
 		{
 	//	User::Leave(KErrGeneral);
 		}
 	
 	 	
  	CleanupStack::PopAndDestroy(testFormat);
 	
 	propertyArray->ResetAndDestroy();
 	CleanupStack::PopAndDestroy(2);
 	stringPool.Close();
 	
 	return KErrNone;
	
	}

// -----------------------------------------------------------------------------
// CTestStoreFormat::CreateDataPropertyL
// -----------------------------------------------------------------------------
//
TInt CTestStoreFormat::CreateDataPropertyL()
	{
	
	CSmlDataProperty* testProperty = CSmlDataProperty::NewLC();
 	CleanupStack::PopAndDestroy(testProperty);
 	
 	//from resource
  	
 	RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader;
 	
 	ReadFormatResourceLC(reader);
 	CSmlDataStoreFormat* format = CSmlDataStoreFormat::NewLC(stringPool, reader );
 	CleanupStack::PopAndDestroy(2);
  	
  	stringPool.Close();
  	
 	return KErrNone;
	
	
	}

// -----------------------------------------------------------------------------
// CTestStoreFormat::SetDataPropertyL
// -----------------------------------------------------------------------------
//

TInt CTestStoreFormat::SetDataPropertyL()
{
	
	CSmlDataProperty* testProperty = CSmlDataProperty::NewLC();
	RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader;
 	
 	ReadFormatResourceLC(reader);
 	
 	CSmlDataStoreFormat* format = CSmlDataStoreFormat::NewLC(stringPool, reader );
 	  	
 	const CSmlMimeFormat& mimeFormat = format->MimeFormat(0);
 	const CSmlDataProperty& property = mimeFormat.Property(0);
 	
  	const CSmlDataField& field = property.Field();
 	 	
 	TInt paramCount = property.ParamCount();
 	TBool hasSize = property.HasMaxSize();
 	TBool hasOccur = property.HasMaxOccur();
 	TInt maxSize  = property.MaxSize();
 	TInt maxOccur = property.MaxOccur();
 	
 	TBool sizeSupport = property.IsSupported(CSmlDataProperty::EOptionHasMaxSize);
 	TBool occurSupport = property.IsSupported(CSmlDataProperty::EOptionHasMaxOccur);
 	TBool noTruncate = property.IsSupported(CSmlDataProperty::EOptionNoTruncate);
 	
 		
 	HBufC8* store = HBufC8::NewLC(128);
 	TPtr8 p(store->Des());
 	RDesWriteStream wStream(p);
 	CleanupClosePushL(wStream);
 	property.ExternalizeL(wStream);
 	
 	RPointerArray<CSmlDataPropertyParam>* array = new (ELeave) RPointerArray<CSmlDataPropertyParam> (1);  
 	CleanupStack::PushL(array);
 	 	
 	testProperty->SetDataFieldL(const_cast<CSmlDataField&> (field));
 	testProperty->SetMaxSize(maxSize);
 	testProperty->SetMaxOccur(maxOccur);
 	
 	if(paramCount)
 		{
 		const CSmlDataPropertyParam& param = property.Param(paramCount - 1);
 		CSmlDataPropertyParam* paramPtr = const_cast <CSmlDataPropertyParam*> (&param);
 		array->AppendL(paramPtr);
 		testProperty->SetPropertyParamsL(*array);
 		}
 	else
 		{
 		CSmlDataPropertyParam* 	param1 = CSmlDataPropertyParam::NewLC();
 		array->AppendL(param1);
 		testProperty->SetPropertyParamsL(*array);
 		CleanupStack::Pop(param1);
 		}
 	TUint flag = 0x0;
    testProperty->SetSupportedOptions(flag);
    /*CleanupStack::PopAndDestroy(testProperty);*/
    
 	array->ResetAndDestroy();
 	
  	CleanupStack::PopAndDestroy(5);
  	CleanupStack::Pop();
	  	
  	stringPool.Close();
  	
 	return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestStoreFormat::SetDataPropertyL
// -----------------------------------------------------------------------------
//


TInt CTestStoreFormat::CreateFilterCapabilityL(CStifItemParser& aItem)
	{
	
	CSmlFilterCapability* testFilter = CSmlFilterCapability::NewLC();
 	CleanupStack::PopAndDestroy(testFilter);
 	TInt param;
 	aItem.GetNextInt ( param );
 	//from resource
  	
 	/*RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader; 	
 	ReadFormatResourceLC(reader);
 	
 	TInt version = reader.ReadInt8();				// BYTE version
	HBufC* displayName = HBufC::NewLC(128);
	displayName = reader.ReadHBufC16L(); 	// LTEXT display_name
	TUint16 tmpSynctypes( reader.ReadInt16() );	// WORD sync_types

	TInt number( reader.ReadInt16() );		// The first WORD contains the number of DATA structs within the resource

 	
 	CSmlFilterCapability* filter = CSmlFilterCapability::NewLC(stringPool, reader );
 	CleanupStack::PopAndDestroy(3);
  	
  	stringPool.Close();*/
  	
  	RArray<RString> *propname;
	propname = new (ELeave) RArray<RString>(10);
  	testFilter->SetPropertiesListL(*propname);
  	testFilter->PropertyName(0);
  	testFilter->PropertyCount();
  	
  	propname->Reset();
	delete propname;
  	
  	RArray<RString> *enumarray;
	enumarray = new (ELeave) RArray<RString>(10);
  	
  //	testFilter->SetKeyWordListL(*enumarray);
  	TInt count=testFilter->KeywordCount();
  	
  	if(count)
  	  testFilter->Keyword(0);
  	
  	
  	enumarray->Reset();
	delete enumarray; 	
  	
 	return KErrNone;
		
	}
// -----------------------------------------------------------------------------
// CTestStoreFormat::SetFilterCapabilityL
// -----------------------------------------------------------------------------
//	
TInt CTestStoreFormat::SetFilterCapabilityL()
{
	CSmlFilterCapability* testFilter = CSmlFilterCapability::NewLC();
	
	RStringPool stringPool;
 	stringPool.OpenL();
 	TResourceReader reader;
 	
 	ReadFormatResourceLC(reader);
 	
 	TInt version( reader.ReadInt8() );					// BYTE version
	HBufC8* tmpMimetype = reader.ReadHBufC8L();
	RStringF mimeType = stringPool.OpenFStringL( *tmpMimetype );
	
	HBufC8* tmpMimeversion = reader.ReadHBufC8L();
	RStringF mimeVersion = stringPool.OpenFStringL( *tmpMimeversion );
	
	
	testFilter->SetMimeTypeL(mimeType);
	testFilter->SetMimeVersionL(mimeVersion);
	if (mimeType.DesC().Compare(testFilter->MimeType().DesC()) != 0)
		{
		//User::Leave(KErrGeneral);	
		}
	if (mimeVersion.DesC().Compare(testFilter->MimeVersion().DesC()) != 0)
		{
		//User::Leave(KErrGeneral);
		}
	
	TInt kwCount = testFilter->KeywordCount();
	TInt pCount = testFilter->PropertyCount();
	
	HBufC8* store = HBufC8::NewLC(128);
 	TPtr8 p(store->Des());
 	RDesWriteStream wStream(p);
 	CleanupClosePushL(wStream);
 	testFilter->ExternalizeL(wStream);
 	
	
	CleanupStack::PopAndDestroy(4);
	
	stringPool.Close();
	return KErrNone;
	
}

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
