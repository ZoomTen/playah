#pragma once

#include <QtCore>

#include "taglib/fileref.h"
#include "taglib/tag.h"

struct PlayahPlaylistItemPrivate;
class PlayahPlaylistItem
{
public:
    PlayahPlaylistItem(QString fileName);
    ~PlayahPlaylistItem();

    QString getFileName() const;

    QString getTitle() const;
    QString getAuthor() const;
    qint64 getDuration() const;
private:
    PlayahPlaylistItemPrivate* d;
};
