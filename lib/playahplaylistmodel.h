#pragma once

#include <QAbstractTableModel>
#include "playahplaylistitem.h"

struct PlayahPlaylistModelPrivate;
class PlayahPlaylistModel : public QAbstractTableModel
{
public:
    enum DisplayColumns{
        Title,
        Author,
        FileName,
        Duration,
        COLUMNS_END
    };

    PlayahPlaylistModel(QObject* parent = nullptr);
    ~PlayahPlaylistModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    int itemCount();

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QString getFileName(const QModelIndex &index);

    const PlayahPlaylistItem getItem(int itemNumber);

    void append(const PlayahPlaylistItem & item);
private:
    PlayahPlaylistModelPrivate* d;
};
