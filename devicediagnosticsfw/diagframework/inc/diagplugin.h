/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Abstract class that represents a diagnostics plug-in
*
*/


#ifndef DIAGPLUGIN_H
#define DIAGPLUGIN_H

// INCLUDES
#include <badesca.h>            // CPtrCArray . Needed since this is typedef.

//Forward declarations
class CGulIcon;
class MDiagEngineCommon;

// Constants for plugin interfaces 
const TUid KDiagPluginInterfaceUid = { 0x2000AFEB };

// UID of top most root suite uid.  All plug-ins are descendant of this suite.
const TUid KDiagRootSuiteUid = { 0x00000000 };

/**
* Abstract class that repsesents a diagnostics plug-in.
*
* @since S60 v5.0
**/
class MDiagPlugin
    {
public: //data structures

    /**
    * Indicates is the plug-in a suite or a test plug-in
    **/
    enum TPluginType
        {
        ETypeTestPlugin,
        ETypeSuitePlugin
        };

    /**
    * Layout type of name being requested.
    */
    enum TNameLayoutType
        {
        ENameLayoutListSingle,              // list_single_graphic_heading_pane_t1_cp2
        ENameLayoutPopupNoteWaitWindow,     // popup_note_wait_window
        ENameLayoutListLargeGraphic,        // list_single_large_graphic_pane_t1
        ENameLayoutHeadingPane,             // heading_pane_t1
        ENameLayoutPopupInfoPane,           // popup_info_list_pane_t1
        ENameLayoutTitlePane,               // title_pane_t2/opt12
        ENameLayoutListSingleGraphic,       // list_single_graphic_pane_t1
        ENameLayoutListDoubleGraphic        // list_double_graphic_pane_t1
        };

public: // public methods

    /**
    * Virtual destructor. This ensures that when the interface is deleted,
    * compiler will look up virtual table to find correct destructor.
    */
    virtual ~MDiagPlugin() {}

    /**
    * Method for getting the name of the plugin. Name returned should be based
    * on the layout requested, and it should fit within the layout.
    *
    * @param aLayoutType - Layout where text will be displayed in. @see TNameLayoutType
    * @return Localized name of the plug-in. Ownership is transferred to caller.
    */
    virtual HBufC* GetPluginNameL( TNameLayoutType aLayoutType ) const = 0;

    /**
    * Get the name of the service that the plug-in provides.
    * Service name is defined in the ECOM resource file, thus, it is not localized.
    *
    * @return The name of the service.
    **/
    virtual const TDesC& ServiceLogicalName() const = 0;

    /**
    * Get logical dependencies. One plug-in can have multiple dependencies to other plug-ins.
    *
    * @param aArray An array of logical names.
    * @see ServiceLogicalName.
    **/
    virtual void GetLogicalDependenciesL( CPtrCArray& aArray ) const = 0;

    /**
    * Return the type of the plug-in. 
    *
    * @return The type.
    * @see TPluginType.
    **/
    virtual TPluginType Type() const = 0;

    /**
    * Create an icon that represents the plug-in.
    *
    * @return An icon. 
    **/
    virtual CGulIcon* CreateIconL() const = 0;

    /** Method for checking, if plugin should be visible.
    * (for example shown in listbox).
    * Overwrite this function to show or hide your plugin dynamically.
    *
    * @return ETrue if plugin should be visible, EFalse otherwise.
    */
    virtual TBool IsVisible() const = 0;

    /** Method for checking, if plugin is supported.
    * Overwrite this function to enable or disable your plugin dynamically.
    * If EFalse, plug-in will not be loaded by plugin pool.
    *
    * @return ETrue if plugin is supported, EFalse otherwise.
    */
    virtual TBool IsSupported() const = 0;

    /**
    * Get localised description of the test. Description should be used to explain 
    * the test for the user. It can contain comments that this service needs for 
    * example access point and it creates costs or that the duration might be long.
    *
    * @return Allocated buffer containing localized description of the plugin.
    **/
    virtual HBufC* GetDescriptionL() const = 0;

    /**
    * Get UID of this plug-in.
    *
    * @return the UID.
    **/
    virtual TUid Uid() const = 0;

    /**
    * Get the order number that this plug-in should appear in its parent list.
    *
    * @return TUint order number.
    **/
    virtual TUint Order() const = 0;

    /**
    * Get UID of the parent.
    *
    * @return The parent UID.
    **/
    virtual TUid ParentUid() const = 0;

    /**
    * Set ECom Destructor ID Key.
    *
    * @param aDtorIdKey Uid returned by REComSession::CreateImplementationL()
    **/
    virtual void SetDtorIdKey( TUid aDtorIdKey ) = 0;


    /**
    * Initialization Step. This method is called before any plugin are executed.
    * This can be used to clean up any left over data from previous execution 
    * sessions. All plug-ins in execution plan will have a chance to clean 
    * up before any plug-ins are run.  This is a synchrouns method.
    *
    * @param aEngine - Reference to engine.
    * @param aSkipDependencyCheck - If ETrue, plug-in will be executed
    *   even if dependencies are not executed.
    * @param aCustomParams Custom parameters for plug-ins. 
    *   It can used to pass arbitrary data from application to the plug-ins.
    *   Owership is not transferred and plug-in must not delete
    *   this parameter. Ownership is not transferred since
    *   client does not always know the type of pointer.
    **/
    virtual void TestSessionBeginL( MDiagEngineCommon& aEngine,
                                    TBool aSkipDependencyCheck,
                                    TAny* aCustomParams ) = 0;


    /**
    * Cleanup Step. This method is called after all plug-ins in the 
    * execution plan is completed to clean up any left over data from 
    * current sesison. This can be used to clean up any data that
    * provides dependent service created for its dependencies.
    * This is a synchrouns method.
    *
    * @param aEngine - Reference to engine.
    * @param aSkipDependencyCheck - If ETrue, plug-in as executed
    *   even if dependencies are not executed.
    * @param aCustomParams Custom parameters for plug-ins. 
    *   It can used to pass arbitrary data from application to the plug-ins.
    *   Owership is not transferred and plug-in must not delete
    *   this parameter. Ownership is not transferred since
    *   client does not always know the type of pointer.
    **/
    virtual void TestSessionEndL( MDiagEngineCommon& aEngine,
                                  TBool aSkipDependencyCheck,
                                  TAny* aCustomParams ) = 0;


    /**
    * Reserved for future use/plugin's custom functionality.
    *
    * @param aUid Unique identifier of the operation.
    * @param aParam Custom parameter. 
    * @return TAny pointer. Custom data.
    *   It can used to pass arbitrary data from application to the plug-ins.
    *   Owership is not transferred and plug-in must not delete
    *   this parameter. Ownership is not transferred since
    *   client does not always know the type of pointer.
    **/
    virtual TAny* CustomOperationL( TUid aUid, TAny* aParam ) = 0;
        
    /**
    * Reserved for future use/plugin's custom functionality. 
    *
    * @param aParam Custom parameter.
    * @return TAny pointer. Custom data. 
    *   It can used to pass arbitrary data from application to the plug-ins.
    *   Owership is not transferred and plug-in must not delete
    *   this parameter. Ownership is not transferred since
    *   client does not always know the type of pointer.
    **/
    virtual TAny* GetCustomL( TUid aUid, TAny* aParam ) = 0;
    };

#endif // DIAGPLUGIN_H

// End of File

