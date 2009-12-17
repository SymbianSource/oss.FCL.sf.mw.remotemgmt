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
* Description:  for dbcaps streaming and converting to CSmlDataStoreFormat.
*
*/


#ifndef __NSMLDBCAPSSERIALIZER_H__
#define __NSMLDBCAPSSERIALIZER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <SmlDataFormat.h>
#include <SmlDataFormat.hrh>

// ------------------------------------------------------------------------------------------------
// Class forwards
// ------------------------------------------------------------------------------------------------
class CNSmlDbCaps;
class CNSmlDevInfProp;
class CNSmlPropParam;
class CNSmlDevInfProp;
class CNSmlCtCap;
class CNSmlFilterCap;
struct sml_devinf_propparam_s;
struct sml_devinf_property_s;
struct sml_devinf_filtercap_s;
struct sml_devinf_ctcap_s;
struct sml_pcdata_list_s;
struct sml_pcdata_s;

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer
// Handles internalization and externalization of CNSmlDbCaps to stream, that can
// be internalized to and externalized from CSmlDataStoreFormat. 
//
//	@lib nsmldbcaps.lib
// ------------------------------------------------------------------------------------------------
class TNSmlDbCapsSerializer
	{
	private:
	
		// ------------------------------------------------------------------------------------------------
		// CNSmlFilterCapabilityData
		// Data for CSmlFilterCapability
		//
		// @lib nsmldbcaps.lib
		// ------------------------------------------------------------------------------------------------
		struct CNSmlFilterCapabilityData
			{
				/**
				* constructs CNSmlFilterCapabilityData
				* @return CNSmlFilterCapabilityData*. Created instance
				*/
				static CNSmlFilterCapabilityData* NewLC();
				
				/**
				* C++ Destructor
				*/
				~CNSmlFilterCapabilityData();
				
				/**
				* fills members from array of filtercapdata
				* @param aFilterInfoArr. array of filter capabilities.
				*/
				void MakeL( const RStringPool& aStringPool, const CArrayFix<TNSmlFilterCapData>& aFilterInfoArr );
				
			public: //Data
			
				RPointerArray<CSmlFilterCapability> iFilterCapabilities;
				RArray<RString> iKeywordList;
				RArray<RString> iPropertyList;
				RStringF iMimeVersion;
				RStringF iMimeType;
			};

		// ------------------------------------------------------------------------------------------------
		// CNSmlDataFieldFromDbCaps
		// inherited from CSmlDataField to have access to protected member variables.
		//
		// @lib nsmldbcaps.lib
		// ------------------------------------------------------------------------------------------------
		class CNSmlDataFieldFromDbCaps : public CSmlDataField
			{
		public:
		
			/**
		    * constructs CSmlDataField from parameters
		    * @param aStringPool. string pool to use
		    * @param aDisplayName. display name- field
		    * @param aPropName. property name- field
		    * @param aDataType. datatype name- field
		    * @param aValEnums. value enumerations
		    * @return CSmlDataField*. created instance.
		    */
			static CSmlDataField* NewLC(
				const RStringPool& aStringPool,
				const TPtrC8& aDisplayName, 
				const TPtrC8& aPropName,
				const TPtrC8& aDataType,
				const sml_pcdata_list_s* aValEnums);

			};
			
		// ------------------------------------------------------------------------------------------------
		// CNSmlDataPropertyParamFromDbCaps
		// inherited from CSmlDataPropertyParam to have access to protected member variables.
		//
		// @lib nsmldbcaps.lib
		// ------------------------------------------------------------------------------------------------
		class CNSmlDataPropertyParamFromDbCaps : CSmlDataPropertyParam
			{
		public:
		
			/**
		    * constructs CSmlDataPropertyParam from parameters
		    * @param aStringPool. string pool to use
		    * @param aDipp. property parameters
		    * @return CSmlDataPropertyParam*. created instance.
		    */
			static CSmlDataPropertyParam* NewLC( const RStringPool& aStringPool, 
				const sml_devinf_propparam_s* aDipp );
			};

		// ------------------------------------------------------------------------------------------------
		// CNSmlDataPropertyFromDbCaps
		// inherited from CSmlDataProperty to have access to protected member variables.
		//
		// @lib nsmldbcaps.lib
		// ------------------------------------------------------------------------------------------------
		class CNSmlDataPropertyFromDbCaps : CSmlDataProperty
			{
		public:
		
			/**
		    * constructs CSmlDataProperty from parameters
		    * @param aStringPool. string pool to use
		    * @param aDipp. properties
		    * @return CSmlDataProperty*. created instance.
		    */
			static CSmlDataProperty* NewLC( const RStringPool& aStringPool, const sml_devinf_property_s* aDipp );
			};

		// ------------------------------------------------------------------------------------------------
		// CNSmlFilterCapabilityFromDbCaps
		// inherited from CSmlFilterCapability to have access to protected member variables.
		//
		// @lib nsmldbcaps.lib
		// ------------------------------------------------------------------------------------------------
		class CNSmlFilterCapabilityFromDbCaps : CSmlFilterCapability
			{
		public:
		
			/**
		    * constructs CSmlFilterCapability from parameters
		    * @param aStringPool. string pool to use
		    * @param aFtCap. filter capabilities
		    * @return CSmlFilterCapability*. created instance.
		    */
			static CSmlFilterCapability* NewLC( const RStringPool& aStringPool, const sml_devinf_filtercap_s* aFtCap );
			};

		// ------------------------------------------------------------------------------------------------
		// CNSmlMimeFormatFromDbCaps
		// inherited from CSmlMimeFormat to have access to protected member variables.
		//
		// @lib nsmldbcaps.lib
		// ------------------------------------------------------------------------------------------------
		class CNSmlMimeFormatFromDbCaps : CSmlMimeFormat
			{
		public:

			/**
		    * constructs CSmlMimeFormat from parameters
		    * @param aStringPool. string pool to use
		    * @param aDic. ctcap.
		    * @return CSmlMimeFormat*. created instance.
		    */
			static CSmlMimeFormat* NewLC( const RStringPool& aStringPool, const sml_devinf_ctcap_s& aDic );
			};
		// ------------------------------------------------------------------------------------------------
		// CNSmlDataStoreFormatFromDbCaps
		// inherited from CSmlDataStoreFormat to have access to protected member variables.
		//
		// @lib nsmldbcaps.lib
		// ------------------------------------------------------------------------------------------------
		class CNSmlDataStoreFormatFromDbCaps : CSmlDataStoreFormat
			{
		public:

			/**
		    * constructs CSmlDataStoreFormat from parameters
		    * @param aStringPool. string pool to use
		    * @param aDbCaps. Db caps
		    * @return CSmlDataStoreFormat*. created instance.
		    */
			static CSmlDataStoreFormat* NewLC( const RStringPool& aStringPool, const CNSmlDbCaps& aDbCaps );
			};
			
	public: //Constructor
		IMPORT_C TNSmlDbCapsSerializer();

		/**
		* Internalizes from data store stream to dbcaps instance.
		* @param aDataStoreFormatStream. Store format stream.
		* @return CNSmlDbCaps*. Instance internalized from stream.
		*/
		IMPORT_C CNSmlDbCaps* InternalizeL( RReadStream& aDataStoreFormatStream ) const;
		
		/**
		* externalizes from dbcaps to data store stream
		* @param aDbCaps. Object to externalize
		* @param aDataStoreFormatStream. Stream, where externalized
		*/
		IMPORT_C void ExternalizeL( const CNSmlDbCaps& aDbCaps, RWriteStream& aDataStoreFormatStream ) const;
		/**
		* externalizes from dbcaps to data store stream
		* @param const CArrayFix<TNSmlFilterCapData>& aFilterInfoArr. filter info to externalize.
		* @param aDataStoreFormatStream. Stream, where externalized
		*/		
		IMPORT_C void ExternalizeL( const CNSmlDbCaps& aDbCaps, const CArrayFix<TNSmlFilterCapData>& aFilterInfoArr, RWriteStream& aDataStoreFormatStream ) const;
		
		/**
		* copies and converts data from aFrom to aTo
		* @param aFrom. Source Object
		* @param aTo. Destination Object
		*/	
		IMPORT_C void SetFromL( const CSmlDataProperty& aFrom, CNSmlDevInfProp& aTo ) const;
		
		/**
		* checks for null before returning data pointer
		* @param aPcdata. place to extract pointer.
		* @return. safe data pointer
		*/
		static TPtrC8 SafePtr( sml_pcdata_s* aPcdata );
		
	private:

		/**
		* copies and converts data from aFrom to aTo
		* @param aFrom. Source Object
		* @param aTo. Destination Object
		*/
		void SetFromL( const CSmlDataStoreFormat& aFrom, CNSmlDbCaps& aTo ) const;

		/**
		* copies and converts data from aFrom to aTo
		* @param aFrom. Source Object
		* @param aTo. Destination Object
		*/
		void SetFromL( const CSmlDataPropertyParam& aFrom, CNSmlPropParam& aTo ) const;
		
		/**
		* copies and converts data from aFrom to aTo
		* @param aFrom. Source Object
		* @param aTo. Destination Object
		*/
		void SetFromL( const CSmlMimeFormat& aFrom, CNSmlCtCap& aTo ) const;
		
		/**
		* copies and converts data from aFrom to aTo
		* @param aFrom. Source Object
		* @param aTo. Destination Object
		*/
		void SetFromL( const CSmlFilterCapability& aFrom, CNSmlFilterCap& aTo ) const;
	private:
	};
	
#endif //__NSMLDBCAPSSERIALIZER_H__

// End of File
