/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DB capabilities header.
*
*/


#ifndef __NSMLDBCAPS_H__
#define __NSMLDBCAPS_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <badesca.h>

// ------------------------------------------------------------------------------------------------
// TNSmlCtCapData 
// ------------------------------------------------------------------------------------------------
struct TNSmlCtCapData
	{
public:

// 1.2 CHANGES: New elements added.
	enum TNSmlCtCapTag
		{
		ECtType,
        EVerCt,
        EFieldLevel,
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
// changes end

	/**
    * C++ constructor.
    * @param aTag Element type.
    * @param aValue Value of the element.
    */
	IMPORT_C TNSmlCtCapData( TNSmlCtCapTag aTag, const TDesC8& aValue );
	
	/**
    * Static compare routine for two TNSmlCtCapData instances. Instances are kept in order
	* in array for faster searching.
    * @param aFirst First compare instance.
    * @param aSecond Second compare instance.
	* @return TInt Returns greater than 0 if aFirst is greater than aSecond, less than 0 if aFirst
	* is less than aSecond and 0 if aFirst is equal to aSecond.
    */
	IMPORT_C static TInt Compare( const TNSmlCtCapData& aFirst, const TNSmlCtCapData& aSecond );

public:
	TNSmlCtCapTag iTag;
	const TBuf8<64> iValue;
	};

// 1.2 CHANGES: FilterCap 
// ------------------------------------------------------------------------------------------------
// TNSmlFilterCapData 
// ------------------------------------------------------------------------------------------------
struct TNSmlFilterCapData
	{
public:

	enum TNSmlFilterCapTag
		{
		ECtType,
        EVerCt,
        EFilterKeyword,
		EPropName
		};

	/**
    * C++ constructor.
    * @param aTag Element type.
    * @param aValue Value of the element.
    */
	IMPORT_C TNSmlFilterCapData( TNSmlFilterCapTag aTag, const TDesC8& aValue );
	
	/**
    * Static compare routine for two TNSmlFilterCapData instances. Instances are kept in order
	* in array for faster searching.
    * @param aFirst First compare instance.
    * @param aSecond Second compare instance.
	* @return TInt Returns greater than 0 if aFirst is greater than aSecond, less than 0 if aFirst
	* is less than aSecond and 0 if aFirst is equal to aSecond.
    */
	IMPORT_C static TInt Compare( const TNSmlFilterCapData& aFirst, const TNSmlFilterCapData& aSecond );

public:
	TNSmlFilterCapTag iTag;
	const TBuf8<64> iValue;
	};

// changes end

// 1.2 CHANGES: New class (replaces CNSmlCtParam).
// ------------------------------------------------------------------------------------------------
// CNSmlPropParam
// ------------------------------------------------------------------------------------------------
struct sml_devinf_propparam_s;
class CNSmlPropParam : public CBase
	{
public:
	/**
    * C++ constructor.
    * @param aParamPtr Pointer to device info property parameter structure.
    */
    IMPORT_C CNSmlPropParam( sml_devinf_propparam_s* aParamPtr );

	/**
    * Sets parameter's displayable name.
    * @param aDispName Displayable name as descriptor.
    */
	IMPORT_C void SetDisplayNameL( const TDesC8& aDispName );

	/**
    * Adds new valenum.
    * @param aValEnum Value of valenum as descriptor.
    */
	IMPORT_C void AddValEnumL( const TDesC8& aValEnum );

	/**
    * Sets datatype.
    * @param aDataType Value of datatype as descriptor.
    */
    IMPORT_C void SetDataTypeL( const TDesC8& aDataType );

private:
    sml_devinf_propparam_s* iParamPtr;
	};
// changes end


// 1.2 CHANGES: New class (replaces CNSmlCtProp).
// ------------------------------------------------------------------------------------------------
// CNSmlDevInfProp
// ------------------------------------------------------------------------------------------------
struct sml_devinf_property_s;
class CNSmlDevInfProp : public CBase
	{
public:

	/**
    * C++ constructor.
    * @param aParamPtr Pointer to device info property structure.
    */
    IMPORT_C CNSmlDevInfProp( sml_devinf_property_s* aPropPtr );

	/**
    * C++ destructor.
    */
	IMPORT_C ~CNSmlDevInfProp();

	/**
    * Adds new parameter to this property. Also leaves newly created instance into cleanupstack.
    * @param aParam Name of the parameter as descriptor.
	* @return CNSmlPropParam* Pointer to newly created instance.
    */
	IMPORT_C CNSmlPropParam* AddParamLC( const TDesC8& aParam );

	/**
    * Sets property's maximum occurrence value.
    * @param aMaxOccur Maximum occurrence value as descriptor.
    */    
    IMPORT_C void SetMaxOccurL( const TDesC8& aMaxOccur );

	/**
    * Sets the maximum size of a property.
    * @param aMaxSize Maximum size of a property as descriptor.
    */    
    IMPORT_C void SetMaxSizeL( const TDesC8& aMaxSize );

	/**
    * Sets NoTruncate element.
    * Secifies if truncation is permitted.
    */    
    IMPORT_C void SetNoTruncateL();

	/**
    * Sets property's displayable name.
    * @param aDispName Displayable name as descriptor.
    */
	IMPORT_C void SetDisplayNameL( const TDesC8& aDispName );

	/**
    * Adds new valenum.
    * @param aValEnum Value of valenum as descriptor.
    */
	IMPORT_C void AddValEnumL( const TDesC8& aValEnum );

	/**
    * Sets datatype.
    * @param aDataType Value of datatype as descriptor.
    */
    IMPORT_C void SetDataTypeL( const TDesC8& aDataType );
private:
    sml_devinf_property_s* iPropPtr;
	};
// changes end

// ------------------------------------------------------------------------------------------------
// CNSmlCtCap
// ------------------------------------------------------------------------------------------------
struct sml_devinf_ctcap_s;
class CNSmlCtCap : public CBase
	{
public:
	CNSmlCtCap( sml_devinf_ctcap_s* aCtCapPtr );
	~CNSmlCtCap();

	/**
    * Sets value of CtCtype.
    * @param aCtType Value of CtType as descriptor.
    */
	IMPORT_C void SetCtTypeL( const TDesC8& aCtType );

// 1.2 CHANGES: Setting new elements in CtCap structure.
	/**
    * Sets value of VerCt.
    * @param aVerCt Value of VerCt as descriptor.
    */
    IMPORT_C void SetVerCtL( const TDesC8& aVerCt );
    
	/**
    * Sets FieldLevel element.
    */
    IMPORT_C void SetFieldLevelL();

	/**
    * Adds new property, also leaves newly created instance into cleanupstack.
    * @param aProp Property's name as descriptor.
	* @return CNSmlDevInfProp* Pointer to newly created instance.
    */
	IMPORT_C CNSmlDevInfProp* AddDevInfPropLC( const TDesC8& aProp );
// changes end

private:
	sml_devinf_ctcap_s* iCtCapPtr;
	};

// 1.2 CHANGES: New class for FilterCap params.
// ------------------------------------------------------------------------------------------------
// CNSmlFilterCap
// ------------------------------------------------------------------------------------------------
struct sml_devinf_filtercap_s;
class CNSmlFilterCap : public CBase
    {
public:
    CNSmlFilterCap( sml_devinf_filtercap_s* aFilterCapPtr );
    ~CNSmlFilterCap();

    /**
    * Adds new filter keyword to FilterCap.
    * @param aKeyword Value of filter keyword as descriptor.
    */
    IMPORT_C void AddFilterKeywordL( const TDesC8& aKeyword );

    /**
    * Adds new property name to FilterCap.
    * @param aPropName Value of property name as descriptor.
    */
    IMPORT_C void AddPropNameL( const TDesC8& aPropName );
private:
    sml_devinf_filtercap_s* iFilterCapPtr;
    };
// changes end

// ------------------------------------------------------------------------------------------------
// CNSmlCtCapHandler
// ------------------------------------------------------------------------------------------------
struct sml_devinf_datastore_s;
struct sml_devinf_ctcaplist_s;
struct sml_devinf_filtercaplist_s;
struct sml_pcdata_s;
struct sml_devinf_xmit_s;
struct sml_devinf_ctcap_s;
struct sml_devinf_ctdataprop_s;

class CNSmlCtCapsHandler : public CBase
	{
public:
	/**
    * C++ constructor.
    * @param aCtCaps Pointer to CtCaps structure.
    */
	IMPORT_C CNSmlCtCapsHandler( sml_devinf_ctcaplist_s* aCtCaps );

	/**
    * Gets CtCaps as array of TNSmlCtCapData items.
    * @param aCtCaps On return contains CtCaps parsed into TNSmlCtCapData items.
	* @param aCtType If defined then CtCaps only for given CtType are returned otherwise all CtCaps
	* are parsed into array.
    */
	IMPORT_C void GetCtCapsL( CArrayFix<TNSmlCtCapData>& aCtCaps, const TDesC8& aCtType = TPtrC8() );

protected:

	void AppendCtCapL( CArrayFix<TNSmlCtCapData>& aCtCaps, sml_devinf_ctcap_s* aCtCap );

// 1.2 CHANGES: New structures for version 1.2.
	void AppendDevInfPropL( CArrayFix<TNSmlCtCapData>& aCtCaps, sml_devinf_property_s* aCtCap );
	void AppendPropParamL( CArrayFix<TNSmlCtCapData>& aCtCaps, sml_devinf_propparam_s* aCtCap, TNSmlCtCapData::TNSmlCtCapTag aTag );
// changes end
private:
	sml_devinf_ctcaplist_s* iCtCaps; // does not own this!
	};

// 1.2 CHANGES: FilterCap Handler
// ------------------------------------------------------------------------------------------------
// CNSmlFilterCapHandler
// ------------------------------------------------------------------------------------------------
class CNSmlFilterCapsHandler : public CBase
	{
public:
	/**
    * C++ constructor.
    * @param aFilterCaps Pointer to FilterCaps structure.
    */
	IMPORT_C CNSmlFilterCapsHandler( sml_devinf_filtercaplist_s* aFilterCaps );

	/**
    * Gets FilterCaps as array of TNSmlFilterCapData items.
    * @param aFilterCaps On return contains FilterCaps parsed into TNSmlFilterCapData items.
	* @param aCtType If defined then FilterCaps only for given CtType are returned otherwise all FilterCaps
	* are parsed into array.
    */
	IMPORT_C void GetFilterCapsL( CArrayFix<TNSmlFilterCapData>& aFilterCaps, const TDesC8& aCtType = TPtrC8() );

protected:

	void AppendFilterCapL( CArrayFix<TNSmlFilterCapData>& aFilterCaps, sml_devinf_filtercap_s* aFilterCap );

private:
	sml_devinf_filtercaplist_s* iFilterCaps; // does not own this!
	};

// changes end

// ------------------------------------------------------------------------------------------------
// CNSmlDbCaps
// ------------------------------------------------------------------------------------------------

// 1.2 HYBRID CHANGES:
	struct sml_devinf_devinf_s;
// changes end

class CNSmlDbCaps : public CBase
	{
public:
	enum ENSmlSyncTypes 
		{
		EAllTypes = 0,
		ETwoWaySync = 1,
		ESlowTwoWaySync,
		EOneWaySyncFromClient,
		ERefreshSyncFromClient,
		EOneWaySyncFromServer,
		ERefreshSyncFromServer,
		EServerAlertedSync,
		ESyncTypeNone = 8
		};

	/**
    * Two-phase (leave safe) constructor.
    * @return CNSmlDbCaps* Pointer to newly created instance.
    */
	IMPORT_C static CNSmlDbCaps* NewL();

	/**
    * Two-phase (leave safe) constructor. Leaves instance into cleanupstack.
    * @return CNSmlDbCaps* Pointer to newly created instance.
    */
	IMPORT_C static CNSmlDbCaps* NewLC();

	/**
    * C++ destructor.
    */
	IMPORT_C ~CNSmlDbCaps();

	// These structs are used by sync agent

	/**
    * Returns pointer to datastore structure.
    * @return sml_devinf_datastore_s* Pointer to datastore structure.
    */
	IMPORT_C sml_devinf_datastore_s* DatastoreL() const;

	/**
    * Returns pointer to ctcap structure.
    * @return sml_devinf_ctcaplist_s* Pointer to ctcap structure.
    */
	IMPORT_C sml_devinf_ctcaplist_s* CtCaps() const;

// 1.2 CHANGES: FilterCap
    /**
    * Returns pointer to filtercap structure.
    * @return sml_devinf_filtercaplist_s* Pointer to filtercap structure.
    */
    IMPORT_C sml_devinf_filtercaplist_s* FilterCaps() const;
// changes end

	/**
    * Checks whether given synctype is supported or not.
	* @param aType Synctype to check.
    * @return TBool ETrue if Synctype is supported, EFalse otherwise.
    */
	IMPORT_C TBool SupportsSyncType( ENSmlSyncTypes aType ) const;
	
	/**
    * Creates instance of CtCaps handler.
    * @return CNSmlCtCapsHandler* Pointer to newly created CtCaps handler.
    */
	IMPORT_C CNSmlCtCapsHandler* CreateHandlerL() const;
	// DataStore element

	/**
    * Sets RxPref element.
    * @param aCtType Value of CtType as descriptor.
    * @param aVerCt Value of VerCt as descriptor.
    */
	IMPORT_C void SetRxPrefL( const TDesC8& aCtType, const TDesC8& aVerCt );

	/**
    * Adds Rx element.
    * @param aCtType Value of CtType as descriptor.
    * @param aVerCt Value of VerCt as descriptor.
    */
	IMPORT_C void AddRxL( const TDesC8& aCtType, const TDesC8& aVerCt );
	
	/**
    * Sets TxPref element.
    * @param aCtType Value of CtType as descriptor.
    * @param aVerCt Value of VerCt as descriptor.
    */
	IMPORT_C void SetTxPrefL( const TDesC8& aCtType, const TDesC8& aVerCt );
	
	/**
    * Adds Tx element.
    * @param aCtType Value of CtType as descriptor.
    * @param aVerCt Value of VerCt as descriptor.
    */
	IMPORT_C void AddTxL( const TDesC8& aCtType, const TDesC8& aVerCt );
	
	/**
    * Enables synctype (does not clear other settings!).
    * @param aType Syntype to enable.
    */
	IMPORT_C void SetSyncType( ENSmlSyncTypes aType );

// 1.2 CHANGES: Filter-Rx & FilterCap
	/**
    * Adds Filter-Rx element.
    * @param aCtType Value of CtType as descriptor.
    * @param aVerCt Value of VerCt as descriptor.
    */
    IMPORT_C void AddFilterRxL( const TDesC8& aCtType, const TDesC8& aVerCt );

    /**
    * Creates new FilterCap element instance, also leaves newly created instance into cleanupstack.
    * @param aCtType Value of CtType as descriptor.
    * @param aVerCt Value of VerCt as descriptor.
    * @return CNSmlFilterCap* Pointer to newly created instance.
    */
    IMPORT_C  CNSmlFilterCap* AddFilterCapLC( const TDesC8& aCtType, const TDesC8& aVerCt );

	/**
    * Creates instance of FilterCaps handler.
    * @return CNSmlFilterCapsHandler* Pointer to newly created FilterCaps handler.
    */
	IMPORT_C CNSmlFilterCapsHandler* CreateFilterCapsHandlerL() const;

// changes end

// 1.2 CHANGES: Setting SupportHierarchicalSync, SourceRef, DisplayName, MaxGuidSize
	/**
    * Sets SupportHierarchicalSync element.
    * Indicates if hierarchical synchronization is supported.
    */
    IMPORT_C void SetSupportHierarchicalSyncL();

	/**
    * Sets SourceRef element.
    * @param aSourceRef Value of SourceRef as descriptor.
    */
    IMPORT_C void SetSourceRefL( const TDesC8& aSourceRef );

	/**
    * Sets DisplayName element.
    * @param aDispName Value of DisplayName as descriptor.
    */
    IMPORT_C void SetDisplayNameL( const TDesC8& aDispName );

    /**
    * Sets MaxGuidSize element.
    * @param aMaxGuid Value of MaxGuidSize as descriptor.
    */
    IMPORT_C void SetMaxGuidSizeL( const TDesC8& aMaxGuid );

// changes end                                
	// CTCap element
	/**
    * Creates new CtCap element instance, also leaves newly created instance into cleanupstack.
    * @return CNSmlCtCap* Pointer to newly created instance.
    */
	IMPORT_C CNSmlCtCap* AddCtCapLC();

// 1.2 HYBRID CHANGES: Device info conversion
	/**
    * Creates version 1.1 CtCap element from version 1.2 Ctcap element
    * @return error code if conversion fails or is not possible
    */
	
	IMPORT_C TInt ConvertDeviceInfoL( sml_devinf_devinf_s* aDeviceInfo);
// changes end

	/**
    * sets maxitems element. Same as: iDatastore->dsmem->maxid
    * @param aMaxItems. Max items. Contains a number.
    */
	void SetMaxItemsL( const TDesC8& aMaxItems );
	
	/**
    * sets maxsize element. Same as: iDatastore->dsmem->maxmem
    * @param aMaxSize. Max size. Contains a number.
    */
	void SetMaxSizeL( const TDesC8& aMaxSize );

	/**
    * Returns pointer to datastore structure. No export needed, only needed inside the dll.
    * @return sml_devinf_datastore_s* Pointer to datastore structure.
    */
	const sml_devinf_datastore_s* Datastore() const;
	
	// 1.2 CHANGES: FilterCap
    /**
    * Returns pointer to filtercap structure. No export needed, only needed inside the dll.
    * @return sml_devinf_filtercaplist_s* Pointer to filtercap structure.
    */
    const sml_devinf_filtercaplist_s* FilterCapsList() const;
// changes end

protected:
	void ConstructL();
	CNSmlDbCaps();
	sml_pcdata_s* CreatePcdataL( const TDesC8& aData );
	sml_devinf_xmit_s* CreateXmitL( const TDesC8& aCtType, const TDesC8& aVerCt );
    
private:
	TUint32 iSyncTypes;
    TInt32 iPublicId;
	mutable sml_devinf_datastore_s* iDatastore;
	mutable sml_devinf_ctcaplist_s* iCtCaps;
    mutable sml_devinf_filtercaplist_s* iFilterCaps;
	};

// ------------------------------------------------------------------------------------------------
// CNSmlDbMetaHandler
// ------------------------------------------------------------------------------------------------
struct sml_metinf_metinf_s;

class CNSmlDbMetaHandler : public CBase
	{
public:
	/**
    * C++ constructor.
    */
	IMPORT_C CNSmlDbMetaHandler();
	
	/**
    * C++ constructor.
    * @param aMetInf Pointer to meta information structure.
    */
	IMPORT_C CNSmlDbMetaHandler( sml_metinf_metinf_s* aMetInf );

	/**
    * Returns value of Format element as 8-bit descriptor.
    * @return TPtrC8 Value of Format element.
    */
	IMPORT_C TPtrC8 Format() const;

	/**
    * Returns value of Type element as 8-bit descriptor.
    * @return TPtrC8 Value of Type element.
    */
	IMPORT_C TPtrC8 Type() const;

	/**
    * Returns value of Mark element as 8-bit descriptor.
    * @return TPtrC8 Value of Mark element.
    */
	IMPORT_C TPtrC8 Mark() const;

	/**
    * Returns value of Size element as 8-bit descriptor.
    * @return TPtrC8 Value of Size element.
    */
	IMPORT_C TPtrC8 Size() const;

	/**
    * Returns value of NextNonce element as 8-bit descriptor.
    * @return TPtrC8 Value of NextNonce element.
    */
	IMPORT_C TPtrC8 NextNonce() const;

	/**
    * Returns value of Version element as 8-bit descriptor.
    * @return TPtrC8 Value of Version element.
    */
	IMPORT_C TPtrC8 Version() const;

	/**
    * Returns value of MaxMsgSize element as 8-bit descriptor.
    * @return TPtrC8 Value of MaxMsgSize element.
    */
	IMPORT_C TPtrC8 MaxMsgSize() const;

	/**
    * Returns value of MaxObjSize element as 8-bit descriptor.
    * @return TPtrC8 Value of MaxObjSize element.
    */
	IMPORT_C TPtrC8 MaxObjSize() const;

	/**
    * Returns value of Mem/Shared element as 8-bit descriptor.
    * @return TPtrC8 Value of Mem/Shared element.
    */
	IMPORT_C TPtrC8 MemShared() const;

	/**
    * Returns value of Mem/Free element as 8-bit descriptor.
    * @return TPtrC8 Value of Mem/Free element.
    */
	IMPORT_C TPtrC8 MemFreeMem() const;

	/**
    * Returns value of Mem/FreeID element as 8-bit descriptor.
    * @return TPtrC8 Value of Mem/FreeID element.
    */
	IMPORT_C TPtrC8 MemFreeID() const;

	/**
    * Returns value of Anchor/Next element as 8-bit descriptor.
    * @return TPtrC8 Value of Anchor/Next element.
    */
	IMPORT_C TPtrC8 AnchorNext() const;

	/**
    * Returns value of Anchor/Last element as 8-bit descriptor.
    * @return TPtrC8 Value of Anchor/Last element.
    */
	IMPORT_C TPtrC8 AnchorLast() const;

	/**
    * Returns values of EMI element as array.
    * @param aEMI On return contains values of EMI element.
    */
	IMPORT_C void GetEmiL( CDesC8Array& aEMI ) const;

private:
	sml_metinf_metinf_s* iMetInf; // does not own this!
	};

// ------------------------------------------------------------------------------------------------
// CNSmlDbMeta
// ------------------------------------------------------------------------------------------------
class CNSmlDbMeta : public CBase
	{
public:
	IMPORT_C static CNSmlDbMeta* NewL();
	IMPORT_C static CNSmlDbMeta* NewLC();
	IMPORT_C ~CNSmlDbMeta();

	// These structs are used by sync agent
	IMPORT_C sml_metinf_metinf_s* MetInfL() const;
	IMPORT_C CNSmlDbMetaHandler* CreateHandlerL() const;
	IMPORT_C void Clear();

	/**
    * Sets value of Format element.
    * @param aValue Value of Format element as 8-bit descriptor.
    */
	IMPORT_C void SetFormatL( const TDesC8& aValue );

	/**
    * Sets value of Type element.
    * @param aValue Value of Type element as 8-bit descriptor.
    */
	IMPORT_C void SetTypeL( const TDesC8& aValue );

	/**
    * Sets value of Mark element.
    * @param aValue Value of Mark element as 8-bit descriptor.
    */
	IMPORT_C void SetMarkL( const TDesC8& aValue );

	/**
    * Sets value of Size element.
    * @param aValue Value of Size element as 8-bit descriptor.
    */
	IMPORT_C void SetSizeL( const TDesC8& aValue );

	/**
    * Sets value of NextNonce element.
    * @param aValue Value of NextNonce element as 8-bit descriptor.
    */
	IMPORT_C void SetNextNonceL( const TDesC8& aValue );

	/**
    * Sets value of Version element.
    * @param aValue Value of Version element as 8-bit descriptor.
    */
	IMPORT_C void SetVersionL( const TDesC8& aValue );

	/**
    * Sets value of MaxMsgSize element.
    * @param aValue Value of MaxMsgSize element as 8-bit descriptor.
    */
	IMPORT_C void SetMaxMsgSizeL( const TDesC8& aValue );

	/**
    * Sets value of MaxObjSize element.
    * @param aValue Value of MaxObjSize element as 8-bit descriptor.
    */
	IMPORT_C void SetMaxObjSizeL( const TDesC8& aValue );

	/**
    * Sets value of Mem/Shared element.
    * @param aValue Value of Mem/Shared element as 8-bit descriptor.
    */
	IMPORT_C void SetMemSharedL( const TDesC8& aValue );

	/**
    * Sets value of Mem/FreeMem element.
    * @param aValue Value of Mem/FreeMem element as 8-bit descriptor.
    */
	IMPORT_C void SetMemFreeMemL( const TDesC8& aValue );

	/**
    * Sets value of Mem/FreeID element.
    * @param aValue Value of Mem/FreeID element as 8-bit descriptor.
    */
	IMPORT_C void SetMemFreeIDL( const TDesC8& aValue );

	/**
    * Sets value of Anchor/Next element.
    * @param aValue Value of Anchor/Next element as 8-bit descriptor.
    */
	IMPORT_C void SetAnchorNextL( const TDesC8& aValue );

	/**
    * Sets value of Anchor/Last element.
    * @param aValue Value of Anchor/Last element as 8-bit descriptor.
    */
	IMPORT_C void SetAnchorLastL( const TDesC8& aValue );

	/**
    * Adds EMI elements.
    * @param aValue Array of EMI element values.
    */
	IMPORT_C void SetEmiL( const CDesC8Array& aValue );
	
protected:
	void MakeSureMetInfExistsL();
	void ConstructL();
	CNSmlDbMeta();

private:
	mutable sml_metinf_metinf_s* iMetInf;
	};

#endif // __NSMLDBCAPS_H__

// End of File
