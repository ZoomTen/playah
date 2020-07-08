#pragma once

#include <QtCore>

struct PlayahPlaylistItemPrivate;
class PlayahPlaylistItem
{
public:
    enum DisplayColumns{
        Title,
        Author,
        FileName,
        Duration,
        COLUMNS_END
    };
    PlayahPlaylistItem(QString fileName);
    ~PlayahPlaylistItem();

    QString getFileName() const;

    QString getTitle() const;
    void setTitle(QString title);

    QString getAuthor() const;
    void setAuthor(QString author);

    qint64 getDuration() const;
    void setDuration(qint64 duration);
private:
    PlayahPlaylistItemPrivate* d;
    QString filename_;
    QString title_;
    QString author_;
    qint64 duration_;
};
