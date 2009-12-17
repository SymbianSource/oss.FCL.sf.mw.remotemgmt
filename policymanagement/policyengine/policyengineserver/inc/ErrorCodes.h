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


#ifndef ERROR_CODES_HEADER_
#define ERROR_CODES_HEADER_

// INCLUDES
// CONSTANTS

const TInt KErrParser 		 =	-91300;
const TInt KErrOpParser 	 = 	-91301;
const TInt KErrPolicyManager =	-91302;
const TInt KErrPolicyStorage =	-91303;
const TInt KErrNativeParser  =	-91304;

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

namespace Panics
{
	_LIT( PolicyStoragePanic, "Policy storage panic");	
	_LIT( PolicyManagerPanic, "Policy manager panic");	
	_LIT( ElementReserverPanic, "Element reserver panic!");	
	_LIT( PolicyParserPanic, "Policy parser panic");	
	_LIT( OperationParserPanic, "Operation parser panic");	
	_LIT( PolicyExecutionPanic, "Policy execution panic");	
	_LIT( SettingEnforcementManagerPanic, "Setting enforcement manager panic");	
	_LIT( TrustedSessionManagerPanic, "Trusted session manager panic");	
}

namespace ParserErrors
{
	_LIT8( ParsingOk, "Parsing Ok");	
	_LIT8( SyntaxError, "Syntax error" );
	_LIT8( InvalidMark, "Invalid mark" );
	_LIT8( InvalidMarks, "| or ~" );
	_LIT8( InvalidElement, "Invalid Element" );
	_LIT8( UnexpectedElement, "Unexpected element" );
	_LIT8( UnexpectedAttribute, "Unexpected attribute" );	
	_LIT8( UnexpectedContent, "Unexpected content" );	
//	_LIT8( UnexpectedType, "Unexpected Type" );	
//	_LIT8( InvalidType,	"Invalid Type");
	_LIT8( InvalidValue,	"Invalid Value");
	
	_LIT8( IncombatibleDataType, "Incombatible datatype");	
	_LIT8( IncombatibleParameters, "Incombatible parameters");	
}

namespace ManagementErrors
{
	_LIT8( OperationOk, "Operation Ok");	
	_LIT8( IdAlreadyExist, "Id already exist");	
	_LIT8( InvalidCertificateMapping, "Invalid certificate mapping");	
	_LIT8( ElementNotFound, "Element not found");	
//	_LIT8( NotAllowed,		"Operation is not allowed");
//	_LIT8( PolicyEditorBusy, "Policy editor is busy");	
//	_LIT8( PolicySystemCorrupted, "Policy system is corrupted");	
//	_LIT8( CantAddElementsToRepository, "Can't add elements to repository");
//	_LIT8( ResourceAllocationProblem, "Resource allocation problem");
//	_LIT8( InvalidPolicyElement, "Invalid policy element");
	_LIT8( SessionMustBeCertificated, "Session must be certificated!");
	_LIT8( AccessDenied, "Access denied");
	_LIT8( ServerIdManagementError, "Server ID management error");
	
	
//	_LIT8( InvalidOperation, "Invalid operation");
//	_LIT8( OperationFailed, "Operation failed");
}

namespace RequestErrors
{
	_LIT8( MissingAttribute, "Missing attribute");	
	_LIT8( FunctionIsNotAllowed, "Function is not allowed");
}


#endif

// CLASS DECLARATION