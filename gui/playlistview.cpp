#include "playlistview.h"

#include <QMouseEvent>
#include <QDebug>
#include <QMenu>

PlaylistView::PlaylistView(QWidget*)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &PlaylistView::customContextMenuRequested,
            this, [=](const QPoint &p){
        QModelIndex selectedItem = this->indexAt(p);
        QMenu itemMenu(this);
        if (selectedItem.isValid()){
            itemMenu.setTitle(tr("With this item"));
            QAction* delet = new QAction(tr("Delete"));
            connect(delet, &QAction::triggered,
                    this,  [=]{
                qDebug() << "delet";
                emit setDeletedItem(selectedItem);
            });
            itemMenu.addAction(delet);
        } else {
            itemMenu.setTitle(tr("To current playlist"));
            QAction* add = new QAction(tr("Add"));
            connect(add, &QAction::triggered,
                    this,  [=]{
                qDebug() << "add";
                emit openFileDialog();
            });
            itemMenu.addAction(add);
        }
        itemMenu.exec(this->viewport()->mapToGlobal(p));
    });
}

PlaylistView::~PlaylistView()
{

}

void PlaylistView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        QModelIndex selectedItem = this->indexAt(event->pos());
        if (selectedItem.isValid())
            emit selectedPlaylistItem(selectedItem);
    }
}
