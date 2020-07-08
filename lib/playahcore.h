#pragma once

#include "playah_global.h"
#include "playahplaylistmodel.h"
#include <QtCore>

struct PlayahCorePrivate;
class PLAYAH_EXPORT PlayahCore : public QObject
{
    Q_OBJECT
public:
    PlayahCore();
    ~PlayahCore();

    bool loadFile(QString fileName);
    bool addFileToPlaylist(QString fileName);

    void play();
    void pause();
    void stop();
    bool seek(qint64 offset);
    bool seekTo(qint64 offset);

    QTime getPositionAsTime();
    QTime getDurationAsTime();

    QString getTitle();
    QString getAuthor();
    QString getFileName();

    bool isPlayable();

    double currentProgress();

    QString applicationVersion();

    PlayahPlaylistModel* getPlaylist();

signals:
    void trackDurationChanged(qint64 duration);
    void trackPositionChanged(qint64 position);
    void fileLoaded(QString fileName);

private:
    PlayahCorePrivate* d;
};
