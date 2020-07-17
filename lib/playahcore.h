#pragma once

#include "playah_global.h"
#include "playahplaylistmodel.h"
#include <QtCore>

struct PlayahCorePrivate;
class PLAYAH_EXPORT PlayahCore : public QObject
{
    Q_OBJECT
public:
    explicit PlayahCore();
    ~PlayahCore();

    // loads a file into the playlist and then plays them
    bool loadFile(QString fileName);

    // returns the new length of the playlist
    int addFileToPlaylist(QString fileName);
    bool loadPlaylistItemNumber(int number);

    void play();
    void pause();
    void stop();
    bool seek(qint64 offset);
    bool seekTo(qint64 offset);

    QTime getPositionAsTime();
    QTime getDurationAsTime();

    QTime playlistDurationAsTime();

    QString getTitle();
    QString getAuthor();
    QString getFileName();

    bool isPlayable();

    double currentProgress();

    static QString applicationVersion();

    PlayahPlaylistModel* getPlaylist();
    PlayahPlaylistItem* getPlaylistItem();

    static PlayahCore* instance();

signals:
    void trackDurationChanged(qint64 duration);
    void trackPositionChanged(qint64 position);
    void fileLoaded(QString fileName);
    void stopped();

private:
    static PlayahCorePrivate* d;
};
