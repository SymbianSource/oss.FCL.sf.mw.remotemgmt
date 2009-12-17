/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Defines field values for creation of sync profile from resource files.
*
*/
// copied from syncmldef.h

enum TSmlServerAlertedAction
	{
	/** Allow the push message parser plugin to specify user interaction type. */
	ESmlUseSANRequestedSetting,
	/** Automatically run the synchronisation without user intervention. */
	ESmlEnableSync,
	/** Prevent the synchronisation. */
	ESmlDisableSync,
	/** Inform the user of the synchronisation. */
	ESmlInformSync,
	/** Get confirmation of the synchronisation from the user. */
	ESmlConfirmSync,
	/** Use the global user interaction type (only allowed for the profile setting) */
	ESmlUseGlobalSetting
	};

/**
	Flags for synchronisation types.
*/
enum TSmlSyncType
	{
	/** Two-way sync.
	
	A normal sync type in which the client and the server exchange information about modified data in these devices. 
	The client sends the modifications first.
	*/
	ESmlTwoWay,
	/** One-way sync from server.
	
	A sync type in which the client gets all modifications from the server, but the client does not send 
	its modifications to the server. */
	ESmlOneWayFromServer,
	/** One-way sync from client.
	
	A sync type in which the client sends its modifications to the server, but the server does not send 
	its modifications back to the client. */
	ESmlOneWayFromClient,
	/** Slow sync.
	
	A form of two-way sync in which the client sends all its data to the server, and the server does 
	the sync analysis for this data and the data in the server. The server may undertake a field by field analysis. */
	ESmlSlowSync,
	/** Refresh sync from server.
	
	A sync type in which the client exports all its data to the server. The server replaces all data in 
	the target database with the data sent by the client. */
	ESmlRefreshFromServer,
	/** Refresh sync from client.
	
	A sync type in which the client exports all its data to the server. The server replaces all data in 
	the target database with the data sent by the client.*/
	ESmlRefreshFromClient
	};
	
/**
	Protocol versions.
*/
enum TSmlProtocolVersion
	{
	/** Version 1.1.2 */
	ESmlVersion1_1_2,
	/** Version 1.2 */
	ESmlVersion1_2
	};

//  End of File
