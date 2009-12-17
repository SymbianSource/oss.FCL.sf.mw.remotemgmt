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

time /t
@echo Symbian Headers
@cd common\inc\symbianheaders
@call bldmake bldfiles
@call abld export
@cd ..\..\..

@echo modified Symbian Headers
@cd common\inc\modifiedheaders
@call bldmake bldfiles
@call abld export
@cd ..\..\..

@echo Policy files
@cd common\inc\policy
@call bldmake bldfiles
@call abld export
@cd ..\..\..

@echo off

@rem DEBUG (THIS MUST BE FIRST!)
@call build_module.bat common\debug %1 %2

@rem WBXML
@call build_module.bat common\wbxml %1 %2

@rem XML
@call build_module.bat common\xml %1 %2

@rem transport
@call build_module.bat common\transport %1 %2

@rem http binding
@call build_module.bat common\http %1 %2

@rem Private API
@call build_module.bat common\sosserver_privateapi %1 %2

@rem obexclient
@call build_module.bat common\obex\obexclient %1 %2
@cd..

@rem obexusbserver
@rem call build_module.bat common\obex\obexciusbserver %1 %2
@rem cd..

@rem obexcommserver
@call build_module.bat common\obex\obexcommserver %1 %2
@cd..

@rem obex server binding
@call build_module.bat common\obex\obexserverbinding %1 %2
@cd..

@rem obex ds plugin
@call build_module.bat common\obex\obexdsplugin %1 %2
@cd..

@rem obex dm plugin
@call build_module.bat common\obex\obexdmplugin %1 %2
@cd..

@rem Filter
@call build_module.bat ds\dsutils\filter %1 %2
cd..

@rem Sync Agent
@call build_module.bat common\syncagent %1 %2

@rem historylog
@call build_module.bat common\historylog %1 %2

@rem TransportHandler
@call build_module.bat common\sosserver_transporthandler %1 %2

@rem Client API
@call build_module.bat common\sosserver_ClientAPI %1 %2

@rem DS Settings
@call build_module.bat ds\settings %1 %2


@rem ----------
@rem DS modules
@rem ----------

@rem dataproviderbase
@call build_module.bat ds\adapters\dataproviderbase %1 %2
cd..

@rem storeformat
@call build_module.bat ds\dsutils\storeformat %1 %2
cd..

@rem cgiscriptutils
@call build_module.bat ds\dsutils\cgiscriptutils %1 %2
cd..

@rem changefinder
@call build_module.bat ds\dsutils\changefinder %1 %2
cd..

@rem dbcaps
@call build_module.bat ds\dsutils\dbcaps %1 %2
cd..

@rem emailxmlutils
@call build_module.bat ds\dsutils\emailxmlutils %1 %2
cd..

@rem Filter
@call build_module.bat ds\dsutils\filterutils %1 %2
cd..

@rem Filter
@call build_module.bat ds\dsutils\filter %1 %2
cd..

@rem Adapter Log
@call build_module.bat ds\adapterlog %1 %2

@rem dshostclient
@call build_module.bat ds\hostserver\dshostclient %1 %2
cd..

@rem dshostserverbase
@call build_module.bat ds\hostserver\dshostserverbase %1 %2
cd..

@rem DS Host Servers
@call build_module.bat ds\hostserver\dshostservers %1 %2
cd..


@rem Agentlog
@call build_module.bat ds\agentlog %1 %2

@rem Sync Agent
@call build_module.bat ds\syncagent %1 %2


@rem ----------
@rem DM modules
@rem ----------

@rem FOTA Dummyengine / This is only temporary compilation solution
@call build_module.bat dm\adapters\fota\test\dummyengine %1 %2
@cd ..\..\..

@rem DM utils
@call build_module.bat dm\dmutils %1 %2

@rem Tree db client
@call build_module.bat dm\treedbclient %1 %2

@rem Tree db handler
@call build_module.bat dm\treedbhandler %1 %2

@rem callback server
@call build_module.bat dm\callbackserver %1 %2

rem Host server base
@call build_module.bat dm\hostserver\dmhostserverbase %1 %2
cd..

@rem Host servers
@call build_module.bat dm\hostserver\dmhostservers %1 %2
cd..

@rem Settings
@call build_module.bat dm\settings %1 %2

@rem Tree module
@call build_module.bat dm\treemodule %1 %2

@rem Sync Agent
@call build_module.bat dm\syncagent %1 %2


@@rem adapters
@call build_module.bat dm\adapters %1 %2


@rem --------------------
@rem One Common Module
@rem --------------------
@rem AlertQueue
@call build_module.bat common\alertqueue %1 %2

@rem SyncML Symbian OS Server
@call build_module.bat common\sosserver %1 %2

@rem ----------
@rem DS modules
@rem ----------

@rem Wap Push Alert
@call build_module.bat ds\wappushalert %1 %2

@rem Datamod
@call build_module.bat ds\datamod %1 %2

@rem DS Adapters
@call build_module.bat ds\adapters %1 %2



@rem Server Alert
@call build_module.bat ds\alerthandler %1 %2

@rem DS OMA provisioning 
@call build_module.bat ds\provisioningadapter %1 %2

@rem ----------
@rem DM modules
@rem ----------

@rem Wap Push Alert
@call build_module.bat dm\wappushalert %1 %2

@rem DM OMA provisioning 
@call build_module.bat dm\provisioningadapter %1 %2

@rem DS OTA configuration disabled
@call build_module.bat ds\ota %1 %2

time /t
