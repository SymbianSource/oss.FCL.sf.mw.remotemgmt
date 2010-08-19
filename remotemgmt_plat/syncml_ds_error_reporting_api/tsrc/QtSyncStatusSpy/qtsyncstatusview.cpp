/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source file for the class CalenSettingsView
*
*/
#include <QGraphicsItem>
#include <QtGui>
#include <QTimer>
#include <QWidget>
#include <QDebug>
#include <hbdataform.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <XQServiceUtil.h>

#include "qtsyncstatusview.h"
#include "qtsyncstatusmodel.h"
#include "qtsyncstatuslog.h"


QtSyncStatusView::QtSyncStatusView(QtSyncStatusLog& log, QGraphicsItem *parent )
: HbView(parent)
{
    setTitle(tr("QtSyncStatusSpy"));
    createMenu();
    mSyncStatusForm = new HbDataForm(this);
    mSyncStatusModel = new QtSyncStatusModel(log, mSyncStatusForm);
    mSyncStatusModel->initModel();
    setWidget(mSyncStatusForm);
}

QtSyncStatusView::~QtSyncStatusView()
{
    delete mSyncStatusModel;
}

void QtSyncStatusView::publish()
{
    mSyncStatusModel->publishSyncStatusKey();
}

void QtSyncStatusView::viewLog()
{
    emit showLog();
}

void QtSyncStatusView::hideView()
{
    XQServiceUtil::toBackground( true );
}

void QtSyncStatusView::deleteLog()
{
    mSyncStatusModel->clearSyncLog();
}

void QtSyncStatusView::activateView()
{
    mainWindow()->setCurrentView(this);
}

void QtSyncStatusView::createMenu()
{
    HbMenu* myMenu = menu();
    connect( myMenu->addAction( tr("Publish")), SIGNAL( triggered() ), SLOT( publish() ) );
    connect( myMenu->addAction( tr("View log")), SIGNAL( triggered() ), SLOT( viewLog() ) );
    connect( myMenu->addAction( tr("Hide")), SIGNAL( triggered() ), SLOT( hideView() ) );
    connect( myMenu->addAction( tr("Delete log")), SIGNAL( triggered() ), SLOT( deleteLog() ) );
    
    HbToolBar* myToolBar = toolBar();
    connect( myToolBar->addAction( tr("Publish")), SIGNAL( triggered() ), SLOT( publish() ) );
    connect( myToolBar->addAction( tr("View log")), SIGNAL( triggered() ), SLOT( viewLog() ) );
    connect( myToolBar->addAction( tr("Hide")), SIGNAL( triggered() ), SLOT( hideView() ) );
    connect( myToolBar->addAction( tr("Delete log")), SIGNAL( triggered() ), SLOT( deleteLog() ) );
}
bool QtSyncStatusView::event(QEvent *event)
{
    debugEvent(event);
//    if (event->type() == QEvent::ApplicationActivate) {
//        mMainWindow.show();
//    }
    return HbView::event(event);
}

