/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Filter header.
*
*/


#ifndef __NSMLFILTER_H__
#define __NSMLFILTER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>

class CNSmlDevInfProp;

struct sml_filter_s;
struct sml_pcdata_s;
struct sml_metinf_metinf_s;
struct sml_devinf_property_s;
struct sml_devinf_propparam_s;
struct sml_devinf_propertylist_s;
struct sml_item_s;

// ------------------------------------------------------------------------------------------------
//  TNSmlFieldPropData
// ------------------------------------------------------------------------------------------------
struct TNSmlFieldPropData
    {
public:

	enum TNSmlFieldPropTag
		{
		EPropName,
        EPropDataType,
        EPropMaxOccur,
        EPropMaxSize,
        EPropNoTruncate,
        EPropValEnum,
        EPropDisplayName,
		EParamName,
		EParamValEnum,
		EParamDataType,
		EParamDisplayName
		};

	/**
    * C++ constructor.
    * @param aTag Element type.
    * @param aValue Value of the element.
    */
	IMPORT_C TNSmlFieldPropData( TNSmlFieldPropTag aTag, const TDesC8& aValue );
	
	/**
    * Static compare routine for two TNSmlFilterCapData instances. Instances are kept in order
	* in array for faster searching.
    * @param aFirst First compare instance.
    * @param aSecond Second compare instance.
	* @return TInt Returns greater than 0 if aFirst is greater than aSecond, less than 0 if aFirst
	* is less than aSecond and 0 if aFirst is equal to aSecond.
    */
	IMPORT_C static TInt Compare( const TNSmlFieldPropData& aFirst, const TNSmlFieldPropData& aSecond );

public:
	TNSmlFieldPropTag iTag;
	const TBuf8<64> iValue;
	};

// ------------------------------------------------------------------------------------------------
//  CNSmlFilterHandler
// ------------------------------------------------------------------------------------------------
class CNSmlFilterHandler : public CBase
    {
public:
    /**
    * C++ constructor.
    * @param aFilter Pointer to filter structure.
    */
    IMPORT_C CNSmlFilterHandler( sml_filter_s* aFilter );

    /**
    * Returns value of filter meta type element as 8-bit descriptor.
    * @return TPtrC8 Value of filter meta type element.
    */
    IMPORT_C TPtrC8 FilterMetaType() const;

    /**
    * Returns value of FilterType element as 8-bit descriptor.
    * @return TPtrC8 Value of FilterType element.
    */
    IMPORT_C TPtrC8 FilterType() const;

	/**
    * Returns value of record meta type element as 8-bit descriptor.
    * @return TPtrC8 Value of record meta type element.
    */
    IMPORT_C TPtrC8 RecordMetaType() const;

	/**
    * Returns value of record data element as 8-bit descriptor.
    * @return TPtrC8 Value of record data element.
    */
    IMPORT_C TPtrC8 RecordData() const;

	/**
    * Returns value of field meta type element as 8-bit descriptor.
    * @return TPtrC8 Value of field meta type element.
    */
    IMPORT_C TPtrC8 FieldMetaType() const;

    /**
    * Gets field properties as an array of TNSmlFieldPropData items.
    * @param aFieldProps On return contains field properties parsed into TNSmlFieldPropData items.
	* @param aPropName If defined then field properties only for given property are returned,
    * otherwise all field properties are parsed into array.
    */
    IMPORT_C void FieldDataPropsL( CArrayFix<TNSmlFieldPropData>& aFieldProps, const TDesC8& aPropName = TPtrC8() );

protected:

	void AppendFieldPropL( CArrayFix<TNSmlFieldPropData>& aFieldProps, sml_devinf_property_s* aProp );
    void AppendPropParamL( CArrayFix<TNSmlFieldPropData>& aFieldProps, sml_devinf_propparam_s* aProp, TNSmlFieldPropData::TNSmlFieldPropTag aTag );

private:
    sml_filter_s* iFilter;
    };

// ------------------------------------------------------------------------------------------------
//  CNSmlFilter
// ------------------------------------------------------------------------------------------------
class CNSmlFilter : public CBase
    {
public:
    /**
    * Destructor.
    */    
	IMPORT_C ~CNSmlFilter();

    /**
    * Two-phase (leave safe) constructor.
    * @return CNSmlDbCaps* Pointer to newly created instance.
    */
    IMPORT_C static CNSmlFilter* NewL();
    /**
    * Two-phase (leave safe) constructor. Leaves instance into cleanupstack.
    * @return CNSmlDbCaps* Pointer to newly created instance.
    */
    IMPORT_C static CNSmlFilter* NewLC();

	/**
    * Creates instance of filter handler.
    * @return CNSmlFilterHandler* Pointer to newly created filter handler.
    */
    IMPORT_C CNSmlFilterHandler* CreateHandlerL() const;

    /**
    * Returns pointer to filter structure.
    * @return sml_filter_s* Pointer to filter structure.
    */
    IMPORT_C sml_filter_s* FilterL() const;

    /**
    * Returns pointer to device info property structure.
    * @return sml_devinf_propertylist_s* Pointer to device info property structure.
    */
    IMPORT_C sml_devinf_propertylist_s* Properties() const;

    /**
    * Sets MetaType element.
    * @param aMetaType Value of MetaType as descriptor.
    */
    IMPORT_C void SetFilterMetaTypeL( const TDesC8& aMetaType );

    /**
    * Sets FilterType element.
    * @param aFilterType Value of FilterType as descriptor.
    */
    IMPORT_C void SetFilterTypeL( const TDesC8& aFilterType );

    /**
    * Sets MetaType and Data elements to record item.
    * @param aMetaType Value of MetaType as descriptor.
    * @param aData Value of Data as descriptor.
    */
    IMPORT_C void SetRecordL( const TDesC8& aMetaType, const TDesC8& aData );

    /**
    * Sets MetaType element to field item.
    * @param aMetaType Value of MetaType as descriptor.
    */
    IMPORT_C void SetFieldMetaTypeL( const TDesC8& aMetaType );

    /**
    * Adds new data property to field item.
    * @param aPropName Property name as descriptor.
    * @return CNSmlDevInfProp* Pointer to newly created instance.  
    */
    IMPORT_C CNSmlDevInfProp* AddFieldDataPropLC( const TDesC8& aPropName );

private:
    void ConstructL();
    CNSmlFilter();

    void PcdataNewL( sml_pcdata_s*& aPcdata, const TDesC8& aContent ) const;
    void DoMetaL( sml_pcdata_s*& aMeta, const sml_metinf_metinf_s* aMetaData ) const;
    void DoItemDataL( sml_pcdata_s*& aData, const sml_devinf_propertylist_s* aProp ) const;

    mutable sml_filter_s* iFilter;
    mutable sml_devinf_propertylist_s* iPropList;
	};

#endif // __NSMLFILTER_H__