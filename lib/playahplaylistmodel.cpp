#include "playahplaylistmodel.h"
#include "playahplaylistitem.h"

#include <QDebug>

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
    return COLUMNS_END;
}

int PlayahPlaylistModel::itemCount()
{
    return d->items.count();
}

QVariant PlayahPlaylistModel::data(const QModelIndex &index, int role) const
{
    // this is used to display the model's items row by row, column by column
    if (role != Qt::DisplayRole) return QVariant();
    if (!index.isValid()) return QVariant();

    const PlayahPlaylistItem item = d->items[index.row()];

    switch(index.column()){
    case Title:
        return item.getTitle();
    case Author:
        return item.getAuthor();
    case Duration:
        return QTime::fromMSecsSinceStartOfDay(item.getDuration()).toString("mm:ss");
    case FileName:
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
    case Title:
        return tr("Title");
    case Author:
        return tr("Author");
    case Duration:
        return tr("Duration");
    case FileName:
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

PlayahPlaylistItem* PlayahPlaylistModel::getItem(int itemNumber)
{
    return &d->items[itemNumber];
}

void PlayahPlaylistModel::append(const PlayahPlaylistItem &item)
{
    // TODO: make this call insertInto
    beginInsertRows(QModelIndex(), d->items.count(), d->items.count());
    d->items.append(item);
    endInsertRows();
}

void PlayahPlaylistModel::insertInto(const PlayahPlaylistItem &item, int row)
{
    beginInsertRows(QModelIndex(), row, row);
    d->items.insert(row, item);
    endInsertRows();
}

void PlayahPlaylistModel::removeEntryNumber(int i)
{
    beginRemoveRows(QModelIndex(), i, i);
    d->items.removeAt(i);
    endRemoveRows();
}

QTime PlayahPlaylistModel::getTotalPlaytime()
{
    quint64 runTime = 0;
    for (PlayahPlaylistItem item : d->items){
        runTime += item.getDuration();
    }
    return QTime::fromMSecsSinceStartOfDay(runTime);
}

QStringList PlayahPlaylistModel::mimeTypes() const
{
    // apparently I only need this
    QStringList types;
    types.append("text/uri-list");
    return types;
}

QMimeData *PlayahPlaylistModel::mimeData(const QModelIndexList &indexes) const
{
        QMimeData* mime = new QMimeData();
        QList<QUrl> files;
        for (QModelIndex index : indexes) {
            files.append(QUrl(d->items.at(index.row()).getFileName()));
        }
        mime->setUrls(files);
        return mime;
}

Qt::DropActions PlayahPlaylistModel::supportedDropActions() const
{
    return Qt::CopyAction;
}

Qt::ItemFlags PlayahPlaylistModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    return Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled | defaultFlags;
}

bool PlayahPlaylistModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::CopyAction) {
        if (data->hasUrls()) {
            for (QUrl url : data->urls()) {
                PlayahPlaylistItem newItem(url.toLocalFile());
                if (row < 0)
                    this->append(newItem);
                else
                    this->insertInto(newItem, row);
            }
            return true;
        } else {
            return false;
        }
    } else if (action == Qt::IgnoreAction) {
        return true;
    }
    return true;
}
