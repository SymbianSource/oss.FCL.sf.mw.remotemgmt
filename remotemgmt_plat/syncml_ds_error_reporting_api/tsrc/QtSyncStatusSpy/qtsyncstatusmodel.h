#ifndef QTSYNCSTATUSMODEL_H
#define QTSYNCSTATUSMODEL_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class QPersistentModelIndex;
class QModelIndex;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbComboBox;
class HbPushButton;
class HbAbstractButton;
class HbLineEdit;
class XQSettingsManager;
class XQSettingsKey;
class HbMainWindow;
class QtSyncStatusLog;

class QtSyncStatusModel : public QObject
{
    Q_OBJECT

public:
    QtSyncStatusModel(QtSyncStatusLog& log, HbDataForm *form, QObject *parent = 0);
    ~QtSyncStatusModel();
    void initModel();
    void clearSyncLog();
    void publishSyncStatusKey();
    
public slots:
    void itemDisplayed(const QModelIndex &index);
    void handleSyncErrorCodeChangeFinished();
    void handleProfileUidChangeFinished();
    void handleSyncTypeChange(int index);
    void handleSyncInitiationChange(int index);
    
    void valueChanged(const XQSettingsKey& key, const QVariant& value);

private:    
    void openSyncLogFile();
    

private:
    HbDataForm *mSettingsForm;
    HbDataFormModel *mSettingsModel;
    HbDataFormModelItem *mNsmlOpDsSyncErrorCodeItem;
    HbDataFormModelItem* mNsmlOpDsSyncProfUidItem;
    HbDataFormModelItem *mNsmlOpDsSyncTypeItem;
    HbDataFormModelItem* mNsmlOpDsSyncInitiationItem;
    
    HbLineEdit *mSyncErrorCodeEdit;
    HbLineEdit *mSyncProfileUidEdit;
    HbComboBox *mmSyncTypeComboBox;
    HbComboBox *mSyncInitiationComboBox;
    
    XQSettingsManager *mSettingsManager;
    XQSettingsKey *mNsmlOpDsSyncErrorCode;
    XQSettingsKey *mNsmlOpDsSyncProfUid;
    XQSettingsKey *mNsmlOpDsSyncType;
    XQSettingsKey *mNsmlOpDsSyncInitiation;
    
    XQSettingsKey *mDataSyncStatusKey;
    
    uint mSyncErrorCode;
    uint mSyncProfileUid;
    uint mSyncType;
    uint mSyncInitiation;
    
    QtSyncStatusLog& mSyncLog;
    };

#endif // QTSYNCSTATUSMODEL_H
