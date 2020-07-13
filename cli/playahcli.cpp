#include "playahcli.h"
#include <playahcore.h>

#include <QCoreApplication>
#include <QDebug>

#include <ncurses.h>

#define KEY_RETURN 10

struct PlayahCliPrivate{
    QCoreApplication* app;
    QSocketNotifier* keyHandler;
    QString fileName;
    int refreshTimer;

    WINDOW* window; // ncurses window

    static PlayahCore* playah;
};
PlayahCore* PlayahCliPrivate::playah = nullptr;

PlayahCli::PlayahCli(QObject *parent) : QObject(parent)
{
    d = new PlayahCliPrivate();
    d->app = QCoreApplication::instance();
    d->playah = new PlayahCore();
    d->keyHandler = new QSocketNotifier{0, QSocketNotifier::Read, this};
}

void PlayahCli::run()
{
    d->playah->loadFile(d->fileName);
    d->playah->play();

    // handle user keys using QSocketNotifier
    connect(d->keyHandler, &QSocketNotifier::activated,
            this, [=]{
        int c;
        while ((c=getch()) != ERR) handleKey(c);
    });
    d->refreshTimer = this->startTimer(5);
}

void PlayahCli::quit()
{
    // cleanup
    emit exit();
}

void PlayahCli::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != d->refreshTimer) return;
    redrawScreen();
    refresh();
}

void PlayahCli::handleKey(int key)
{
    //printw("%c", (char)(c <= 255 ? c : '?'));
    switch(key){

    case KEY_LEFT:
        d->playah->seek(-5000);
        break;

    case KEY_RIGHT:
        d->playah->seek(5000);
        break;

    case KEY_RETURN:
        // return = restart
        d->playah->stop();
        d->playah->play();
        break;

    case 'p':
    case 'P':
        d->playah->pause();
        break;

    case 'q':
    case 'Q':
        quit();
        break;

    default:
        break;

    }
}

PlayahCli::~PlayahCli(){
    delete d;
}

void PlayahCli::setWindow(WINDOW *win)
{
    d->window = win;
}

void PlayahCli::redrawScreen()
{
    int curRow = 0;

    int width = getmaxx(d->window);
    int height = getmaxy(d->window);

    // render UI
    mvprintw(curRow++, 1, "Playah! CLI v" + qApp->applicationVersion().toUtf8());
    mvprintw(curRow++, 1, "- by Zumi -");
    mvprintw(curRow++, 1, "");
    mvprintw(curRow++, 1, "[q]      = quit");
    mvprintw(curRow++, 1, "[p]      = pause");
    mvprintw(curRow++, 1, "[Return] = restart");
    mvprintw(curRow++, 1, "[left]   = seek -5 seconds");
    mvprintw(curRow++, 1, "[right]  = seek +5 seconds");
    mvprintw(curRow++, 1, "");

    // progress line
    for (int i = 0; i < width; ++i){
        mvprintw(curRow, i, "-");
    }
    qint64 w = (qint64) width * d->playah->currentProgress();
    mvprintw(curRow++, (int) w, "|");   // progress bar

    mvprintw(curRow++, 1, "");

    std::string timeIndicator = QString("%1 / %2")
            .arg(d->playah->getPositionAsTime().toString("mm:ss.zzz"))
            .arg(d->playah->getDurationAsTime().toString("mm:ss.zzz"))
            .toUtf8()
            .constData();

    mvprintw(curRow++, 0, timeIndicator.c_str());

    mvprintw(curRow++, 1, "");
    mvprintw(curRow++, 0, d->playah->getTitle().toUtf8().constData());
    mvprintw(curRow++, 0, d->playah->getAuthor().toUtf8().constData());


}

void PlayahCli::setFileToPlay(QString fileName)
{
    d->fileName = fileName;
}

void PlayahCli::aboutToQuit()
{
}
