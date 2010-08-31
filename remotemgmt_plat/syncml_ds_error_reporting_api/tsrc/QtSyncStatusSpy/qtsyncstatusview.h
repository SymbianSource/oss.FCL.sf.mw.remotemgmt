#ifndef QTSYNCSTATUSVIEW_H
#define QTSYNCSTATUSVIEW_H

#include <QObject>
#include <hbview.h>

class QGraphicsItem;
class HbDataForm;
class HbAction;
class QtSyncStatusModel;
class HbMainWindow;
class QtSyncStatusLog;

class QtSyncStatusView : public HbView
{
    Q_OBJECT

public:
    QtSyncStatusView(QtSyncStatusLog& log, QGraphicsItem *parent = 0);
    ~QtSyncStatusView();
    
signals:
    void showLog();
    
public slots:    
    void publish();
    void viewLog();
    void hideView();
    void deleteLog();
    void activateView();    
    
private:
    void createMenu();
    
    virtual bool event(QEvent *event);
    
    void debugEvent(QEvent *event);
    
private:
    HbDataForm *mSyncStatusForm;
    QtSyncStatusModel *mSyncStatusModel;
};

#endif // QTSYNCSTATUSVIEW_H

