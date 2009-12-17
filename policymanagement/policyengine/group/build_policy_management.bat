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
* Description: Implementation of policymanagement components
rem

@echo off
echo ############
echo Policy Management -- Building
echo ############
echo 

call bldmake bldfiles
call abld build %1 %2 centreptoolclient
call abld build %1 %2 policyengineclient
call abld build %1 %2 policyengineui
call abld build %1 %2 policyengineserver
call abld build %1 %2 centreptoolserver
