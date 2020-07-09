#include "playahplaylistitem.h"
#include <QDebug>

struct PlayahPlaylistItemPrivate{
    QString filename;
    int duration;
    std::wstring title;
    std::wstring author;
};

PlayahPlaylistItem::PlayahPlaylistItem(QString file)
{
    d = new PlayahPlaylistItemPrivate();
    d->filename = file;

    TagLib::FileRef fileref(file.toUtf8());

    d->duration = fileref.audioProperties()->lengthInMilliseconds();
    d->title = fileref.tag()->title().toWString();
    d->author = fileref.tag()->artist().toWString();
}

PlayahPlaylistItem::~PlayahPlaylistItem()
{
}

QString PlayahPlaylistItem::getFileName() const
{
    return d->filename;
}

QString PlayahPlaylistItem::getTitle() const
{
    return QString::fromStdWString(d->title);
}

QString PlayahPlaylistItem::getAuthor() const
{
    return QString::fromStdWString(d->author);
}

qint64 PlayahPlaylistItem::getDuration() const
{
    return d->duration;
}
