#pragma once

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>

class WebContents : public QObject
{

    Q_OBJECT

public:
    WebContents();
    void fetch(QString url);

public slots:
    void replyFinished(QNetworkReply*);

signals:
    void stringAvailable(QString str);

private:
    QNetworkAccessManager* m_manager;

};