void  QtSyncStatusView::debugEvent(QEvent *event)
{
    qDebug() << "event->type() = " << event->type();
    switch (event->type())
        {
        case 0:
            qDebug() << "QEvent::None, Not an event";
        break;
        
        case 130:
            qDebug() << "QEvent::AccessibilityDescription, Used to query accessibility description texts (QAccessibleEvent).";
        break;
        
        case 119:
            qDebug() << "QEvent::AccessibilityHelp, Used to query accessibility help texts (QAccessibleEvent).";
        break;
        
        case 86:
            qDebug() << "QEvent::AccessibilityPrepare, Accessibility information is requested.";
        break;
        
        case 114:
            qDebug() << "QEvent::ActionAdded, A new action has been added (QActionEvent).";
        break;
        
        case 113:
            qDebug() << "QEvent::ActionChanged, An action has been changed (QActionEvent).";
        break;
        
        case 115:
            qDebug() << "QEvent::ActionRemoved, An action has been removed (QActionEvent).";
        break;
        
        case 99:
            qDebug() << "QEvent::ActivationChange, A widget's top-level window activation state has changed.";
        break;
        
        case 121:
            qDebug() << "QEvent::ApplicationActivate, The application has been made available to the user.";
        break;
        
        //qDebug() << "QEvent::ApplicationActivated, ApplicationActivate This enum has been deprecated. Use ApplicationActivate instead.";    
        
        case 122:
            qDebug() << "QEvent::ApplicationDeactivate, The application has been suspended, and is unavailable to the user.";
        break;
        
        case 36:
           qDebug() << "QEvent::ApplicationFontChange, The default application font has changed.";
        break;
            
        case 37:
            qDebug() << "QEvent::ApplicationLayoutDirectionChange, The default application layout direction has changed.";
        break;
        
        case 38:
            qDebug() << "QEvent::ApplicationPaletteChange, The default application palette has changed.";
        break;
    
        case 35:
            qDebug() << "QEvent::ApplicationWindowIconChange, The application's icon has changed.";
        break;
        
        case 68:
            qDebug() << "QEvent::ChildAdded, An object gets a child (QChildEvent).";
        break;
        
        case 70:
            qDebug() << "QEvent::ChildInserted , An object gets a child (QChildEvent). Qt3Support only, use ChildAdded instead.";
        break;
        
        case 69:
            qDebug() << "QEvent::ChildPolished, A widget child gets polished (QChildEvent).";
        break;
        
        case 71:
            qDebug() << "QEvent::ChildRemoved, An object loses a child (QChildEvent).";
        break;
        
        case 40:
            qDebug() << "QEvent::Clipboard, The clipboard contents have changed (QClipboardEvent).";
        break;
        
        case 19:
            qDebug() << "QEvent::Close, Widget was closed (QCloseEvent).";
        break;
        
        case 200:
            qDebug() << "QEvent::CloseSoftwareInputPanel, A widget wants to close the software input panel (SIP).";
        break;
    
        case 178:
            qDebug() << "QEvent::ContentsRectChange, The margins of the widget's content rect changed.";
        break;
        
        case 82:
            qDebug() << "QEvent::ContextMenu, Context popup menu (QContextMenuEvent).";
        break;
        
        case 183:
            qDebug() << "QEvent::CursorChange, The widget's cursor has changed.";
        break;
        
        case 52:
            qDebug() << "QEvent::DeferredDelete, The object will be deleted after it has cleaned up.";
        break;
        
        case 60:
            qDebug() << "QEvent::DragEnter, The cursor enters a widget during a drag and drop operation (QDragEnterEvent).";
        break;
        
        case 62:
            qDebug() << "QEvent::DragLeave, The cursor leaves a widget during a drag and drop operation (QDragLeaveEvent).";
        break;
        
        case 61:
            qDebug() << "QEvent::DragMove, A drag and drop operation is in progress (QDragMoveEvent).";
        break;
        
        case 63:
            qDebug() << "QEvent::Drop, A drag and drop operation is completed (QDropEvent).";
        break;
    
        case 98:
            qDebug() << "QEvent::EnabledChange, Widget's enabled state has changed.";
        break;
        
        case 10:
            qDebug() << "QEvent::Enter, Mouse enters widget's boundaries.";
        break;
        
        case 150:
            qDebug() << "QEvent::EnterEditFocus, An editor widget gains focus for editing.";
        break;
        
        case 124:
            qDebug() << "QEvent::EnterWhatsThisMode, Send to toplevel widgets when the application enters What's This? mode.";
        break;
        
        case 116:
            qDebug() << "QEvent::FileOpen, File open request (QFileOpenEvent).";
        break;
        
        case 8:
            qDebug() << "QEvent::FocusIn, Widget gains keyboard focus (QFocusEvent).";
        break;
        
        case 9:
            qDebug() << "QEvent::FocusOut, Widget loses keyboard focus (QFocusEvent).";
        break;
        
        case 97:
            qDebug() << "QEvent::FontChange, Widget's font has changed.";
        break;
    
        case 188:
            qDebug() << "QEvent::GrabKeyboard, Item gains keyboard grab (QGraphicsItem only).";
        break;
        
        case 186:
            qDebug() << "QEvent::GrabMouse, Item gains mouse grab (QGraphicsItem only).";
        break;
        
        case 159:
            qDebug() << "QEvent::GraphicsSceneContextMenu, Context popup menu over a graphics scene (QGraphicsSceneContextMenuEvent).";
        break;
        
        case 164:
            qDebug() << "QEvent::GraphicsSceneDragEnter, The cursor enters a graphics scene during a drag and drop operation (QGraphicsSceneDragDropEvent).";
        break;
        
        case 166:
            qDebug() << "QEvent::GraphicsSceneDragLeave, The cursor leaves a graphics scene during a drag and drop operation (QGraphicsSceneDragDropEvent).";
        break;
        
        case 165:
            qDebug() << "QEvent::GraphicsSceneDragMove, A drag and drop operation is in progress over a scene (QGraphicsSceneDragDropEvent).";
        break;
        
        case 167:
            qDebug() << "QEvent::GraphicsSceneDrop, A drag and drop operation is completed over a scene (QGraphicsSceneDragDropEvent).";
        break;
        
        case 163:
            qDebug() << "QEvent::GraphicsSceneHelp, The user requests help for a graphics scene (QHelpEvent).";
        break;
        
        case 160:
            qDebug() << "QEvent::GraphicsSceneHoverEnter, The mouse cursor enters a hover item in a graphics scene (QGraphicsSceneHoverEvent).";
        break;
        
        case 162:
            qDebug() << "QEvent::GraphicsSceneHoverLeave, The mouse cursor leaves a hover item in a graphics scene (QGraphicsSceneHoverEvent).";
        break;
    
        case 161:
            qDebug() << "QEvent::GraphicsSceneHoverMove, The mouse cursor moves inside a hover item in a graphics scene (QGraphicsSceneHoverEvent).";
        break;
        
        case 158:
            qDebug() << "QEvent::GraphicsSceneMouseDoubleClick, Mouse press again (double click) in a graphics scene (QGraphicsSceneMouseEvent).";
        break;
        
        case 155:
            qDebug() << "QEvent::GraphicsSceneMouseMove, Move mouse in a graphics scene (QGraphicsSceneMouseEvent).";
        break;
        
        case 156:
            qDebug() << "QEvent::GraphicsSceneMousePress, Mouse press in a graphics scene (QGraphicsSceneMouseEvent).";
        break;
        
        case 157:
            qDebug() << "QEvent::GraphicsSceneMouseRelease, Mouse release in a graphics scene (QGraphicsSceneMouseEvent).";
        break;
            
        case 182:
            qDebug() << "QEvent::GraphicsSceneMove, Widget was moved (QGraphicsSceneMoveEvent).";
        break;
        
        case 181:
            qDebug() << "QEvent::GraphicsSceneResize, Widget was resized (QGraphicsSceneResizeEvent).";
        break;
    
        case 168:
            qDebug() << "QEvent::GraphicsSceneWheel, Mouse wheel rolled in a graphics scene (QGraphicsSceneWheelEvent).";
        break;
        
        case 18:
            qDebug() << "QEvent::Hide, Widget was hidden (QHideEvent).";
        break;
        
        case 27:
            qDebug() << "QEvent::HideToParent, A child widget has been hidden.";
        break;
        
        case 127:
            qDebug() << "QEvent::HoverEnter, The mouse cursor enters a hover widget (QHoverEvent).";
        break;
        
        case 128:
            qDebug() << "QEvent::HoverLeave, The mouse cursor leaves a hover widget (QHoverEvent).";
        break;
        
        case 129:
            qDebug() << "QEvent::HoverMove, The mouse cursor moves inside a hover widget (QHoverEvent).";
        break;
        
        case 96:
            qDebug() << "QEvent::IconDrag, The main icon of a window has been dragged away (QIconDragEvent).";
        break;
        
        case 101:
            qDebug() << "QEvent::IconTextChange, Widget's icon text has been changed.";
        break;
        
        case 83:
            qDebug() << "QEvent::InputMethod, An input method is being used (QInputMethodEvent).";
        break;
        
        case 6:
            qDebug() << "QEvent::KeyPress, Key press (QKeyEvent).";
        break;
        
        case 7:
            qDebug() << "QEvent::KeyRelease, Key release (QKeyEvent).";
        break;
        
        case 89:
            qDebug() << "QEvent::LanguageChange, The application translation changed.";
        break;
        
        case 90:
            qDebug() << "QEvent::LayoutDirectionChange, The direction of layouts changed.";
        break;
        
        case 76:
            qDebug() << "QEvent::LayoutRequest, Widget layout needs to be redone.";
        break;
        
        case 11:
            qDebug() << "QEvent::Leave, Mouse leaves widget's boundaries.";
        break;
        
        case 151:
            qDebug() << "QEvent::LeaveEditFocus, An editor widget loses focus for editing.";
        break;
        
        case 125:
            qDebug() << "QEvent::LeaveWhatsThisMode, Send to toplevel widgets when the application leaves What's This? mode.";
        break;
        
        case 88:
            qDebug() << "QEvent::LocaleChange, The system locale has changed.";
        break;
            
        case 176:
            qDebug() << "QEvent::NonClientAreaMouseButtonDblClick, A mouse double click occurred outside the client area.";
        break;
    
        case 174:
            qDebug() << "QEvent::NonClientAreaMouseButtonPress, A mouse button press occurred outside the client area.";
        break;
    
        case 175:
            qDebug() << "QEvent::NonClientAreaMouseButtonRelease, A mouse button release occurred outside the client area.";
        break;
    
        case 173:
            qDebug() << "QEvent::NonClientAreaMouseMove, A mouse move occurred outside the client area.";
        break;
    
        case 177:
            qDebug() << "QEvent::MacSizeChange, The user changed his widget sizes (Mac OS X only).";
        break;
    
        case 153:
            qDebug() << "QEvent::MenubarUpdated, The window's menu bar has been updated.";
        break;
    
        case 43:
            qDebug() << "QEvent::MetaCall, An asynchronous method invocation via QMetaObject::invokeMethod().";
        break;
    
        case 102:
            qDebug() << "QEvent::ModifiedChange, Widgets modification state has been changed.";
        break;
    
        case 4:
            qDebug() << "QEvent::MouseButtonDblClick, Mouse press again (QMouseEvent).";
        break;
    
        case 2:
            qDebug() << "QEvent::MouseButtonPress, Mouse press (QMouseEvent).";
        break;
    
        case 3:
            qDebug() << "QEvent::MouseButtonRelease, Mouse release (QMouseEvent).";
        break;
    
        case 5:
            qDebug() << "QEvent::MouseMove, Mouse move (QMouseEvent).";
        break;
    
        case 109:
            qDebug() << "QEvent::MouseTrackingChange, The mouse tracking state has changed.";
        break;
    
        case 13:
            qDebug() << "QEvent::Move, Widget's position changed (QMoveEvent).";
        break;
    
        case 12:
            qDebug() << "QEvent::Paint, Screen update necessary (QPaintEvent).";
        break;
    
        case 39:
            qDebug() << "QEvent::PaletteChange, Palette of the widget changed.";
        break;
    
        case 131:
            qDebug() << "QEvent::ParentAboutToChange, The widget parent is about to change.";
        break;
    
        case 21:
            qDebug() << "QEvent::ParentChange, The widget parent has changed.";
        break;
    
        case 75:
            qDebug() << "QEvent::Polish, The widget is polished.";
        break;
    
        case 74:
            qDebug() << "QEvent::PolishRequest, The widget should be polished.";
        break;
    
        case 123:
            qDebug() << "QEvent::QueryWhatsThis, The widget should accept the event if it has What's This? help.";
        break;
    
        case 199:
            qDebug() << "QEvent::RequestSoftwareInputPanel, A widget wants to open a software input panel (SIP).";
        break;
    
        case 14:
            qDebug() << "QEvent::Resize, Widget's size changed (QResizeEvent).";
        break;
    
        case 117:
            qDebug() << "QEvent::Shortcut, Key press in child for shortcut key handling (QShortcutEvent).";
        break;
    
        case 51:
            qDebug() << "QEvent::ShortcutOverride, Key press in child, for overriding shortcut key handling (QKeyEvent).";
        break;
    
        case 17:
            qDebug() << "QEvent::Show, Widget was shown on screen (QShowEvent).";
        break;
    
        case 26:
            qDebug() << "QEvent::ShowToParent, A child widget has been shown.";
        break;
    
        case 50:
            qDebug() << "QEvent::SockAct, Socket activated, used to implement QSocketNotifier.";
        break;
    
        case 192:
            qDebug() << "QEvent::StateMachineSignal, A signal delivered to a state machine (QStateMachine::SignalEvent).";
        break;
    
        case 193:
            qDebug() << "QEvent::StateMachineWrapped, The event is a wrapper for, i.e., contains, another event (QStateMachine::WrappedEvent).";
        break;
    
        case 112:
            qDebug() << "QEvent::StatusTip, A status tip is requested (QStatusTipEvent).";
        break;
    
        case 100:
            qDebug() << "QEvent::StyleChange, Widget's style has been changed.";
        break;
    
        case 87:
            qDebug() << "QEvent::TabletMove, Wacom tablet move (QTabletEvent).";
        break;
    
        case 92:
            qDebug() << "QEvent::TabletPress, Wacom tablet press (QTabletEvent).";
        break;
    
        case 93:
            qDebug() << "QEvent::TabletRelease, Wacom tablet release (QTabletEvent).";
        break;
    
        case 94:
            qDebug() << "QEvent::OkRequest, Ok button in decoration pressed. Supported only for Windows CE.";
        break;
    
        case 171:
            qDebug() << "QEvent::TabletEnterProximity, Wacom tablet enter proximity event (QTabletEvent), sent to QApplication.";
        break;
    
        case 172:
            qDebug() << "QEvent::TabletLeaveProximity, Wacom tablet leave proximity event (QTabletEvent), sent to QApplication.";
        break;
    
        case 1:
            qDebug() << "QEvent::Timer, Regular timer events (QTimerEvent).";
        break;
    
        case 120:
            qDebug() << "QEvent::ToolBarChange, The toolbar button is toggled on Mac OS X.";
        break;
    
        case 110:
            qDebug() << "QEvent::ToolTip, A tooltip was requested (QHelpEvent).";
        break;
    
        case 184:
            qDebug() << "QEvent::ToolTipChange, The widget's tooltip has changed.";
        break;
    
        case 189:
            qDebug() << "QEvent::UngrabKeyboard, Item loses keyboard grab (QGraphicsItem only).";
        break;
    
        case 187:
            qDebug() << "QEvent::UngrabMouse, Item loses mouse grab (QGraphicsItem only).";
        break;
    
        case 78:
            qDebug() << "QEvent::UpdateLater, The widget should be queued to be repainted at a later time.";
        break;
    
        case 77:
            qDebug() << "QEvent::UpdateRequest, The widget should be repainted.";
        break;
    
        case 111:
            qDebug() << "QEvent::WhatsThis, The widget should reveal What's This? help (QHelpEvent).";
        break;
    
        case 118:
            qDebug() << "QEvent::WhatsThisClicked, A link in a widget's What's This? help was clicked.";
        break;
    
        case 31:
            qDebug() << "QEvent::Wheel, Mouse wheel rolled (QWheelEvent).";
        break;
    
        case 132:
            qDebug() << "QEvent::WinEventAct, A Windows-specific activation event has occurred.";
        break;
    
        case 24:
            qDebug() << "QEvent::WindowActivate, Window was activated.";
        break;
    
        case 103:
            qDebug() << "QEvent::WindowBlocked, The window is blocked by a modal dialog.";
        break;
    
        case 25:
            qDebug() << "QEvent::WindowDeactivate, Window was deactivated.";
        break;
    
        case 34:
            qDebug() << "QEvent::WindowIconChange, The window's icon has changed.";
        break;
    
        case 105:
            qDebug() << "QEvent::WindowStateChange, The window's state (minimized, maximized or full-screen) has changed (QWindowStateChangeEvent).";
        break;
    
        case 33:
            qDebug() << "QEvent::WindowTitleChange, The window title has changed.";
        break;
    
        case 104:
            qDebug() << "QEvent::WindowUnblocked, The window is unblocked after a modal dialog exited.";
        break;
    
        case 126:
            qDebug() << "QEvent::ZOrderChange, The widget's z-order has changed. This event is never sent to top level windows.";
        break;
    
        case 169:
            qDebug() << "QEvent::KeyboardLayoutChange, The keyboard layout has changed.";
        break;
    
        case 170:
            qDebug() << "QEvent::DynamicPropertyChange, A dynamic property was added, changed or removed from the object.";
        break;
    
        case 194:
            qDebug() << "QEvent::TouchBegin, Beginning of a sequence of touch-screen and/or track-pad events (QTouchEvent)";
        break;
    
        case 195:
            qDebug() << "QEvent::TouchUpdate, Touch-screen event (QTouchEvent)";
        break;
    
        case 196:
            qDebug() << "QEvent::TouchEnd, End of touch-event sequence (QTouchEvent)";
        break;
    
        case 203:
            qDebug() << "QEvent::WinIdChange, The window system identifer for this native widget has changed";
        break;
    
        case 198:
            qDebug() << "QEvent::Gesture, A gesture was triggered (QGestureEvent)";
        break;
    
        case 202:
            qDebug() << "QEvent::GestureOverride, A gesture override was triggered (QGestureEvent)";
        break;
        
        default:
        break;
        }
}

// End of file
