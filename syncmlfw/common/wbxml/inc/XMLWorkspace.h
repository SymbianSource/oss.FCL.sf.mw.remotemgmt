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
* Description:  Base workspace class.
*
*/


#ifndef __XMLWORKSPACE_H__
#define __XMLWORKSPACE_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>

// ------------------------------------------------------------------------------------------------
// CXMLWorkspace
// ------------------------------------------------------------------------------------------------
class CXMLWorkspace : public CBase
	{
public:
	virtual ~CXMLWorkspace();

	// transaction handling
	virtual void BeginTransaction();
	virtual void CommitAll();
	virtual TInt Commit();
	virtual TInt Rollback();

	// pure virtuals
	virtual void Reset() = 0;
	virtual void WriteL( const TDesC8& aData ) = 0;
	virtual void WriteL( const TUint8 aData ) = 0;
	virtual TPtrC8 Buffer() const = 0;

	virtual TInt FreeSize() const = 0;
	virtual TInt MaxSize() const = 0;
	virtual TInt Size() const = 0;

protected:
	void ConstructL();

private:
	// pure virtuals
	virtual void Delete( TInt aPos, TInt aLength ) = 0;

private:
	RArray<TUint>* iTransactions;
	};

#endif // __XMLWORKSPACE_H__