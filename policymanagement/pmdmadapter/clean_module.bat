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
echo ########################
echo Cleaning %1
echo ########################

pushd %1\group
@if exist abld.bat call abld.bat reallyclean %2 %3
@call bldmake clean
@if exist ..\BWINS call rmdir /S /Q ..\BWINS
@if exist ..\Bmarm call rmdir /S /Q ..\Bmarm
@if exist ..\eabi call rmdir /S /Q ..\eabi
popd