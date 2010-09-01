/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Bio control for Provisioning documents.
*
*/


#ifndef CWPBIOCONTROL_H
#define CWPBIOCONTROL_H

// INCLUDES
#include <msgbiocontrol.h>                // for CMsgBioControl

// FORWARD DECLARATIONS
class CWPEngine;
class CWPNameValue;
class CWPPushMessage;
class CRichBio;

// CLASS DECLARATION

/**
 * Bio control for OMA Provisioning messages.
 * @since 2.0
 */
class CWPBioControl: public CMsgBioControl
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        * @param aObserver Reference to the Bio control observer.
        * @param aSession Reference to Message Server session.
        * @param aId Id of the message.
        * @param aEditorOrViewerMode Enum for the mode, is it as editor or viewer.
        * @param aFileName The newly created object.
        * @param aFile file handle to provisioning content.
        */
        IMPORT_C static CWPBioControl* NewL( MMsgBioControlObserver& aObserver,
                                             CMsvSession* aSession,
                                             TMsvId aId,
                                             TMsgBioMode aEditorOrViewerMode,
                                             const RFile* aFile );

        /**
        * Destructor.
        */
        virtual ~CWPBioControl();

    public: // From MMsgBioControl
        
        /**
        * Calculates and sets size for a Bio control according to aSize.
        * This function might become deprecated. The SetSizeL() function will
        * be the replacement.
        * The height of the Bio control may be less or more than requested by
        * aSize, but the width must be exactly the same. If width of the Bio
        * control is not the same as given by aSize, the width must be reset
        * back to requested one. The aSize is set to the new size.
        * @param aSize Size.
        */
        void SetAndGetSizeL( TSize& aSize );

        /**
        * This is called by the container to allow the Bio control to add
        * a menu item command. The command ID's should start from the value
        * returned by the MMsgBioControlObserver::FirstFreeCommand().
        * @param aMenuPane Reference to the applications menu which will be
        * modified.
        */
        void SetMenuCommandSetL( CEikMenuPane& aMenuPane );

        /**
        * The command handler. The Bio Control should only handle its own
        * commands that it has set using the function SetMenuCommandSetL().
        * @param aCommand ID of command to be handled.
        * @return If the command is handled, it returns ETrue, and vice versa
        */
        TBool HandleBioCommandL( TInt aCommand );

        /**
        * Returns a rectangle slice of the bio controls viewing area.
        * It is used by the CMsgEditorView class for scrolling the screen.
        * The position is given relative to the bio controls top left
        * corner.
        * @return TRect
        */
        TRect CurrentLineRect() const;

        /**
        * Used by the body container for managing focus and scrolling.
        * @param aDirection The direction to be checked.
        * @return ETrue if focus change is possible, and vice versa.
        */
        TBool IsFocusChangePossible( TMsgFocusDirection aDirection ) const;

        /**
        * Returns the header text.
        * @return The header text.
        */
        HBufC* HeaderTextL()  const;

        /**
        * The application can get the option menu permissions using this
        * function.
        * @return The option menu permission flags. If the flag is off it
        * means that the option menu command is not recommended with this
        * Bio Control.
        */
        TUint32 OptionMenuPermissionsL() const;

        /**
        * Gives the height of the text in pixels.
        * It is used by the scrolling framework.
        * @return Height of the text in pixels.
        */
        TInt VirtualHeight();

        /**
        * Gives the cursor position in pixels.
        * It is used by the scrolling framework.
        * @return Cursor position in pixels.
        */
        TInt VirtualVisibleTop();

        /**
        * Tells whether the cursor is in the topmost or bottom position.
        * It is used by the scrolling framework.
        * @param aLocation Specifies either top or bottom.
        * @return ETrue if the cursor is in the part specified by aLocation.
        */
        TBool IsCursorLocation( TMsgCursorLocation aLocation ) const;


    public: // From CCoeControl

        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    protected: // From CCoeControl

        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        void SizeChanged();
        void FocusChanged( TDrawNow aDrawNow );
        void SetContainerWindowL( const CCoeControl& aContainer );
    /** 
     * SetCenrepKeyL
     * Set the Cenrep key.
     * @param aValue Cenrep Value.
     * @return void
     **/
    void SetCenrepKeyL(const TDesC8& aValue);

    private: // Constructors

        /**
        * Constructor is prohibited.
        * @param aObserver Reference to the Bio control observer.
        * @param aSession Reference to Message Server session.
        * @param aId Id of the message in Message Server.
        * @param aEditorOrViewerMode Enum for the mode, is it as editor or viewer.
        * @param aFileName The newly created object.
        * @param aFile file handle to provisioning content.
        */
        CWPBioControl( MMsgBioControlObserver& aObserver,
                       CMsvSession* aSession,
                       TMsvId aId,
                       TMsgBioMode aEditorOrViewerMode,
                       const RFile* aFile );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    private: // New functions

        /**
        * Restores the message from messaging store.
        */
        void RestoreMsgL();

        /**
        * Adds the message summaries to the richbio.
        */
        void AddItemsL();

        /**
        * Collects the summary items from engine and 
        * sorts them.
        * @return Array with summary items
        */
        RPointerArray<CWPNameValue>* CollectItemsLC() const;

        /**
        * Collates items with same title and adds
        * them to the rich bio.
        * @param aItems The items to be collated
        */
        void CollateAndAddItemsL( RPointerArray<CWPNameValue>& aItems );

        /**
        * Save the settings.
        */
        void SaveSettingsL();

        /**
        * Save the message again.
        */
        void SaveMessageL();

        /**
        * Do the actual saving.
        */
        void DoSaveL();

        /**
        * Try to authenticate a message.
        * @param aMessage The message to authenticate
        */
        void AuthenticateL( CWPPushMessage& aMessage );

        /**
        * ResetAndDestroy() cleanup for an RPointerArray<CMsgNameValue>.
        * @param aAny Array
        */
        static void Cleanup( TAny* aAny );

        /** 
        * Comparator for two string pairs. Compares the names of the pairs.
        * @param aImpl1 First string pair to compare
        * @param aImpl2 Second string pair to compare
        */
        static TInt Compare( const CWPNameValue& aImpl1,
                             const CWPNameValue& aImpl2 );
    private:
        void ErrorNoteL(const TDesC& aText);
        void ErrorNoteL(TInt aStringResource);
        void ConfirmationNoteL(const TDesC& aText);
        void ConfirmationNoteL(TInt aStringResource);
        void InformationNoteL(const TDesC& aText);
        void InformationNoteL(TInt aStringResource);

    private: //Data
        enum TMenuCommands
            {
            ECmdSaveSettings
            };

        /// Pointer to richbio control. Owns.
        CRichBio*       iRichBio;

        /// Provisioning engine. Owns.
        CWPEngine*      iEngine;

        /// The message being handled. Owns.
        CWPPushMessage* iMessage;
    };

#endif // CWPBIOCONTROL_H

// End of File
