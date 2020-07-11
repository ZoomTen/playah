#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QTreeView>

class PlaylistView : public QTreeView
{
    Q_OBJECT
public:
    explicit PlaylistView(QWidget* = nullptr);
    ~PlaylistView();
signals:
    void selectedPlaylistItem(QModelIndex item);
    void setDeletedItem(QModelIndex item);
    void openFileDialog();
protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // PLAYLISTVIEW_H
