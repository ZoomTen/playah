#include "playahplaylistmodel.h"
#include "playahplaylistitem.h"

struct PlayahPlaylistModelPrivate{
    QList<PlayahPlaylistItem> items;
};

PlayahPlaylistModel::PlayahPlaylistModel(QObject* parent):
    QAbstractTableModel(parent)
{
    d = new PlayahPlaylistModelPrivate();
}

PlayahPlaylistModel::~PlayahPlaylistModel()
{
    delete d;
}

int PlayahPlaylistModel::rowCount(const QModelIndex &) const
{
    return d->items.count();
}

int PlayahPlaylistModel::columnCount(const QModelIndex &) const
{
    return PlayahPlaylistItem::COLUMNS_END;
}

QVariant PlayahPlaylistModel::data(const QModelIndex &index, int role) const
{
    // this is used to display the model's items row by row, column by column
    if (role != Qt::DisplayRole) return QVariant();
    if (!index.isValid()) return QVariant();

    const PlayahPlaylistItem item = d->items[index.row()];

    switch(index.column()){
    case PlayahPlaylistItem::Title:
        return item.getTitle();
    case PlayahPlaylistItem::Author:
        return item.getAuthor();
    case PlayahPlaylistItem::Duration:
        return QTime::fromMSecsSinceStartOfDay(item.getDuration()).toString("mm:ss");
    case PlayahPlaylistItem::FileName:
        return item.getFileName();
    default:
        return QVariant();
    }
}

QVariant PlayahPlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // this is used to display the model's headers. a.k.a the titles
    if (orientation != Qt::Horizontal) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();
    switch(section){
    case PlayahPlaylistItem::Title:
        return tr("Title");
    case PlayahPlaylistItem::Author:
        return tr("Author");
    case PlayahPlaylistItem::Duration:
        return tr("Duration");
    case PlayahPlaylistItem::FileName:
        return tr("File name");
    default:
        return QVariant();
    }
}

QString PlayahPlaylistModel::getFileName(const QModelIndex &index)
{
    if (!index.isValid()) return "";
    const PlayahPlaylistItem item = d->items[index.row()];
    return item.getFileName();
}

void PlayahPlaylistModel::append(const PlayahPlaylistItem &item)
{
    beginInsertRows(QModelIndex(), d->items.count(), d->items.count());
    d->items.append(item);
    endInsertRows();
}
