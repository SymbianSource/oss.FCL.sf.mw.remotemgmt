/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/


#ifndef OPERATION_PARSER_CONSTANTS_HEADER__
#define OPERATION_PARSER_CONSTANTS_HEADER__

namespace OperationParserConstants
{
	//elements
	_LIT8( Package, "Package");
	_LIT8( Operation, "Operation");
	_LIT8( Data, "Data");

	//attributes
	_LIT8( ActionId, "action_id");
	_LIT8( TargetId, "target_id");
	_LIT8( UseBearerCert, "use_bearer_certificate");
	_LIT8( AddServerId, "add_server_id");
	_LIT8( RemoveServerId, "remove_server_id");
	_LIT8( ResetAllTag, "*");
	
	//values
	_LIT8( True, "true");
	_LIT8( False, "false");
	

	//Operations
	_LIT8( Add, "Add");
	_LIT8( Remove, "Remove");
	_LIT8( Replace, "Replace");
	
	const TPtrC8 XMLStack[3] = { Package(), Operation(), Data()};
	
}

#endif