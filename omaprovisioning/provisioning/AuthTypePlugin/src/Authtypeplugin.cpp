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


// User includes
#include    "Authtypeplugin.h"

#include    <gsparentplugin.h>
#include    <gscommon.hrh>
#include    <AuthtypepluginRsc.rsg> // GUI Resource
#include    <Authtypeplugin.mbg>
#include    <gsprivatepluginproviderids.h>
#include    <gsfwviewuids.h>
#include    "ProvisioningInternalCRKeys.h"
#include    <gsbaseview.h>

// System includes
#include    <AknWaitDialog.h>
#include    <AknNullService.h>
#include    <aknnotewrappers.h>
#include    <akntextsettingpage.h>
#include    <aknViewAppUi.h>
#include    <bautils.h>
#include    <featmgr.h>
#include    <StringLoader.h>

#include    <badesca.h>
#include    <aknlists.h> 
#include    <AknListBoxSettingPage.h> 
#include    <centralrepository.h>
#include    "ProvisioningDebug.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CAuthtypePlugin::CAuthtypePlugin()
// Constructor
// ---------------------------------------------------------------------------
//
CAuthtypePlugin::CAuthtypePlugin() :
    iResources( *iCoeEnv)
    {
    }

// ---------------------------------------------------------------------------
// CAuthtypePlugin::~CAuthtypePlugin()
// Destructor
// ---------------------------------------------------------------------------
//
CAuthtypePlugin::~CAuthtypePlugin()
    {
    iResources.Close();
    }

// ---------------------------------------------------------------------------
// CAuthtypePlugin::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
void CAuthtypePlugin::ConstructL()
    {

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::ConstructL:" ) );
    OpenLocalizedResourceFileL(KOperatorLogoResourceFileName, iResources);
    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::ConstructL: DONE" ) );

    }

