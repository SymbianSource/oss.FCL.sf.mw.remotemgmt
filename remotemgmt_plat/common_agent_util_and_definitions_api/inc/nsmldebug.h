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
* Description:  DS debug
*
*/


#ifndef __NSMLDEBUGEPOC_H__
#define __NSMLDEBUGEPOC_H__

#include "nsmldefines.h"

// Default defines - no debug flags set
//=====================================
#define DBG_FILE_CODE(i, p)
#define DBG_BEGIN()
#define DBG_FILE(p)
#define DBG_INFO(p)
#define _DBG_FILE(p)
#define _DBG_INFO(p)
#define DBG_DUMP(p,l,msg)
#define _DBG_DUMP

#ifndef __GNUC__
#define DBG_ARGS8 
#define DBG_ARGS
#else
#define DBG_ARGS8(arg...)
#define DBG_ARGS(arg...)
#endif // __GNUC__


#if defined(__NSML_DEBUG__) || defined(__NSML_DEVELOPER_DEBUG__)
//==============================================================

typedef void TAny;
typedef signed char TInt8;
typedef unsigned char TUint8;
typedef short int TInt16;
typedef unsigned short int TUint16;
typedef long int TInt32;
typedef unsigned long int TUint32;
typedef signed int TInt;
typedef unsigned int TUint;
typedef float TReal32;
typedef double TReal64;
typedef double TReal;
typedef unsigned char TText8;
typedef unsigned short int TText16;
typedef int TBool;

#if defined(__VC32__)
#define __NO_CLASS_CONSTS__
#define IMPORT_C __declspec(dllexport)
#define EXPORT_C __declspec(dllexport)
#endif
#if defined(__CW32__)
#define __NO_CLASS_CONSTS__
#define IMPORT_C __declspec(dllexport)
#define EXPORT_C __declspec(dllexport)
#endif
#if defined(__GCC32__)
#define __DESC_NO_BIT_FIELDS__
#define __DOUBLE_WORDS_SWAPPED__
#define __NO_CLASS_CONSTS__
#define IMPORT_C
#if !defined __WINS__ && defined _WIN32 // for VC++ Browser
#define EXPORT_C
#define asm(x)
#else
#define EXPORT_C __declspec(dllexport)
#endif
#define TEMPLATE_SPECIALIZATION
#endif

#define _S8(a) ((const TText8 *)a)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

IMPORT_C void NSmlDebugInitL();
IMPORT_C void NSmlDebugDumpL( void* aData, TInt aLength, TText8* aFile, TInt aLine, const TText8* aMsg );
IMPORT_C void NSmlDebugPrintL( const TText8* aMsg, TBool aPrintToFile, TText8* aFile, TInt aLine );
IMPORT_C void NSmlDebugPrint_CodeL( TInt aCode, const TText8* aMsg, TBool aPrintToFile, TText8* aFile, TInt aLine );
IMPORT_C void NSmlDebugPrintHeaderL( TText8* aFile, TInt aLine );
IMPORT_C void NSmlDebugPrintArgs8L( const TText8* aFmt, ... );
IMPORT_C void NSmlDebugPrintArgs16L( const TText16* aFmt, ... );

#ifdef __cplusplus
}
#endif // __cplusplus

#undef DBG_BEGIN
#define DBG_BEGIN() NSmlDebugInitL()
#undef	DBG_DUMP
#define DBG_DUMP(p,l,msg) NSmlDebugDumpL ( p, l, (unsigned char*)__FILE__, __LINE__, msg )

#ifndef __NSML_DEVELOPER_DEBUG__
//==============================
#undef DBG_FILE_CODE
#undef DBG_FILE
#undef DBG_INFO
#undef _DBG_FILE
#undef _DBG_INFO
#undef DBG_ARGS8 
#undef DBG_ARGS

#define DBG_FILE_CODE(i, p) NSmlDebugPrint_CodeL ( i, p, 1, (unsigned char*)__FILE__, __LINE__ )
#define DBG_FILE(p) NSmlDebugPrintL ( p, 1, (unsigned char*)__FILE__, __LINE__ )
#define DBG_INFO(p) NSmlDebugPrintL ( p, 0, (unsigned char*)__FILE__, __LINE__ )
#define _DBG_FILE(p) NSmlDebugPrintL ( _S8(p), 1, (unsigned char*)__FILE__, __LINE__ )
#define _DBG_INFO(p) NSmlDebugPrintL ( _S8(p), 0, (unsigned char*)__FILE__, __LINE__ )

#ifndef __GNUC__
#define DBG_ARGS8 NSmlDebugPrintHeaderL((unsigned char*)__FILE__, __LINE__); NSmlDebugPrintArgs8L
#define DBG_ARGS NSmlDebugPrintHeaderL((unsigned char*)__FILE__, __LINE__); NSmlDebugPrintArgs16L
#else
#define DBG_ARGS8(arg...) NSmlDebugPrintHeaderL((unsigned char*)__FILE__, __LINE__); NSmlDebugPrintArgs8L(arg);
#define DBG_ARGS(arg...) NSmlDebugPrintHeaderL((unsigned char*)__FILE__, __LINE__); NSmlDebugPrintArgs16L(arg);
#endif // __GNUC__

#endif //#ifndef (__NSML_DEVELOPER_DEBUG__)


#endif // defined(__NSML_DEBUG__) || defined(__NSML_DEVELOPER_DEBUG__)

#endif // __NSMLDEBUGEPOC_H__
