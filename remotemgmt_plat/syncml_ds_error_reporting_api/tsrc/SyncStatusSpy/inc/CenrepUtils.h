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
* Description: 
*
*/


#ifndef CCENREPUTILS_H
#define CCENREPUTILS_H

// INCLUDES

// FORWARD DECLARATIONS

// CLASS DECLARATION

enum TLogKey
    {
	EErrorCode = 1,
	EProfileId,
	ESyncType,
	ESyncInitiation
    };

/**
* CCenrepUtils utils class to write logs.
*/
class CCenrepUtils : public CBase
    {
	public: //Constructors and destructors

		static void WriteLogL( TUint32 aKey, TLogKey aValue );

		static void GetDateAndTimeL( TDes8& aDateBuffer, TDes8& aTimeBuffer );

		static void CreateSyncLogDirL();

		static void ClearSyncLogL();

	private:

    };


#endif

// End of File
