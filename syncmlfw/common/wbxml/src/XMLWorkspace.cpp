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
* Description:  Base workspace class implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "XMLWorkspace.h"

// ------------------------------------------------------------------------------------------------
// CXMLWorkspace
// ------------------------------------------------------------------------------------------------
void CXMLWorkspace::ConstructL()
	{
	iTransactions = new (ELeave) RArray<TUint>();
	}

// ------------------------------------------------------------------------------------------------
CXMLWorkspace::~CXMLWorkspace()
	{
	if( iTransactions )
		{
		iTransactions->Reset();
		}
	delete iTransactions;
	}

// ------------------------------------------------------------------------------------------------
void CXMLWorkspace::BeginTransaction()
	{
	iTransactions->Append(Size());
	}

// ------------------------------------------------------------------------------------------------
TInt CXMLWorkspace::Commit()
	{
	if( iTransactions->Count() == 0 )
		{
		return KErrGeneral;
		}
	iTransactions->Remove(iTransactions->Count() - 1);
	return KErrNone;
	}

// ------------------------------------------------------------------------------------------------
void CXMLWorkspace::CommitAll()
	{
	iTransactions->Reset();
	}

// ------------------------------------------------------------------------------------------------
TInt CXMLWorkspace::Rollback()
	{
	if( iTransactions->Count() == 0 )
		{
		return KErrGeneral;
		}
	TUint begin = iTransactions->operator[](iTransactions->Count() - 1);
	Delete(begin, Size() - begin);
	iTransactions->Remove(iTransactions->Count() - 1);
	return KErrNone;
	}
