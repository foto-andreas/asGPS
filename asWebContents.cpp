class WebContents : public QObject
{

Q_OBJECT

public:
    WebContents();
    void fetch(); 
    virtual void handle(QString contents) = 0;

public slots:
    void replyFinished(QNetworkReply*);

public signals:
    void handler(Qtring str);

private:
    QNetworkAccessManager* m_manager;
};


WebContents::WebContents()
{
    m_manager = new QNetworkAccessManager(this);

    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
         this, SLOT(replyFinished(QNetworkReply*)));

}

void WebContents::fetch()
{
    m_manager->get(QNetworkRequest(QUrl("http://schrell.de/AfterShotPro/WebTools/PlugVersion.php?sdk=8&id=%25")));
}

void WebContents::register(void handler(QString contents)) {
    
}

void WebContents::replyFinished(QNetworkReply* pReply)
{

    QByteArray data=pReply->readAll();
    QString str(data);
    handler(str);
    
}
