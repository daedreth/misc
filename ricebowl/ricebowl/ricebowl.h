#ifndef RICEBOWL_H
#define RICEBOWL_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class ricebowl;
}

class ricebowl : public QMainWindow
{
    Q_OBJECT

public:
    explicit ricebowl(QWidget *parent = 0);
    ~ricebowl();

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QString *command;
    QString directory = "/home/" + qgetenv("USER") + "/Music";
    int volumeModifier = 20;
    QString defStrSep = " - ";
    qint64 currentMediaDuration;
    int commandPort = 6363;
    int metadataPort = 6364;
    int readingBufferSize = 64;
    bool localMode = false;

public slots:
    void acceptConnection();
    void acceptConnectionForMeta();
    void startReading();
    void startSending();
    void executeCommand();
    void loadConfig();
    void setCurrentMediaDuration(qint64 dur);
    void updateLocalMetaData();
    void runLocalMetaDataTimer();

private slots:
    void on_pushButtonPlay_clicked();

    void on_pushButtonPause_clicked();

    void on_pushButtonStop_clicked();

    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    Ui::ricebowl *ui;
    QTcpServer server;
    QTcpSocket* client;
    QTcpServer metaServer;
    QTcpSocket* metaClient;
};

#endif // RICEBOWL_H
