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
* Description:  Inline methods for WBXML TXMLElementParams.
*
*/


#ifndef __XMLELEMENT_INL__
#define __XMLELEMENT_INL__

// ------------------------------------------------------------------------------------------------
inline TXMLElementParams::TXMLElementParams() : iCallbacks(0), iCmdStack(0), iCleanupStack(0)
	{
	}

// ------------------------------------------------------------------------------------------------
inline TAny* TXMLElementParams::Callbacks() const
	{
	return iCallbacks;
	}

// ------------------------------------------------------------------------------------------------
inline CNSmlStack<CXMLElement>* TXMLElementParams::CmdStack() const
	{
	return iCmdStack;
	}

// ------------------------------------------------------------------------------------------------
inline CNSmlStack<CXMLElement>* TXMLElementParams::CleanupStack() const
	{
	return iCleanupStack;
	}

#endif // __XMLELEMENT_INL__