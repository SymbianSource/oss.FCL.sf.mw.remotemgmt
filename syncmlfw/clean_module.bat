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
rem Description:
rem

@echo off

@rem Cleans one module. Module name is given in parameter 1.

echo **
echo ** Cleaning [ %1 ]
echo **

cd %1\bld
@if exist abld.bat call abld.bat reallyclean %2 %3
@call bldmake clean
@if exist def for %%a in (def\*.def) do del %%a
cd ..\..\..








