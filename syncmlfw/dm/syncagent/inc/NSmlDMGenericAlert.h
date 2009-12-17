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
* Description:  Posseses all the needed fields to generate 
*                a generic alert structure
*
*/



#ifndef __NSMLDMGENERICALERT_H
#define __NSMLDMGENERICALERT_H

/**
* Class CNSmlDMGenericAlert. 
* Posseses all the needed fields to generate a generic alert structure 
* to the client init package.
*/
class CNSmlDMGenericAlert : public CBase
	{
	public:
	/**
	* Creates a new generic alert object.
	* @param aFwMgmtUri. The uri which has been used in the exe command whose final result is 
	* about to be reported.
	* @param aMetaType. Meta/Type that should be used in the alert.
	* @param aMetaFormat. Meta/Format that should be used in the alert.
	* @param aFinalResult. The final result value which is reported to remote server.
	* @param aCorrelator. Correlator value used in the original exec command.
	* @return New instance of this class.
	*/	
	static CNSmlDMGenericAlert* NewL ( const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, TInt aFinalResult, const TDesC8& aFwCorrelator );
	static CNSmlDMGenericAlert* NewL ( const TDesC8& aCorrelator, const RArray<CNSmlDMAlertItem>& aItemList  );
	/**
	* C++ destructor.
	*/	
	~CNSmlDMGenericAlert();
	
	/**
	* Inline function to retrieve a firmware management uri.
	* @return The firmware management uri.
	*/	
	inline HBufC8* FwMgmtUri() const;
	/**
	* Inline function to retrieve a meta type.
	* E.g. "org.openmobilealliance.firmwareupdate.update"
	* @return The meta type.
	*/	
	inline HBufC8* MetaType() const;
	/**
	* Inline function to retrieve a meta format.
	* E.g. "text/plain"
	* @return The meta format.
	*/	
	inline HBufC8* MetaFormat() const;
	/**
	* Inline function to retrieve a correlator of the exec command.
	* @return The correlator.
	*/	
	inline HBufC8* FwCorrelator() const;
	/**
	* Inline function to retrieve a Data.
	* @return The Data.
	*/	
	inline RArray<CNSmlDMAlertItem>* DataItem() const;
	/**
	* Inline function to retrieve a final result.
	* @return The final result value.
	*/	
	inline TInt FinalResult() const;

	private:
	/**
	* C++ constructor.
	*/	
	CNSmlDMGenericAlert();
	/**        
	* Symbian 2nd phase constructor
	*/	
	void ConstructL ( const TDesC8& aFwMgmtUri, const TDesC8& aMetaType, const TDesC8& aMetaFormat, TInt aFinalResult, const TDesC8& aFwCorrelator, const RArray<CNSmlDMAlertItem>* aDataItem = NULL );
	
	private:
	HBufC8* iFwMgmtUri;
	HBufC8* iMetaType;
	HBufC8* iMetaFormat;
	TInt iFinalResult;
	HBufC8* iFwCorrelator;
	RArray<CNSmlDMAlertItem>* iDataItem;
	};	

#include "NSmlDMGenericAlert.inl"

#endif      // __NSMLDMGENERICALERT_H
            
// End of File
