/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definitions for SyncML toolkit types.
*
*/



#ifndef __SMLDEF_H__
#define __SMLDEF_H__

/** 
 * ==================================
 * Basic data types 
 * ==================================
 **/
typedef short			Short_t;		// short integer,  16 Bytes
typedef long			Long_t;			// long integer,   32 Bytes
typedef char*			String_t;		// String pointer, 
typedef unsigned char	Byte_t;			// a single byte
typedef Byte_t			Boolean_t;		// a boolean

typedef int				Ret_t;        // Return Type of API Commands
typedef Long_t			Length_t;     // System dependent string length
typedef Short_t			MemHandle_t;  // Memory object Handle 
typedef unsigned char	*MemPtr_t;    // Memory object Pointer
typedef void			*VoidPtr_t;   // Void Pointer
typedef Long_t			MemSize_t;    // System dependent memory object size
typedef unsigned char	MemByte_t;    // Memory element
typedef unsigned int	Flag_t;       // A generic flag type. This type is used to 
                                    // declare variables in structures wherever  
                                    // flags are used.

/**
 * ==================================
 * Definitions used in the SyncML API
 * ==================================
 **/

/**
 * Application callback function displaying output strings to the user
 **/ 
typedef void (*smlPrintFunc) (String_t outputString);

/** 
 * structure describing the options and setting of this syncml process 
 **/
typedef struct sml_options_s {
  smlPrintFunc  defaultPrintFunc;       // default application callback for displaying strings,
  MemSize_t     maxWorkspaceAvailMem;   // size which all workspaces in total MUST not exceed
} *SmlOptionsPtr_t, SmlOptions_t;

/**
 * Reference of an instance
 **/ 
typedef MemHandle_t InstanceID_t;    // Handle, used as a unique ID of 
                                     // an synchronization instance 
/**
 * Type of used encoding
 **/
typedef enum {
	SML_UNDEF = 0,
        SML_WBXML,
        SML_XML
} SmlEncoding_t;

/** 
 * structure describing the options of an instance, 
 **/
typedef struct sml_instance_options_s {
  SmlEncoding_t encoding;               // Used encoding type,
  MemSize_t     workspaceSize;          // size of the workspace to allocate
  String_t      workspaceName;          // name of the workspace
} *SmlInstanceOptionsPtr_t, SmlInstanceOptions_t;

/**
 * Processing modes
 **/
typedef enum {
  SML_DO_NOTHING = 0,
  SML_FIRST_COMMAND,
  SML_NEXT_COMMAND,
  SML_NEXT_MESSAGE,
  SML_ALL_COMMANDS
} SmlProcessMode_t;

/**
 * Requested buffer pointer position
 **/
typedef enum {
  SML_FIRST_DATA_ITEM = 0,
  SML_FIRST_FREE_ITEM
} SmlBufPtrPos_t;

/**
 * SyncML Protocol Management and Command Elements (PE)
 **/
typedef enum {
    SML_PE_UNDEF = 0,
    SML_PE_ERROR,
    SML_PE_ADD,
    SML_PE_ALERT,
    SML_PE_ATOMIC_START,
    SML_PE_ATOMIC_END,
    SML_PE_COPY,
    SML_PE_DELETE,
    SML_PE_EXEC,
    SML_PE_GET,
    SML_PE_MAP,
	SML_PE_MOVE, // 1.2 CHANGES: Move added
    SML_PE_PUT,
    SML_PE_RESULTS,
    SML_PE_SEARCH,
    SML_PE_SEQUENCE_START,
    SML_PE_SEQUENCE_END,
    SML_PE_STATUS,
    SML_PE_SYNC_START,
    SML_PE_SYNC_END,
    SML_PE_REPLACE,
    SML_PE_HEADER,
    SML_PE_PUT_GET,
    SML_PE_CMD_GROUP,
    SML_PE_GENERIC,
    SML_PE_FINAL
} SmlProtoElement_t;

#define SML_PE_SIZE_SENT (1<<0)
#define SML_PE_META_CREATED_BY_GENERATOR (1<<1)
#define SML_PE_SIZE_CREATED_BY_GENERATOR (1<<2)



#endif // __SMLDEF_H__
