#include <qtextstream.h>
#include <qstringlist.h>
#include <qdir.h>
#include "qtsyncstatuslog.h"

const QString LogFilePath = "C:/logs/Sync/";
const QString LogFileName = LogFilePath + "QtSyncStatus.txt";

QtSyncStatusLog::QtSyncStatusLog() : mLogFile(LogFileName)
{
    open();
}

QtSyncStatusLog::~QtSyncStatusLog()
{
    mLogFile.close();
}

void QtSyncStatusLog::clear()
{
    mLogFile.remove();
    open();    
}

void QtSyncStatusLog::write(QString& string)
{
    QTextStream stream(&mLogFile);
    stream << string;
    stream.flush();
}

QStringList QtSyncStatusLog::lines()
{
    QStringList list;
    QTextStream stream(&mLogFile);
    stream.seek(0);
    while (!stream.atEnd()) {
        list.append(stream.readLine());
    }
    return list;
}

void QtSyncStatusLog::open()
{
    QDir dir(LogFilePath);
    if (!dir.exists()) {
        dir.mkpath(LogFilePath);
    }
    if (!mLogFile.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
        qFatal("error opening log file");
        return;
    }    
}

   
