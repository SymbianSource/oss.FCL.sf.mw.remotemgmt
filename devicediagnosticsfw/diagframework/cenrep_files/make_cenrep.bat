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

@set XLS_FILE=keys_diagnosticsfw.xls
@set INI_FILE=2000B16B.txt
@set RELEASE=3.2

@set CENREP_DIR=%CD%
@cd \epoc32\tools\cenrep

cenrep.pl -o "%CENREP_DIR%\output" -l "%CENREP_DIR%\%XLS_FILE%" -p %RELEASE%
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\winscw\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\udeb\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\RELEASE\armv5\urel\Z\private\10202be9\
copy "%CENREP_DIR%\output\inifiles\%INI_FILE%" \epoc32\data\Z\private\10202be9\
@cd "%CENREP_DIR%"

@echo ================= done. Hit any key to continue ==================
@pause

