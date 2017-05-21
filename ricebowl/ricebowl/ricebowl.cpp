#include "ricebowl.h"
#include "ui_ricebowl.h"
#include <unistd.h>
#include <QMediaPlayer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFileDialog>
#include <QTimer>

ricebowl::ricebowl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ricebowl)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);

    loadConfig();

    if(localMode == false){
        connect(&server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
        server.listen(QHostAddress::Any, commandPort);

        connect(&metaServer, SIGNAL(newConnection()),this, SLOT(acceptConnectionForMeta()));
        metaServer.listen(QHostAddress::Any, metadataPort);
    }

    if(directory.isEmpty()) return;

    QDir dir(directory);
    QStringList files = dir.entryList(QStringList() << "*.mp3",QDir::Files);
    QList<QMediaContent> content;
    for(const QString& f:files) content.push_back(QUrl::fromLocalFile(dir.path()+"/" + f));
    playlist->addMedia(content);
    player->setPlaylist(playlist);

    connect(player, SIGNAL(positionChanged(qint64)),this, SLOT(setCurrentMediaDuration(qint64)));

}

ricebowl::~ricebowl()
{
    delete ui;
}

void ricebowl::loadConfig()
{

    QString configString;
    configString = "/home/"+qgetenv("USER")+"/.ricebowl/config";
    QFile configFile(configString);

    if(!configFile.exists()) return;

    configFile.open(QIODevice::ReadOnly);

    for(int i=0; i < 10; i++)
    {
        QString configString = configFile.readLine();

        // path to songs
        if(configString.startsWith("music_path:")){
            configString.remove(0,12);
            configString.chop(1);
            directory = configString;

            // playback mode  - clean up this mess later
        }else if(configString.startsWith("default_playback_mode:"))
        {
            configString.remove(0,23);

            if(configString.contains("sequential"))
            {
                playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Sequential);
            }else if(configString.contains("current_once"))
            {
                playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::CurrentItemOnce);
            }else if(configString.contains("current_loop"))
            {
                playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::CurrentItemInLoop);
            }else if(configString.contains("loop"))
            {
                playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
            }else if(configString.contains("shuffle"))
            {
                playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Random);
            }
        }else if(configString.startsWith("string_separator:"))
        {
            configString.remove(0,26);
            configString.chop(1);
            defStrSep = configString;
        }else if(configString.startsWith("command_port:"))
        {
            configString.remove(0,14);
            configString.chop(1);
            commandPort = configString.toInt();
        }else if(configString.startsWith("metadata_port:"))
        {
            configString.remove(0,15);
            configString.chop(1);
            metadataPort = configString.toInt();
        }else if(configString.startsWith("reading_buffer_size:")){
            configString.remove(0,21);
            configString.chop(1);
            readingBufferSize = configString.toInt();
        }
    }
}

void ricebowl::acceptConnection()
{
    client = server.nextPendingConnection();
    connect(client, SIGNAL(readyRead()),this, SLOT(startReading()));
}

void ricebowl::acceptConnectionForMeta()
{
    metaClient = metaServer.nextPendingConnection();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(startSending()));
    timer->start(1000);
    //connect(metaClient, SIGNAL(readyRead()),this, SLOT(startSending()));
}

void ricebowl::startReading()
{
    char buffer[64] = {0};  // the buffer seems a bit large
    client->read(buffer, client->bytesAvailable());
    command = new QString(buffer);
    executeCommand();
}

void ricebowl::setCurrentMediaDuration(qint64 dur)
{
    currentMediaDuration = dur;
}


void ricebowl::startSending()
{
    // metadata string construction, to be changed
    QString metaData = player->metaData("Title").toString() + defStrSep + player->metaData("ContributingArtist").toString() +
            defStrSep + player->metaData("AlbumTitle").toString() + defStrSep +
            QString::number(currentMediaDuration) + defStrSep +
            QString::number(player->duration()) + "\n";
    std::string str = metaData.toUtf8().constData();
    const char* p = str.c_str();
    metaClient->write(p, metaData.length());
    metaClient->waitForBytesWritten(-1);
}

// needs more commands
void ricebowl::executeCommand()
{
    if(command->startsWith("play")){
        player->play();
    }else if(command->startsWith("next")){
        player->pause();
        playlist->next();
        player->play();
    }else if(command->startsWith("prev")){
        player->pause();
        playlist->previous();
        player->play();
    }else if(command->startsWith("+")){
        player->setVolume(player->volume()+volumeModifier);
    }else if(command->startsWith("-")){
        player->setVolume(player->volume()-volumeModifier);
    }else if(command->startsWith("pause")){
        player->pause();
    }else if(command->startsWith("stop")){
        player->stop();
    }else if(command->startsWith("repeat")){
        playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::CurrentItemInLoop);
    }else if(command->startsWith("shuffle")){
        playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Random);
    }else if(command->startsWith("kill")){ // brutal, but it must be
        std::exit(1);
    }
}



// local gui stuff
// how not to block the main thread 101

void ricebowl::runLocalMetaDataTimer()
{
    QTimer *localMetadataTimer = new QTimer(this);
    connect(localMetadataTimer, SIGNAL(timeout()), this, SLOT(updateLocalMetaData()));
    localMetadataTimer->start(1000);
}

void ricebowl::updateLocalMetaData()
{
    ui->labelArtist->setText("lol");
    ui->labelArtist->setText(player->metaData("ContributingArtist").toString());
    ui->labelAlbum->setText(player->metaData("AlbumTitle").toString());
    ui->labelTitle->setText(player->metaData("Title").toString());
    ui->labelYear->setText(player->metaData("Year").toString());
}

void ricebowl::on_pushButtonPlay_clicked()
{
    runLocalMetaDataTimer();
    player->play();
}

void ricebowl::on_pushButtonPause_clicked()
{
    player->pause();
}

void ricebowl::on_pushButtonStop_clicked()
{
    player->stop();
}

void ricebowl::on_pushButtonPrev_clicked()
{
    player->pause();
    playlist->previous();
    player->play();
}

void ricebowl::on_pushButtonNext_clicked()
{
    player->pause();
    playlist->next();
    player->play();
}
