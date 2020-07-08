#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <playahcore.h>

#include <QFileDialog>
#include <QMessageBox>

#include "aboutwindow.h"

#include <QDebug>

struct MainWindowPrivate{
    PlayahCore* playah;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    d = new MainWindowPrivate();
    d->playah = new PlayahCore();

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

    connect(ui->playlistView, &QAbstractItemView::doubleClicked,
            this,             [=](const QModelIndex &index){
        d->playah->stop();
        loadSong(d->playah->getPlaylist()->getFileName(index));
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
