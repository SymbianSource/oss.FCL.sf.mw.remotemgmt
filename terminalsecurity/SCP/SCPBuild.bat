rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description: Implementation of terminalsecurity components
rem

@echo off
echo
echo -----------------------------------------
echo Building the SCP module (%1 %2)
echo -----------------------------------------

echo **
echo ***
echo .....Exporting server headers
echo ***
echo **
cd SCPServer\group
call bldmake bldfiles
call abld export

echo **
echo ***
echo .....Building the client library
echo ***
echo **
cd ..\..\SCPClient\group
call bldmake bldfiles
call abld build %1 %2

echo **
echo ***
echo .....Building the server
echo ***
echo **
cd ..\..\SCPServer\group
call abld build %1 %2

echo **
echo ***
echo .....Building SCPPatternPlugin
echo ***
echo **
cd ..\..\SCPPatternPlugin\group
call bldmake bldfiles
call abld build %1 %2


echo **
echo ***
echo .....Building SCPHistoryPlugin
echo ***
echo **
cd ..\..\SCPHistoryPlugin\group
call bldmake bldfiles
call abld build %1 %2


echo **
echo ***
echo .....Building SCPTimestampPlugin
echo ***
echo **
cd ..\..\SCPTimestampPlugin\group
call bldmake bldfiles
call abld build %1 %2

cd ..\..

echo **
echo ***
echo ...done.
echo ***
echo **

