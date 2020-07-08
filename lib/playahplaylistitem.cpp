#include "playahplaylistitem.h"

PlayahPlaylistItem::PlayahPlaylistItem(QString file)
{
    filename_ = file;
}

PlayahPlaylistItem::~PlayahPlaylistItem()
{
}

QString PlayahPlaylistItem::getFileName() const{ return filename_; }

QString PlayahPlaylistItem::getTitle() const   { return title_; }
void PlayahPlaylistItem::setTitle(QString title){ title_ = title; }

QString PlayahPlaylistItem::getAuthor() const  { return author_; }
void PlayahPlaylistItem::setAuthor(QString author){ author_ = author; }

qint64 PlayahPlaylistItem::getDuration() const { return duration_; }
void PlayahPlaylistItem::setDuration(qint64 duration){ duration_ = duration; }
