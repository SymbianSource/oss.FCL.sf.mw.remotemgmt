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
* Description: Implementation of terminalsecurity components
*
*/
#ifndef _TPTRC8I_H_INCLUDED_
#define _TPTRC8I_H_INCLUDED_

class TPtrC8I : public TPtrC8
	{
	public:

	inline TPtrC8I( const TDesC8 &aDes ): TPtrC8( aDes )
		{
		}

	inline TBool operator==( const TDesC8 &aDes )
		{
		HBufC8* copyThis  = this->Alloc();
		TPtr8 ptrThis     = copyThis->Des();

		HBufC8* copyParam = aDes.Alloc();
		TPtr8 ptrParam    = copyParam->Des();

		ptrThis.LowerCase();
		ptrThis.TrimAll();
		ptrParam.LowerCase();
		ptrParam.TrimAll();
		TBool ret = ptrParam == ptrThis;
		delete copyParam;
		delete copyThis;

		return ret;
		}
	};

class TPtrC16I : public TPtrC16
	{
	public:

	inline TPtrC16I( const TDesC16 &aDes ): TPtrC16( aDes )
		{
		}

	inline TBool operator==( const TDesC16 &aDes )
		{
		HBufC16* copyThis  = this->Alloc();
		TPtr16   ptrThis   = copyThis->Des();

		HBufC16* copyParam = aDes.Alloc();
		TPtr16   ptrParam  = copyParam->Des();

		ptrThis.LowerCase();
		ptrThis.TrimAll();
		ptrParam.LowerCase();
		ptrParam.TrimAll();
		TBool ret = ptrParam == ptrThis;
		delete copyParam;
		delete copyThis;

		return ret;
		}
	};

#endif //_TPTRC8I_H_INCLUDED_
