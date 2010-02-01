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
rem Description: Implementation of policymanagement components
rem

echo *************************
echo * Policy file configure *
echo *************************



set policy_dir=policies
if not "%1"=="" set policy_dir=%1

if not exist \epoc32\data\Z\private\10207815\thirdpartypolicy.txt (


call md \epoc32\data\Z\private\10207815
call del /q \epoc32\data\Z\private\10207815\*.* 
call copy ..\policyengine\data\%policy_dir%\*.txt \epoc32\data\Z\private\10207815

call md \epoc32\release\winscw\udeb\Z\private\10207815
call del /q \epoc32\release\winscw\udeb\Z\private\10207815\*.*
call copy ..\policyengine\data\%policy_dir%\*.txt \epoc32\release\winscw\udeb\Z\private\10207815

call md \epoc32\release\winscw\urel\Z\private\10207815
call del /q \epoc32\release\winscw\urel\Z\private\10207815\*.*
call copy ..\policyengine\data\%policy_dir%\*.txt \epoc32\release\winscw\urel\Z\private\10207815

call md \epoc32\release\armv5\udeb\Z\private\10207815
call del /q \epoc32\release\armv5\udeb\Z\private\10207815\*.*
call copy ..\policyengine\data\%policy_dir%\*.txt \epoc32\release\armv5\udeb\Z\private\10207815

call md \epoc32\release\armv5\urel\Z\private\10207815
call del /q \epoc32\release\armv5\urel\Z\private\10207815\*.*
call copy ..\policyengine\data\%policy_dir%\*.txt \epoc32\release\armv5\urel\Z\private\10207815

)