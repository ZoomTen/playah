#include "playahcore.h"

#include <QMediaPlayer>

#include <taglib/fileref.h>
#include <QDebug>

#include <string.h>


QString PlayahCore::applicationVersion(){return "0.0.1";}

struct PlayahCorePrivate{
    QMediaPlayer* player;

    QString fileName;
    bool paused = false;
    bool ablePlayed = false;
    TagLib::FileRef* tags = nullptr;

    PlayahPlaylistModel* playlist;
};

PlayahCore::PlayahCore()
{
    d = new PlayahCorePrivate();
    d->player = new QMediaPlayer();
    d->playlist = new PlayahPlaylistModel();

    QObject::connect(d->player, &QMediaPlayer::positionChanged,
                     this,      &PlayahCore::trackPositionChanged);

    QObject::connect(d->player, &QMediaPlayer::durationChanged,
                     this,      &PlayahCore::trackDurationChanged);
}

PlayahCore::~PlayahCore()
{
    delete d;
}

PlayahPlaylistModel* PlayahCore::getPlaylist()
{
    return d->playlist;
}

bool PlayahCore::loadFile(QString fileName)
{
    d->player->setMedia(QUrl::fromLocalFile(fileName));
    if (d->player->error() == QMediaPlayer::Error::NoError){

        char* fn = new char [fileName.toStdString().size()+1];
        strcpy( fn, fileName.toStdString().c_str() );
        d->tags = new TagLib::FileRef(fn);

        d->fileName = fileName;

        emit fileLoaded(fileName);
        d->ablePlayed = true;
    } else {
        d->ablePlayed = false;
    }
    return d->ablePlayed;
}

bool PlayahCore::addFileToPlaylist(QString fileName)
{
    char* fn = new char [fileName.toStdString().size()+1];
    strcpy( fn, fileName.toStdString().c_str() );
    TagLib::FileRef tagfile(fn);

    if (!tagfile.isNull()){
        TagLib::Tag* tag = tagfile.tag();

        if (!tag->isEmpty()){
            QString title = QString(tagfile.tag()->title().to8Bit(true).c_str());
            QString author = QString(tagfile.tag()->artist().to8Bit(true).c_str());

            int songLength = tagfile.audioProperties()->lengthInMilliseconds();

            PlayahPlaylistItem newItem(fileName);
            newItem.setTitle(title);
            newItem.setAuthor(author);
            newItem.setDuration(songLength);

            d->playlist->append(newItem);
            return true;
        }
    }
    return false;
}

void PlayahCore::play(){
    d->player->play();
}

void PlayahCore::pause(){
    if (d->paused){
        d->player->play();
        d->paused = false;
    } else {
        d->player->pause();
        d->paused = true;
    }
}

void PlayahCore::stop(){
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

QString PlayahCore::getTitle()
{
    if (d->tags != nullptr){
        TagLib::String title = d->tags->tag()->title();

        if ( title != TagLib::String::null){
            return QString(title.to8Bit(true).c_str());
        }
    }
    return QString("");
}

QString PlayahCore::getAuthor()
{
    if (d->tags != nullptr){
        TagLib::String artist = d->tags->tag()->artist();

        if ( artist != TagLib::String::null){
            return QString(artist.to8Bit(true).c_str());
        }
    }
    return QString("");
}

QString PlayahCore::getFileName()
{
    return d->fileName;
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
