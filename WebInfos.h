#pragma once

#include <QString>
#include <QStringList>

#include "WebContents.h"

class WebInfos : public QObject
{

    Q_OBJECT

public:
    WebInfos(QString identifier, QString sdkVersion);

    void fetch();
    bool isWebNewer();
    QString name();
    QString identifier();
    QString version();
    QString link();
    QString sdk_version();

public slots:
    void infosArrived(QString str);

signals:
    void ready();

private:
    WebContents* m_wc;
    QString m_name;
    QString m_identifier;
    QString m_version;
    QString m_link;
    QString m_sdk_version;

};

