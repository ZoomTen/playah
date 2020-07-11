#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <playahcore.h>
#include <playahplaylistmodel.h>

#include <QFileDialog>
#include <QMessageBox>

#include "aboutwindow.h"

#include <QDebug>

#include <QPainter>

#include "playlistview.h"

struct MainWindowPrivate{
    PlayahCore* playah;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    d = new MainWindowPrivate();
    d->playah = PlayahCore::instance();

    ui->frame->installEventFilter(this);

//    d->playah = new PlayahCore();

    connect(d->playah, &PlayahCore::trackDurationChanged,
            this,      [=](qint64 duration){
        ui->seekBar->setMaximum(duration);
    });
    connect(d->playah, &PlayahCore::fileLoaded,
            this,      [=](QString fileName){
        ui->statusbar->showMessage(tr("Successfully loaded %1").arg(fileName));
    });
    connect(d->playah, &PlayahCore::trackPositionChanged,
            this,      [=](qint64 position){
        ui->seekBar->setValue(position);
        qDebug() << d->playah->getPositionAsTime() << d->playah->getDurationAsTime();
        ui->timeLabel->setText(
                    QString("%1 / %2")
                    .arg(d->playah->getPositionAsTime().toString("mm:ss"))
                    .arg(d->playah->getDurationAsTime().toString("mm:ss"))
                    );
    });

    connect(ui->playButton, &QToolButton::clicked,
            this,           [=]{
        d->playah->play();
    });
    connect(ui->pauseButton,&QToolButton::clicked,
            this,           [=]{
        d->playah->pause();
    });
    connect(ui->stopButton, &QToolButton::clicked,
            this,           [=]{
        d->playah->stop();
    });

    connect(ui->seekBar, &QSlider::sliderMoved,
            this,        [=](int position){
        d->playah->seekTo(position);
    });

    connect(ui->actionAbout_Qt, &QAction::triggered,
            this,               &QApplication::aboutQt);

    ui->playlistView->setModel(d->playah->getPlaylist());
    ui->playlistView->setColumnWidth(0, 400);
    ui->playlistView->setColumnWidth(1, 200);
    ui->playlistView->setColumnWidth(2, 400);

    connect(ui->playlistView->model(), &PlayahPlaylistModel::rowsInserted,
            this,                      [=](const QModelIndex &, int, int last){
        ui->playlistItemCount->setText(tr("%n item(s)", "", ++last));
        ui->playlistTotalLength->setText(tr("Total: %1")
                                         .arg(d->playah->playlistDurationAsTime()
                                              .toString("hh:mm:ss")
                                              )
                                         );
    });

    connect(ui->playlistView, &PlaylistView::openFileDialog,
            this,             [=]{
        on_actionOpen_triggered();
    });

    connect(ui->playlistView, &PlaylistView::setDeletedItem,
            this,             [=](QModelIndex item){
        int rowId = item.row();
        if (d->playah->getFileName() ==
                item.siblingAtColumn(PlayahPlaylistModel::FileName).data()){
            d->playah->stop();
            disableControls();
        }
        d->playah->getPlaylist()->removeEntryNumber(rowId);
//        if (item.siblingAtColumn(2).data())
    });

    connect(ui->playlistView, &PlaylistView::selectedPlaylistItem,
            this,             [=](QModelIndex item){
        d->playah->stop();
        d->playah->loadPlaylistItemNumber(item.row());

        //qDebug() << item.siblingAtColumn(PlayahPlaylistModel::Title).data();
        //qDebug() << item.siblingAtColumn(PlayahPlaylistModel::Author).data();

        ui->titleLabel->setText(d->playah->getTitle());
        ui->authorLabel->setText(d->playah->getAuthor());

        enableControls();
        d->playah->play();
    });
    disableControls();
}

MainWindow::~MainWindow()
{
    delete d;
    delete ui;
}


void MainWindow::on_actionAbout_triggered()
{
    AboutWindow a;
    a.exec();
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog dialog(this);
    dialog.setNameFilters({tr("Audio files (*.mp3 *.wav *.ogg *.flac)"), tr("Tracker modules (*.xm *.it *.mod *.s3m)")});
    dialog.setDirectory(QDir::homePath());
    if (dialog.exec()){
        QString selectedFile = dialog.selectedFiles().first();
        if (QFile::exists(selectedFile)){
            loadSong(selectedFile);

            // automatically select last item
            ui->playlistView->setCurrentIndex(
                        ui->playlistView->model()->index(
                            ui->playlistView->model()->rowCount()-1,
                            0)
                        );

            d->playah->play();
            qDebug() << "OK!";
        } else {
            QMessageBox errorDialog(this);
            errorDialog.setWindowTitle(tr("File not found!"));
            errorDialog.setText(tr("Somehow the file doesn't exist. Select another file."));
            errorDialog.setIcon(QMessageBox::Critical);
            errorDialog.exec();
            disableControls();
        }
        // OK
    }
}

void MainWindow::disableControls()
{
    ui->playButton->setDisabled(true);
    ui->pauseButton->setDisabled(true);
    ui->stopButton->setDisabled(true);
    ui->seekBar->setDisabled(true);
}

void MainWindow::enableControls()
{
    ui->playButton->setDisabled(false);
    ui->pauseButton->setDisabled(false);
    ui->stopButton->setDisabled(false);
    ui->seekBar->setDisabled(false);
}

void MainWindow::loadSong(QString filename)
{
    d->playah->loadFile(filename);
    ui->titleLabel->setText(d->playah->getTitle());
    ui->authorLabel->setText(d->playah->getAuthor());
    enableControls();
}

bool MainWindow::eventFilter(QObject *target, QEvent *e)
{
    if (target == ui->frame){
        if (e->type() == QEvent::Paint){
            QRect area = ui->frame->rect();
            QPainter* p = new QPainter(ui->frame);
            p->setBrush(Qt::black);
            p->drawRect(0, 0, area.width()-1, area.height()-1);

            // album art here
            QImage img;
            img.load(":/art.jpg");

            p->drawImage(area, img, img.rect());
            // end album art

            p->end();
            return true;
        } else return false;
    } else return QMainWindow::eventFilter(target, e);
}

void MainWindow::on_addToPlaylist_clicked()
{
    QFileDialog dialog(this);
    dialog.setNameFilters({tr("Audio files (*.mp3 *.wav *.ogg *.flac)"), tr("Tracker modules (*.xm *.it *.mod *.s3m)")});
    dialog.setDirectory(QDir::homePath());
    if (dialog.exec()){
        QString selectedFile = dialog.selectedFiles().first();
        if (QFile::exists(selectedFile)){
            d->playah->addFileToPlaylist(selectedFile);
            qDebug() << "OK!";
        } else {
            QMessageBox errorDialog(this);
            errorDialog.setWindowTitle(tr("File not found!"));
            errorDialog.setText(tr("Somehow the file doesn't exist. Select another file."));
            errorDialog.setIcon(QMessageBox::Critical);
            errorDialog.exec();
            disableControls();
        }
        // OK
    }
}
