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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/


#ifndef __DMCert_H__
#define __DMCert_H__

#include <ssl.h>

class RDMCert
{
	public:
	    /**
		* Get Gets the current DM SSL Session certificate
		* @param aCertInfo Storage for the certificate
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Get( TCertInfo &aCertInfo );
};

#endif //__DMCert_H__
