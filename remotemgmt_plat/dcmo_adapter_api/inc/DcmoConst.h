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
* Description:  DCMO Constants.
*
*/

#ifndef __DCMOCONST_H__
#define __DCMOCONST_H__

TInt const KDCMOMaxStringSize = 255;
	
enum TDCMOGroup
{
EHardware,			// for Hardware group
EIO,						// for I/O group
EConnectivity,	// for Connectivity group
ESoftware,			// for Software group
EService				// for Service group
};

enum TDCMONode
{
EGroup,         	// Int, Get
EAttached,      	// Int, Get
EEnabled,       	// Int, Get
EDenyUserEnable,  // Int, Get
ENotifyUser,      // Int, Get
EEnable, 					// Int, Set // combined EDisable with this , values ETrue/EFalse 
EProperty,      	// Str, Get
EDescription   		// Str, Get
};  

enum TDCMOStatus
{
EDcmoSuccess,
EDcmoFail,
EDcmoNotSupported,
EDcmoNotFound,
EDcmoAccessDenied,
EDcmoUnknown
};

#endif// __DCMOCONST_H__
