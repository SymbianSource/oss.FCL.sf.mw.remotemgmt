/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class declaration for TDiagEngineWatchdogTypes
*
*/



#ifndef DIAGENGINEWATCHDOGTYPES_H
#define DIAGENGINEWATCHDOGTYPES_H

/**
* These types indicate to engine how long it should wait between each
* test execution progress reports.  
*
* Typically, if a test step requires user interaction, engine should wait
* longer before stopping its execution. For non-interactive steps,
* engine should not wait as long. 
* 
* The actual values are specifed by cenrep key value owned by engine.
*
* Values:
* EDiagEngineWatchdogTypeInteractive - Indicates that engine should
*   use interactive watchdog timer value.
*
* EDiagEngineWatchdogTypeAutomatic - Indicates that engine should
*   use non-interactive watchdog timer value.
*
* @since S60 3.2
*/
enum TDiagEngineWatchdogTypes
    {
    EDiagEngineWatchdogTypeInteractive   = 0,
    EDiagEngineWatchdogTypeAutomatic
    };

#endif // DIAGENGINEWATCHDOGTYPES_H

// End of File

