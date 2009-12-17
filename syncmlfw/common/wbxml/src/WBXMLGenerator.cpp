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
* Description:  Generic WBXML generator class implementation.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "WBXMLGenerator.h"
#include "WBXMLDefs.h"
#include "XMLStaticWorkspace.h"
#include "XMLDynamicWorkspace.h"
#include "WBXMLGeneratorError.h"

// ------------------------------------------------------------------------------------------------
// CWBXMLGenerator
// ------------------------------------------------------------------------------------------------
EXPORT_C TPtrC8 CWBXMLGenerator::Document()
	{
	return Workspace()->Buffer();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::Reset()
	{
	Workspace()->Reset();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::CreateStaticWorkspaceL( TInt aBufferSize )
	{
	FreeWorkspace();
	iWorkspace = CXMLStaticWorkspace::NewL(aBufferSize);
	iOwnsWorkspace = ETrue;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::CreateDynamicWorkspaceL()
	{
	FreeWorkspace();
	iWorkspace = CXMLDynamicWorkspace::NewL();
	iOwnsWorkspace = ETrue;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CXMLWorkspace* CWBXMLGenerator::Workspace() const
	{
	return iWorkspace;
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::SetWorkspace( CXMLWorkspace* aBuffer, TBool aTakeOwnership )
	{	
	FreeWorkspace();
	iOwnsWorkspace = aTakeOwnership;
	iWorkspace = aBuffer;
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLGenerator::FreeWorkspace()
	{
	if( iOwnsWorkspace )
		{
		delete iWorkspace;
		iWorkspace = 0;
		}
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLGenerator::CWBXMLGenerator()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C CWBXMLGenerator::~CWBXMLGenerator()
	{
	FreeWorkspace();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::BeginDocumentL( TUint8 aVersion, TInt32 aPublicId, TUint32 aCharset, const TDesC8& aStringTbl )
	{
	Workspace()->WriteL(aVersion);

	if( aPublicId <= 0 )
		{
		Workspace()->WriteL(0);
		WriteMUint32L(-aPublicId);
		}
	else
		{
		WriteMUint32L(aPublicId);
		}
	WriteMUint32L(aCharset);
	WriteMUint32L(aStringTbl.Size());
	Workspace()->WriteL(aStringTbl);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::BeginElementL( TUint8 aElement, TBool aHasContent, TBool aHasAttributes )
	{
	if( aHasAttributes )
		{
		aElement |= KWBXMLHasAttributes;
		}
	if( aHasContent )
		{
		aElement |= KWBXMLHasContent;
		}
	WriteMUint32L(aElement);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::EndElementL()
	{
	Workspace()->WriteL(END);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::AddElementL( TUint8 aElement, const TDesC8& aContent, const TWBXMLContentFormat aFormat )
	{
	BeginElementL(aElement, ETrue);
	if( aFormat == EWBXMLContentFormatOpaque )
		{
		WriteOpaqueDataL(aContent);
		}
	else
		{
		WriteInlineStringL(aContent);
		}
	EndElementL();
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::AddElementL( TUint8 /*aElement*/, const TDesC8& /*aContent*/, const CWBXMLAttributes& /*aAttributes*/, const TWBXMLContentFormat /*aFormat*/ )
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::AddEntityL( TUint32 aEntity )
	{
	Workspace()->WriteL(ENTITY);
	WriteMUint32L(aEntity);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::AddExt_IL( TInt aExtNum, const TDesC8& aContent )
	{
	Workspace()->WriteL(TUint8(EXT_I_0 + aExtNum));
	WriteInlineStringL(aContent);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::AddExt_TL( TInt aExtNum, TUint32 aContent )
	{
	Workspace()->WriteL(TUint8(EXT_T_0 + aExtNum));
	WriteMUint32L(aContent);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::AddExtL( TInt aExtNum )
	{
	Workspace()->WriteL(TUint8(EXT_0 + aExtNum));
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C void CWBXMLGenerator::SwitchCodePageL( TUint8 aNewCodePage )
	{
	Workspace()->WriteL(SWITCH_PAGE);
	Workspace()->WriteL(aNewCodePage);
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CWBXMLGenerator::HandleResult( TInt aResult, TInt aTreshold )
	{
	switch( aResult )
		{
		case KErrNone:
			if( Workspace()->FreeSize() < aTreshold )
				{				
				Workspace()->Rollback();
				return KWBXMLGeneratorBufferFull;
				}
			Workspace()->Commit();
			return KWBXMLGeneratorOk;

		case KErrTooBig:
			Workspace()->Rollback();
			return KWBXMLGeneratorBufferFull;

		default:
			return KErrGeneral;
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLGenerator::WriteMUint32L( TUint32 aValue )
	{
	TUint8 temp[5];
	TInt i(4);
	
	temp[i--] = TUint8(aValue & 0x7F);
	aValue >>= 7;
	while( aValue > 0 )
		{
		temp[i--] = TUint8((aValue & 0x7F) | 0x80);
		aValue >>= 7;
		}
			
	while( i < 4 )
		{
		Workspace()->WriteL(temp[++i]);
		}
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLGenerator::WriteOpaqueDataL( const TDesC8& aData )
	{
	Workspace()->WriteL(OPAQUE);
	WriteMUint32L(aData.Size());
	Workspace()->WriteL(aData);
	}

// ------------------------------------------------------------------------------------------------
void CWBXMLGenerator::WriteInlineStringL( const TDesC8& aData )
	{
	Workspace()->WriteL(STR_I);
	Workspace()->WriteL(aData);
	Workspace()->WriteL(0);
	}

