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
* Description:  Inline methods for WBXML stackitem.
*
*/


#ifndef __WBXMLPARSER_INL__
#define __WBXMLPARSER_INL__


inline TWBXMLStackItem::TWBXMLStackItem( TUint8 aTag, TUint8 aCodePage ) : iTag(aTag), iCodePage(aCodePage)
	{
	}

inline TUint8 TWBXMLStackItem::Tag() const
	{
	return iTag;
	}

inline TUint8 TWBXMLStackItem::CodePage() const
	{
	return iCodePage;
	}


#endif // __WBXMLPARSER_INL__
