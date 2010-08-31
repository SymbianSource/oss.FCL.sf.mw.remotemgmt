#ifndef QTSYNCSTATUSLOGVIEW_H
#define QTSYNCSTATUSLOGVIEW_H

#include <QObject>
#include <hbview.h>
#include <QFile>
#include <QTextStream>

class QGraphicsItem;
class HbDataForm;
class HbAction;
class QtSyncStatusModel;
class HbMainWindow;
class HbWidget;
class HbTextItem;
class QGraphicsLinearLayout;
class QtSyncStatusLog;


class QtSyncStatusLogView : public HbView
{
    Q_OBJECT

public:
    QtSyncStatusLogView(QtSyncStatusLog& log, QGraphicsItem *parent = 0);
    ~QtSyncStatusLogView();
    
signals:
    void comingBack();
    
public slots:
    void activateView();
    
private slots:
    void updateView();
    void deleteLog();
    void goToMainView();

private:
    void createMenu();
    void openSyncLogFile();
    virtual void showEvent(QShowEvent * event);
    void readLog();

private:    
    HbTextItem* mTextItem;
    QtSyncStatusLog& mSyncLog;
};

#endif // QTSYNCSTATUSLOGVIEW_H
