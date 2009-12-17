/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  AuthtypePlugin implementation.
 *
*/

#ifndef AuthtypePLUGIN_H
#define AuthtypePLUGIN_H

// User includes

// System includes
#include <gsplugininterface.h>
#include <aknview.h>
#include <ConeResLoader.h>
#include <eikclb.h>
#include <AknServerApp.h>
#include <AknCapServerClient.h>
#include <gsbasecontainer.h>

// Classes referenced
class CAknNavigationDecorator;
class CAknViewAppUi;
class CAknNullService;

// Constants
const TUid KAuthtypePluginUID =
    {
    0x2001247D
    };
_LIT( KOperatorLogoResourceFileName, "z:AuthtypePluginRsc.rsc" );
_LIT( KAuthtypePluginIconDirAndName, "z:AuthtypePlugin.mif");
// Use KDC_BITMAP_DIR

const TInt KFirstItemSelected = 0;
const TInt KSecondItemSelected = 1;
const TInt KThirdItemSelected = 2;

const TInt KNetworkPinOnly = 1;
const TInt KNetworkorUserPin = 3;
const TInt KAllMethodsSupported = 15;

// CLASS DECLARATION


/**
 * CAuthtypePlugin.
 *
 * This class handles state and application logic of OperatorLogo settings.
 * The plugin is a type of EGSItemTypeSettingDialog and therefore the GS FW will
 * call HandleSelection() instead of DoActivate(). No CAknView functionality is
 * supported even though the base class is CAknView derived via
 * CGSPluginInterface.
 *
 */
class CAuthtypePlugin : public CGSPluginInterface,
    public MAknServerAppExitObserver// Embedding
    {
public:
    // Constructors and destructor

    /**
     * Symbian OS two-phased constructor
     * @return
     */
    static CAuthtypePlugin* NewL(TAny* aInitParams);

    /**
     * Destructor.
     */
    ~CAuthtypePlugin();

public:
    // From CAknView

    /**
     * See base class.
     */
    TUid Id() const;

public:
    // From CGSPluginInterface

    /**
     * See base class.
     */
    void GetCaptionL(TDes& aCaption) const;

    /**
     * See base class.
     */
    TInt PluginProviderCategory() const;

    /**
     * See base class.
     */
    TGSListboxItemTypes ItemType();

    /**
     * See base class.
     */
    void GetValue(const TGSPluginValueKeys aKey, TDes& aValue);

    /**
     * See base class.
     */
    void HandleSelection(const TGSSelectionTypes aSelectionType);

    /**
     * See base class.
     */
    CGulIcon* CreateIconL(const TUid aIconType);

    /*
     * Handles selection item
     */
    void HandleItemContentL();

    /*
     * Set Cenrep value based on selection of item from list
     */
    void SetCenrepItemL(TInt index, TInt auth_value);

    /* 
     * popup note shown on selection of 2nd and 3rd item from list 
     */

    void ShowPopupNoteL(HBufC *title);

protected:
    // New

    /**
     * C++ default constructor.
     */
    CAuthtypePlugin();

    /**
     * Symbian OS default constructor.
     */
    void ConstructL();

protected:
    // From CAknView

    /**
     * This implementation is empty because this class, being just a dialog,
     * does not implement the CAknView finctionality.
     */
    void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

    /**
     * This implementation is empty because this class, being just a dialog,
     * does not implement the CAknView finctionality.
     */
    void DoDeactivate();

private:
    // New

    /**
     * Opens localized resource file.
     */
    void OpenLocalizedResourceFileL(const TDesC& aResourceFileName,
            RConeResourceLoader& aResourceLoader);

    /**
     * Suppress the menu/app key when reminder dialog is displayed
     * @param ETrue to Suppress, else EFalse
     * @return None
     */

    void SuppressAppKey(TBool aValue);

private:
    //data

    //Holds the state of menu/app key supress
    TBool iIsAppKeySuppressed;

protected:
    // Data

    // Resource loader.
    RConeResourceLoader iResources;

    };

#endif // AuthtypePLUGIN_H
// End of File
