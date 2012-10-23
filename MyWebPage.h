#pragma once

#include <QWebPage>
#include <QWebView>
#include <QDebug>

class MyWebPage : public QWebPage
{

//    /** We are a Chrome. So we can do moves and clicks of markers. */
//    virtual QString userAgentForUrl(const QUrl& url) const {
//        Q_UNUSED(url);
//        return "Chrome/20.0";
//    }

    virtual void javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID ) {
        qDebug() << "JavaScript:" << message << "line:" << lineNumber << "source:" << sourceID;
    }
};
