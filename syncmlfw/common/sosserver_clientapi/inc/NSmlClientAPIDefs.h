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
* Description: SyncML constants definition. 
*
*/


#ifndef __NSMLCLIENTAPIDEFS__
#define __NSMLCLIENTAPIDEFS__

//#define __CLIENT_API_MT_
#define SYNCML_V3

_LIT( KNSmlClientAPIPanic, "SyncMLClientAPI" );

_LIT( KNSmlInitialString, "" );
_LIT8( KNSmlInitialString8, "" );
_LIT8( KNSmlInitialZeroString8, "0" );

const TInt KNSmlMaxInt32Length = 10;
const TInt KNSmlMaxIntFetchCount = 10;

const TInt KNSmlMaxEventMessageLength = 64;
const TInt KNSmlMaxProgressMessageLength = 128;

const TUint KDefaultMessageSlots=4;
#endif