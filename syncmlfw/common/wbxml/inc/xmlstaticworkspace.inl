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
* Description:  Inline methods for static workspace class.
*
*/


#ifndef __XMLSTATICWORKSPACE_INL__
#define __XMLSTATICWORKSPACE_INL__

inline TPtr8 CXMLStaticWorkspace::Ptr() const
	{
	return iBuffer->Des();
	}

#endif // __XMLSTATICWORKSPACE_INL__