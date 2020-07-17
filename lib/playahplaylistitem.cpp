#include "playahplaylistitem.h"

#include <QDebug>
#include <QImage>

#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/attachedpictureframe.h>

struct PlayahPlaylistItemPrivate{
    QString filename;

    QImage* albumArt;

    int id;
    int duration;
    std::wstring title;
    std::wstring author;
    std::wstring album;
    int year;
    std::wstring unsyncLyrics;
    std::wstring bpmString;
};

PlayahPlaylistItem::PlayahPlaylistItem(QString file, int id)
{
    d = new PlayahPlaylistItemPrivate();
    d->filename = file;
    d->albumArt = nullptr;
    d->id = id;

    if (file.endsWith(".mp3", Qt::CaseInsensitive)){
        TagLib::MPEG::File mp3(QFile::encodeName(file).constData());

        qDebug() << "has id3v1?" << mp3.hasID3v1Tag();
        qDebug() << "has id3v2?" << mp3.hasID3v2Tag();
        TagLib::ID3v2::Tag* id3v2 = mp3.ID3v2Tag();

        if (id3v2){
            qDebug() << "has id3v2";
            TagLib::ID3v2::FrameListMap frames = id3v2->frameListMap();
            if (frames.contains("USLT")){
                qDebug() << "has uslt";
                TagLib::ID3v2::FrameList uslt = frames["USLT"];
                TagLib::ID3v2::UnsynchronizedLyricsFrame* ulyrics = static_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(uslt.front());
                if (ulyrics) d->unsyncLyrics = ulyrics->text().toWString();
            }
            if (frames.contains("TBPM")){
                qDebug() << "has tbpm";
                TagLib::ID3v2::FrameList tbpm = frames["TBPM"];
                TagLib::ID3v2::TextIdentificationFrame* bpms = static_cast<TagLib::ID3v2::TextIdentificationFrame*>(tbpm.front());
                if (bpms) d->bpmString = bpms->toString().toWString();
            }
            if (frames.contains("APIC")){
                qDebug() << "has apic";
                TagLib::ID3v2::FrameList apic = frames["APIC"];
                TagLib::ID3v2::AttachedPictureFrame* pic = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(apic.front());
                if (pic) {
                    QByteArray data;
                    data.append(pic->picture().data(), pic->picture().size());

                    d->albumArt = new QImage();
                    d->albumArt->loadFromData(data);
                }
            }
        }

        /* this has to be split due to the way Windows reads files */
        TagLib::Tag*             tag      = mp3.tag();
        TagLib::AudioProperties* audProps = mp3.audioProperties();

        if (audProps){
            d->duration = audProps->lengthInMilliseconds();
        }

        if (tag){
            d->title = tag->title().toWString();
            d->author = tag->artist().toWString();
            d->album = tag->album().toWString();
            d->year = tag->year();
        }
    } else {
        /* I really need to learn how templates work lol */
        TagLib::FileRef fileref(QFile::encodeName(file).constData());

        TagLib::Tag*             tag      = fileref.tag();
        TagLib::AudioProperties* audProps = fileref.audioProperties();

        if (audProps){
            d->duration = audProps->lengthInMilliseconds();
        }

        if (tag){
            d->title = tag->title().toWString();
            d->author = tag->artist().toWString();
            d->album = tag->album().toWString();
            d->year = tag->year();
        }
    }
}

PlayahPlaylistItem::~PlayahPlaylistItem()
{
}

QString PlayahPlaylistItem::getFileName() const
{
    return d->filename;
}

qint64 PlayahPlaylistItem::getID() const
{
    return d->id;
}

QString PlayahPlaylistItem::getTitle() const
{
    QString title = QString::fromStdWString(d->title);
    if (title.isEmpty()){
        QFileInfo fi(d->filename);
        return fi.baseName();
    } else {
        return title;
    }
}

QString PlayahPlaylistItem::getAuthor() const
{
    return QString::fromStdWString(d->author);
}

qint64 PlayahPlaylistItem::getDuration() const
{
    return d->duration;
}

QString PlayahPlaylistItem::getUnsyncLyrics() const
{
    return QString::fromStdWString(d->unsyncLyrics);
}

QString PlayahPlaylistItem::getBPMString() const
{
    return QString::fromStdWString(d->bpmString);
}

QString PlayahPlaylistItem::getAlbum() const
{
    return QString::fromStdWString(d->album);
}

QImage* PlayahPlaylistItem::getAlbumArt() const
{
    return d->albumArt;
}

int PlayahPlaylistItem::getYear() const
{
    return d->year;
}
