#pragma once

#include "playah_global.h"
#include <QtCore>

#ifdef WIN32
#define TAGLIB_STATIC
#endif
#include <taglib/fileref.h>
#include <taglib/tag.h>

struct PlayahPlaylistItemPrivate;
class PLAYAH_EXPORT PlayahPlaylistItem
{
public:
    PlayahPlaylistItem(QString fileName);
    ~PlayahPlaylistItem();

    QString getFileName() const;

    QString getTitle() const;
    QString getAuthor() const;
    qint64 getDuration() const;
    QString getUnsyncLyrics() const;
    QString getBPMString() const;
    QString getAlbum() const;
    QImage* getAlbumArt() const;
    int getYear() const;
private:
    PlayahPlaylistItemPrivate* d;
};
