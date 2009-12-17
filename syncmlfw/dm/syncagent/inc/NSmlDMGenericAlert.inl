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
* Description:  Implementation file to retrieve all the needed fields of the 
*                 generic alert structure.
*
*/


#ifndef __NSMLDMGENERICALERT_INL__
#define __NSMLDMGENERICALERT_INL__

// -----------------------------------------------------------------------------
// CNSmlDMGenericAlert::FwMgmtUri()
// -----------------------------------------------------------------------------
//
HBufC8* CNSmlDMGenericAlert::FwMgmtUri() const	
	{
	return iFwMgmtUri;
	}
	
// -----------------------------------------------------------------------------
// CNSmlDMGenericAlert::MetaType()
// -----------------------------------------------------------------------------
//
HBufC8* CNSmlDMGenericAlert::MetaType() const	
	{
	return iMetaType;
	}

// -----------------------------------------------------------------------------
// CNSmlDMGenericAlert::MetaFormat()
// -----------------------------------------------------------------------------
//
HBufC8* CNSmlDMGenericAlert::MetaFormat() const	
	{
	return iMetaFormat;
	}

// -----------------------------------------------------------------------------
// CNSmlDMGenericAlert::FwCorrelator()
// -----------------------------------------------------------------------------
//
HBufC8* CNSmlDMGenericAlert::FwCorrelator() const	
	{
	return iFwCorrelator;
	}

// -----------------------------------------------------------------------------
// CNSmlDMGenericAlert::ItemListData()
// -----------------------------------------------------------------------------
//
RArray<CNSmlDMAlertItem>* CNSmlDMGenericAlert::DataItem() const	
	{
	return iDataItem;
	}
		
// -----------------------------------------------------------------------------
// CNSmlDMGenericAlert::FinalResult()
// -----------------------------------------------------------------------------
//
TInt CNSmlDMGenericAlert::FinalResult() const	
	{
	return iFinalResult;
	}

#endif

// End of File
