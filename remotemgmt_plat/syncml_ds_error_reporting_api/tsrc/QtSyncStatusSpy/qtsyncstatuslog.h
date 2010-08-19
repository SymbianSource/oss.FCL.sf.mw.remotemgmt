#ifndef QTSYNCSTATUSLOG_H
#define QTSYNCSTATUSLOG_H

#include <QObject>
#include <qfile.h>

class QtSyncStatusLog : public QObject
{
public:
    QtSyncStatusLog();
    ~QtSyncStatusLog();
    
    void clear();
    void write(QString& string);
    QStringList lines();
    
private:
    void open();

private:
    QFile mLogFile;
};

#endif // QTSYNCSTATUSLOG_H
