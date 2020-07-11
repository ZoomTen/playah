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

    TagLib::FileName fn(file.toUtf8());
    TagLib::FileRef fileref(fn);

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
