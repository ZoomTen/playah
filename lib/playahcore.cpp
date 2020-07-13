#include "playahcore.h"

#include <QMediaPlayer>

#include <QDebug>

#include <string.h>


QString PlayahCore::applicationVersion(){return "0.0.5";}

struct PlayahCorePrivate{
    PlayahCore* playah;

    QMediaPlayer* player;
    bool paused = false;
    bool stopped = true;
    bool ablePlayed = true;

    PlayahPlaylistModel* playlist;
    PlayahPlaylistItem*  currentItem;
};
PlayahCorePrivate* PlayahCore::d = new PlayahCorePrivate();

PlayahCore::PlayahCore()
{
    d->player = new QMediaPlayer();
    d->playlist = new PlayahPlaylistModel();

    QObject::connect(d->player, &QMediaPlayer::positionChanged,
                     this,      &PlayahCore::trackPositionChanged);

    QObject::connect(d->player, &QMediaPlayer::durationChanged,
                     this,      &PlayahCore::trackDurationChanged);

    QObject::connect(d->player, &QMediaPlayer::mediaStatusChanged,
                     this,      [=](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::MediaStatus::EndOfMedia){
            qDebug() << "EOF";
            d->stopped = true;
            d->paused = false;
        }
    });
    QObject::connect(d->player, &QMediaPlayer::audioAvailableChanged,
            this,      [=](bool available){
            d->ablePlayed = available;
    });
}

PlayahCore::~PlayahCore()
{
    delete d;
}

PlayahPlaylistModel* PlayahCore::getPlaylist()
{
    return d->playlist;
}

PlayahPlaylistItem *PlayahCore::getPlaylistItem()
{
    return d->currentItem;
}

PlayahCore *PlayahCore::instance()
{
    if (d->playah == nullptr) d->playah = new PlayahCore();
    return d->playah;
}

bool PlayahCore::loadFile(QString fileName)
{
    int newPlaylistItem = addFileToPlaylist(fileName);

    bool itemLoaded = false;
    if (newPlaylistItem != 0) itemLoaded = loadPlaylistItemNumber(newPlaylistItem-1);
    return itemLoaded;
}

int PlayahCore::addFileToPlaylist(QString fileName)
{
    PlayahPlaylistItem playlistEntry(fileName);
    d->playlist->append(playlistEntry);
    return d->playlist->itemCount();
}

bool PlayahCore::loadPlaylistItemNumber(int number)
{
    if (number > d->playlist->itemCount()) return false;
    else {
        d->currentItem = d->playlist->getItem(number);
        QString fileName = d->currentItem->getFileName();

        d->player->setMedia(QUrl::fromLocalFile(fileName));
        if (d->player->error() == QMediaPlayer::Error::NoError){
            emit fileLoaded(fileName);
            return true;
        } else {
            return false;
        }
        return false;
    }
}

void PlayahCore::play(){
    d->stopped = false;
    d->paused = false;
    d->player->play();
}

void PlayahCore::pause(){
    if (!d->stopped){
        if (d->paused){
            d->player->play();
            d->paused = false;
        } else {
            d->player->pause();
            d->paused = true;
        }
    }
}

void PlayahCore::stop(){
    d->stopped = true;
    d->paused = false;
    d->player->stop();
}

bool PlayahCore::seek(qint64 offset)
{
    if (d->player->isSeekable()){
        qint64 target = d->player->position() + offset;
        if (target > d->player->duration()){
            d->player->setPosition(d->player->duration());
        } else {
            d->player->setPosition(target);
        }
    }
    return d->player->isSeekable();
}

bool PlayahCore::seekTo(qint64 destination)
{
    if (d->player->isSeekable()){
        d->player->setPosition(destination);
    }
    return d->player->isSeekable();
}

QTime PlayahCore::getPositionAsTime()
{
    return QTime::fromMSecsSinceStartOfDay(d->player->position());
}

QTime PlayahCore::getDurationAsTime()
{
    return QTime::fromMSecsSinceStartOfDay(d->player->duration());
}

QTime PlayahCore::playlistDurationAsTime()
{
    return d->playlist->getTotalPlaytime();
}

QString PlayahCore::getTitle()
{
    return d->currentItem->getTitle();
}

QString PlayahCore::getAuthor()
{
    return d->currentItem->getAuthor();
}

QString PlayahCore::getFileName()
{
    return d->currentItem->getFileName();
}

bool PlayahCore::isPlayable()
{
    return d->ablePlayed;
}

double PlayahCore::currentProgress()
{
    if (d->player->duration() != 0){
    return (double)d->player->position() / (double)d->player->duration();
    } else {
        return 0;
    }
}
