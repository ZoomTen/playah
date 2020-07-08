#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct MainWindowPrivate;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAbout_triggered();
    void on_actionOpen_triggered();

    void on_addToPlaylist_clicked();

private:
    Ui::MainWindow *ui;
    MainWindowPrivate* d;
    void disableControls();
    void enableControls();
    void loadSong(QString filename);
};
#endif // MAINWINDOW_H