// ---------------------------------------------------------------------------
// CAuthtypePlugin::NewL()
// Static constructor
// ---------------------------------------------------------------------------
//
CAuthtypePlugin* CAuthtypePlugin::NewL(TAny* /*aInitParams*/)
    {

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::NewL:" ) );

    CAuthtypePlugin* self = new( ELeave ) CAuthtypePlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;

    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::Id
// -----------------------------------------------------------------------------
//
TUid CAuthtypePlugin::Id() const
    {
    return KAuthtypePluginUID;
    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::DoActivateL
// -----------------------------------------------------------------------------
//
void CAuthtypePlugin::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {

    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::DoDeactivate
// -----------------------------------------------------------------------------
//
void CAuthtypePlugin::DoDeactivate()
    {

    }

// ========================= From CGSPluginInterface ==================


// -----------------------------------------------------------------------------
// CAuthtypePlugin::GetCaptionL
// -----------------------------------------------------------------------------
//
void CAuthtypePlugin::GetCaptionL(TDes& aCaption) const
    {

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::GetCaptionL" ) );

    HBufC* result = StringLoader::LoadL( R_SET_CONF_MSG_AUTHENTICATION_CP);
    aCaption.Copy( *result);
    delete result;

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::GetCaptionL  DONE" ) );
    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::PluginProviderCategory
// -----------------------------------------------------------------------------
//
TInt CAuthtypePlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::ItemType()
// -----------------------------------------------------------------------------
//
TGSListboxItemTypes CAuthtypePlugin::ItemType()
    {
    return EGSItemTypeSettingDialog;
    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::GetValue()
// -----------------------------------------------------------------------------
//
void CAuthtypePlugin::GetValue(const TGSPluginValueKeys /*aKey*/, TDes& /*aValue*/)
    {
    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::HandleSelection()
// -----------------------------------------------------------------------------
//
void CAuthtypePlugin::HandleSelection(const TGSSelectionTypes /*aSelectionType*/)
    {
    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::HandleSelection" ) );

    TRAP_IGNORE( HandleItemContentL() );

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::HandleSelection  DONE" ) );
    }

// ---------------------------------------------------------------------------
// CAuthtypePlugin::CreateIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CAuthtypePlugin::CreateIconL(const TUid aIconType)
    {

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::CreateIconL" ) );

    //EMbm<Mbm_file_name><Bitmap_name>
    CGulIcon* icon;
    TParse* fp = new( ELeave ) TParse();
    CleanupStack::PushL(fp);
    fp->Set(KAuthtypePluginIconDirAndName, &KDC_BITMAP_DIR, NULL);

    if (aIconType == KGSIconTypeLbxItem)
        {
        icon = AknsUtils::CreateGulIconL(AknsUtils::SkinInstance(),
                KAknsIIDDefault, fp->FullName(),
                EMbmAuthtypepluginQgn_prop_set_conn_config,
                EMbmAuthtypepluginQgn_prop_set_conn_config_mask);
        }
    else
        {
        icon = CGSPluginInterface::CreateIconL(aIconType);
        }

    CleanupStack::PopAndDestroy(fp);

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::CreateIconL:  DONE" ) );

    return icon;

    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::OpenLocalizedResourceFileL()
// -----------------------------------------------------------------------------
//
void CAuthtypePlugin::OpenLocalizedResourceFileL(
        const TDesC& aResourceFileName, RConeResourceLoader& aResourceLoader)
    {

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::OpenLocalizedResourceFileL" ) );

    RFs fsSession;
    User::LeaveIfError(fsSession.Connect() );

    CleanupClosePushL(fsSession);

    // Find the resource file:
    TParse parse;
    parse.Set(aResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    TFileName fileName(parse.FullName() );

    // Get language of resource file:
    BaflUtils::NearestLanguageFile(fsSession, fileName);

    // Open resource file:
    aResourceLoader.OpenL(fileName);

    CleanupStack::PopAndDestroy(); //fssession

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::OpenLocalizedResourceFileL  DONE" ) );
    }

// -----------------------------------------------------------------------------
// CAuthtypePlugin::HandleItemContentL()
// -----------------------------------------------------------------------------
//

void CAuthtypePlugin::HandleItemContentL()
    {

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::HandleItemContentL" ) );

    // Holds the index of selected item 
    TInt selectedItem(KThirdItemSelected);

    // Holds the authentication type value
    TInt getAuthValue(KAllMethodsSupported);

    //Load all the strings from resource file 

    HBufC *title = StringLoader::LoadLC(R_QTN_SET_CONF_AUTH_METHOD);

    HBufC *netPinSupport = StringLoader::LoadLC(R_QTN_CONF_AUTH_NETWORK_PIN);
    HBufC *netUserPinSupport =
            StringLoader::LoadLC(R_QTN_CONF_AUTH_NETWORK_OR_USER_PIN);
    HBufC *allMethodsSupport = StringLoader::LoadLC(R_QTN_CONF_AUTH_ALL_AUTH);

    //creation of list

    CAknSinglePopupMenuStyleListBox* list = new(ELeave)CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

    CAknPopupList* popupList = CAknPopupList::NewL(list, 
    R_AVKON_SOFTKEYS_OK_CANCEL__OK, AknPopupLayouts::EMenuWindow);

    CleanupStack::PushL(popupList);

    list->ConstructL(popupList, 0);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto);

    //Append strings into array which will be populated into list

    CDesCArrayFlat* ItemArray = new(ELeave)CDesCArrayFlat(3);
    CleanupStack::PushL(ItemArray);

    ItemArray->AppendL(*netPinSupport);
    ItemArray->AppendL(*netUserPinSupport);
    ItemArray->AppendL(*allMethodsSupport);

    CleanupStack::Pop(); //ItemArray

    list->Model()->SetItemTextArray(ItemArray);
    list->Model()->SetOwnershipType(ELbmOwnsItemArray);

    popupList->SetTitleL(*title);

    /* Get authentication type value from Central Repository and based on current cenrep value
     * List box item will be selected */

    CRepository * rep= NULL;
    TRAPD( err, rep = CRepository::NewL( KCRUidOMAProvisioningLV ))
    ;

    if (err == KErrNone)
        {
        rep->Get(KOMAProvAuthenticationLVFlag, getAuthValue);
        delete rep;
        }

    switch (getAuthValue)
        {
        case KNetworkPinOnly:
            selectedItem = KFirstItemSelected;
            break;

        case KNetworkorUserPin:
            selectedItem = KSecondItemSelected;
            break;

        case KAllMethodsSupported:
            selectedItem = KThirdItemSelected;
            break;

        default:
            break;
        }

    list->View()->SetCurrentItemIndex(selectedItem);

    // Show popup list and then show return value.
    TInt popupOk = popupList->ExecuteLD();

    //When user selects item from list based on selection cenrep value will be set

    if (popupOk)
        {

        selectedItem = list->CurrentItemIndex();

        switch (selectedItem)
            {
            case KFirstItemSelected:
                SetCenrepItemL(selectedItem, KNetworkPinOnly);
                break;
            case KSecondItemSelected:
                SetCenrepItemL(selectedItem, KNetworkorUserPin);
                break;
            case KThirdItemSelected:
                SetCenrepItemL(selectedItem, KAllMethodsSupported);
                break;
            default:
                break;

            }

        }

    CleanupStack::Pop(); // popuplist
    CleanupStack::PopAndDestroy(5); // list, title, netPinSupport, netUserPinSupport, allMethodsSupport

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::HandleItemContentL  DONE" ) );

    }

// ---------------------------------------------------------
// CAuthtypeUIContainer::SetCenrepItemL()
// ---------------------------------------------------------
//
void CAuthtypePlugin::SetCenrepItemL(TInt aSelected, TInt aSetAuthValue)
    {

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::SetCenrepItemL" ) );
    //Based on current selection item from list set the cenrep with different authentication type

    CRepository * rep= NULL;
    TRAPD( err, rep = CRepository::NewL( KCRUidOMAProvisioningLV ))
    ;

    if (err == KErrNone)
        {
        rep->Set(KOMAProvAuthenticationLVFlag, aSetAuthValue);
        delete rep;
        }

    //Shows Information note to user on selection of second and third Item from List       

    if (aSelected == KSecondItemSelected)
        {

        HBufC
                * netUserPinNote =
                        StringLoader::LoadLC(R_QTN_CONF_AUTH_NETWORK_OR_USER_PIN_NOTE);
        ShowPopupNoteL(netUserPinNote);
        CleanupStack::PopAndDestroy(); // netUserPinNote
        }

    else
        if (aSelected == KThirdItemSelected)
            {
            HBufC* allMethodsNote =
                    StringLoader::LoadLC(R_QTN_CONF_AUTH_ALL_AUTH_NOTE);
            ShowPopupNoteL(allMethodsNote);
            CleanupStack::PopAndDestroy(); // allMethodsNote
            }

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::SetCenrepItemL  DONE" ) );
    }

// ---------------------------------------------------------
// CAuthtypeUIContainer::ShowPopupNoteL()
// ---------------------------------------------------------
//
void CAuthtypePlugin::ShowPopupNoteL(HBufC *aNote)
    {

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::ShowPopupNoteL" ) );

    CAknQueryDialog* queryDialog = new (ELeave) CAknQueryDialog;
    queryDialog->ExecuteLD(R_AUTHTYPEUI_INFORMATION_QUERY, *aNote);

    FLOG( _L( "[AuthtypePlugin] CAuthtypePlugin::ShowPopupNoteL  DONE" ) );

    }

// End of file
