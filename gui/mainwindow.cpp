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

#include <QVariantAnimation>

struct MainWindowPrivate{
    PlayahCore* playah;

    QString  lyricsTabText = QApplication::tr("Lyrics");
    QImage   albumArt;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    d = new MainWindowPrivate();
    d->playah = PlayahCore::instance();

    d->albumArt = QImage();

    ui->frame->installEventFilter(this);

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

    connect(ui->seekLeft, &QToolButton::clicked,
            this,           [=]{
        d->playah->seek(-5000);
    });
    connect(ui->seekRight, &QToolButton::clicked,
            this,           [=]{
        d->playah->seek(5000);
    });

    connect(ui->seekBar, &QSlider::sliderMoved,
            this,        [=](int position){
        d->playah->seekTo(position);
    });

    connect(ui->actionAbout_Qt, &QAction::triggered,
            this,               &QApplication::aboutQt);

    ui->playlistView->setModel(d->playah->getPlaylist());
    ui->playlistView->setColumnWidth(0, 300);
    ui->playlistView->setColumnWidth(1, 100);
    ui->playlistView->setColumnWidth(2, 300);

    connect(ui->playlistView->model(), &PlayahPlaylistModel::rowsInserted,
            this,                      &MainWindow::updatePlaylistCount);

    connect(ui->playlistView->model(), &PlayahPlaylistModel::rowsRemoved,
            this,                      &MainWindow::updatePlaylistCount);

    connect(ui->playlistView, &PlaylistView::openFileDialog,
            this,             [=]{
        on_addToPlaylist_clicked();
    });

    connect(ui->playlistView, &PlaylistView::setDeletedItem,
            this,             [=](QModelIndex item){
        int rowId = item.row();
//        if (d->playah->getPlaylistItem()){
//            // if there's an item currently playing
//            if (d->playah->getFileName() ==
//                    item.siblingAtColumn(PlayahPlaylistModel::FileName).data()){
//                d->playah->stop();
//                disableControls();
//            }
//        }
        d->playah->getPlaylist()->removeEntryNumber(rowId);
        //if (item.siblingAtColumn(2).data())
    });

    connect(ui->playlistView, &PlaylistView::selectedPlaylistItem,
            this,             [=](QModelIndex item){
        // on playlist double click
        d->playah->stop();
        d->playah->loadPlaylistItemNumber(item.row());

        //qDebug() << item.siblingAtColumn(PlayahPlaylistModel::Title).data();
        //qDebug() << item.siblingAtColumn(PlayahPlaylistModel::Author).data();

        checkPlayabilityImmediately();
    });

    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->lyricsTab));

    ui->frame->setFixedWidth(0);

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
            qDebug() << "Loaded!";
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
    checkPlayabilityImmediately();
}

void MainWindow::checkPlayabilityImmediately()
{
    // audioAvailable bounces
    QTimer::singleShot(500, this, [=]{
        if (d->playah->isPlayable()){
            updateSongInfo();
            QString lyrics = d->playah->getPlaylistItem()->getUnsyncLyrics();
            if (!lyrics.isEmpty()){
                if (ui->tabWidget->indexOf(ui->lyricsTab) == -1){
                    ui->tabWidget->insertTab(2, ui->lyricsTab, d->lyricsTabText);
                    ui->lyricsArea->setPlainText(lyrics);
                }
            } else {
                if (ui->tabWidget->indexOf(ui->lyricsTab) != -1)
                    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->lyricsTab));
            }
            enableControls();
            d->playah->play();
        } else {
            QMessageBox errorDialog(this);
            errorDialog.setWindowTitle(tr("Not playable!"));
            errorDialog.setText(tr("The file you selected can't be played, maybe the codec for it is missing. Try another file."));
            errorDialog.setIcon(QMessageBox::Critical);
            errorDialog.exec();
            disableControls();
        }
    });
}

bool MainWindow::eventFilter(QObject *target, QEvent *e)
{
    if (target == ui->frame){
        if (e->type() == QEvent::Paint){
            QRect area = ui->frame->rect();
            QPainter* p = new QPainter(ui->frame);
            p->setBrush(Qt::transparent);

            if (!d->albumArt.isNull())
                p->drawImage(QRect(0, 0, 64, 64),
                             d->albumArt,
                             d->albumArt.rect());

            p->drawRect(0, 0, area.width()-1, area.height()-1);

            p->end();
            return true;
        } else return false;
    } else return QMainWindow::eventFilter(target, e);
}

void MainWindow::updateSongInfo()
{
    bool noPreviousImage = (d->albumArt.isNull());

    PlayahPlaylistItem* currentItem = d->playah->getPlaylistItem();

    ui->titleLabel->setText(currentItem->getTitle());
    ui->titleInfo->setText(currentItem->getTitle());

    ui->authorLabel->setText(currentItem->getAuthor());
    ui->artistInfo->setText(currentItem->getAuthor());

    ui->albumLabel->setText(currentItem->getAlbum());
    ui->albumInfo->setText(currentItem->getAlbum());

    ui->bpmInfo->setText(currentItem->getBPMString());
    ui->yearInfo->setText(QString::number(currentItem->getYear()));

    if (noPreviousImage){
        d->albumArt = currentItem->getAlbumArt() ?
                      currentItem->getAlbumArt()->scaled(64,64,Qt::IgnoreAspectRatio,Qt::SmoothTransformation) :
                      QImage();

        if (!d->albumArt.isNull()){
            QVariantAnimation* anim = new QVariantAnimation(this);
            anim->setDuration(1000);
            anim->setStartValue(0);
            anim->setEndValue(64); // thumbnail size
            anim->setEasingCurve(QEasingCurve::InOutExpo);
            connect(anim, &QVariantAnimation::valueChanged,
                    this,  [=](const QVariant &value){
                ui->frame->setFixedWidth(value.toInt());
                this->update();
            });
            anim->start();
        }
    } else {
        if (currentItem->getAlbumArt() == nullptr){
            QVariantAnimation* anim = new QVariantAnimation(this);
            anim->setDuration(1000);
            anim->setStartValue(64); // thumbnail size
            anim->setEndValue(0);
            anim->setEasingCurve(QEasingCurve::InOutExpo);
            connect(anim, &QVariantAnimation::valueChanged,
                    this,  [=](const QVariant &value){
                ui->frame->setFixedWidth(value.toInt());
                this->update();
            });
            connect(anim, &QVariantAnimation::finished,
                    this, [=]{
                d->albumArt = QImage();
            });
            anim->start();
        } else {
            d->albumArt = currentItem->getAlbumArt()->scaled(64,64,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            this->update();
        }
    }
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

void MainWindow::updatePlaylistCount(const QModelIndex &, int, int)
{
    ui->playlistItemCount->setText(tr("%n item(s)", "", d->playah->getPlaylist()->itemCount()));
    ui->playlistTotalLength->setText(tr("Total: %1")
                                     .arg(d->playah->playlistDurationAsTime()
                                          .toString("hh:mm:ss")
                                          )
                                     );
}
