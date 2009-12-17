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
rem Description:    This batch file is for building Advance Device Management.
rem

@echo off
echo
echo -----------------------------------------
echo Building the whole TARM (%1 %2)
echo you can give tarmbuild.bat armv5 or 
echo tarmbuild.bat armv5 urel
echo -----------------------------------------

echo -----------------------------------------
echo Make sure that you have added the TARM flags.
echo -----------------------------------------

cd \s60\osext\systemswextensions\commonservices\featuremanager\group
call bldmake bldfiles
call abld build %1 %2
call abld build %1 %2 -check


cd \psw\s60_50_psw\bin_var\featuremanager_var\group
call bldmake bldfiles
call abld build %1 %2
call abld build %1 %2 -check


cd \s60\mw\devicecontentservices\policymanagement\group
call bldmake bldfiles
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

cd \s60\mw\devicemanagement\securityuis\SecUi\group
call bldmake bldfiles
call abld makefile
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

cd \s60\mw\devicecontentservices\terminalsecurity\group
call bldmake bldfiles
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

cd \s60\app\contentcontrol\applicationmanagement\group
call bldmake bldfiles
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

cd \s60\app\contentcontrol\customization\group
call bldmake bldfiles
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

cd \s60\mw\devicecontentservices\syncmlfw\group
call bldmake bldfiles
call abld makefile
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

cd \s60\osext\systemswextensions\systemstartup\Starter\group
call bldmake bldfiles
call abld build %1 %2
call abld build %1 %2 -check


cd \s60\mw\devicemanagement\settingsuis\Gs\group
call bldmake bldfiles
call abld makefile
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

cd \s60\mw\devicemanagement\securityuis\group
call bldmake bldfiles
call abld makefile
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check


cd \s60\app\devicecontrol\omadm\OmaDmAppUi\group
call bldmake bldfiles
call abld makefile
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check


cd \s60\app\contentcontrol\omads\OmaDsAppUi\group
call bldmake bldfiles
call abld makefile
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

cd \s60\app\messaging\messagingeditorsandviewers\Imum\Utils\group
call bldmake bldfiles
call abld reallyclean %1 %2
call abld build %1 %2
call abld build %1 %2 -check

echo -----------------------------------------
echo BUILD DONE!!! 
echo -----------------------------------------

echo -----------------------------------------
echo checking if the build executed properly
echo -----------------------------------------

echo -----------------------------------------
echo checking for featuremanager
echo -----------------------------------------

cd \s60\osext\systemswextensions\commonservices\featuremanager\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for policymanagement
echo -----------------------------------------

cd \s60\mw\devicecontentservices\policymanagement\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for terminalsecurity
echo -----------------------------------------

cd \s60\mw\devicecontentservices\terminalsecurity\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for applicationmanagement
echo -----------------------------------------

cd \s60\app\contentcontrol\applicationmanagement\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for customization
echo -----------------------------------------

cd \s60\app\contentcontrol\customization\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for syncmlfw
echo -----------------------------------------

cd \s60\mw\devicecontentservices\syncmlfw\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for Starter
echo -----------------------------------------

cd \s60\osext\systemswextensions\systemstartup\Starter\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for Gs
echo -----------------------------------------

cd \s60\mw\devicemanagement\settingsuis\Gs\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for securityuis
echo -----------------------------------------

cd \s60\mw\devicemanagement\securityuis\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for OmaDmAppUi
echo -----------------------------------------

cd \s60\app\devicecontrol\omadm\OmaDmAppUi\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for OmaDsAppUi
echo -----------------------------------------

cd \s60\app\contentcontrol\omads\OmaDsAppUi\group
call abld build %1 %2 -check

echo -----------------------------------------
echo checking for messagingeditorsandviewers
echo -----------------------------------------

cd \s60\app\messaging\messagingeditorsandviewers\Imum\Utils\group
call abld build %1 %2 -check

echo -----------------------------------------
echo CHECK DONE!!! 
echo -----------------------------------------

echo --------------------------------------------------
echo before you create the image add the following line to the file 
echo "SCPClient.dll"
echo --------------------------------------------------
