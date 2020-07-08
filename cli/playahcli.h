#ifndef PLAYAHCLI_H
#define PLAYAHCLI_H

#include <QObject>

typedef struct _win_st WINDOW;
struct PlayahCliPrivate;
class PlayahCli : public QObject
{
    Q_OBJECT
public:
    explicit PlayahCli(QObject *parent = nullptr);
    ~PlayahCli();

    void setWindow(WINDOW* win);
    void redrawScreen();
    void setFileToPlay(QString fileName);

public slots:
    void aboutToQuit();
    void run();

signals:
    void exit();

private:
    PlayahCliPrivate* d;
    void quit();
    void timerEvent(QTimerEvent* e);
    void handleKey(int key);
};

#endif // PLAYAHCLI_H
