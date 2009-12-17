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
rem Description: Implementation of policymanagement components
rem

@echo off
echo ######################
echo Building PMA
echo ######################

pushd group
call bldmake bldfiles
@call abld export
@call abld makefile %1
@call abld resource %1 %2
@call abld target %1 %2
@call abld freeze %1 
@call abld makefile %1 
@call abld library %1 
@call abld final %1 %2
popd

echo ##########################
echo Build PMA finished
echo ##########################