#pragma once

#include "playah_global.h"
#include <QAbstractTableModel>
#include "playahplaylistitem.h"

struct PlayahPlaylistModelPrivate;
class PLAYAH_EXPORT PlayahPlaylistModel : public QAbstractTableModel
{
public:
    enum DisplayColumns{
        Title,
        Author,
        FileName,
        Duration,
//        EID,
        COLUMNS_END
    };

    PlayahPlaylistModel(QObject* parent = nullptr);
    ~PlayahPlaylistModel();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    int itemCount();

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QString getFileName(const QModelIndex &index);

    PlayahPlaylistItem* getItem(int itemNumber);
    PlayahPlaylistItem* getByIndex(const QModelIndex &index);

    void append(const QString filename);
    void insertInto(const QString filename, int row);

    void removeEntryNumber(int i);

    QTime getTotalPlaytime();

    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList &indexes) const override;
    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int, const QModelIndex &parent) override;

    QModelIndex searchHasId(int id = 0, QModelIndex parent = QModelIndex());
private:
    PlayahPlaylistModelPrivate* d;
};
