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

del /Q \epoc32\winscw\c\private\10202be9\persists\10207816.txt
del /Q \epoc32\winscw\c\private\10202be9\persists\10207815.txt
copy /Y ..\Data\RootIni\10207815.txt \epoc32\RELEASE\winscw\UDEB\z\private\10202BE9
copy /Y ..\Data\RootIni\10207816.txt \epoc32\RELEASE\winscw\UDEB\z\private\10202BE9
del /Q \epoc32\winscw\c\private\10207815\*.*
del /Q \epoc32\winscw\c\private\10207815\backup\*.*
del /Q \epoc32\winscw\c\private\10207815\backup\*.*

md \epoc32\RELEASE\winscw\UDEB\z\private\10207815
copy /y ..\Data\RootIni\0000000257.txt \epoc32\RELEASE\winscw\UDEB\z\private\10207815