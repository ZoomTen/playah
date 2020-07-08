#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTimer>
#include <QFile>

#include <ncurses.h>

#include "playahcli.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("Playah-CLI");
    QCoreApplication::setApplicationVersion("0.0.1");

    QCommandLineParser parse;

    parse.setApplicationDescription("Dumb shit player");
    parse.addPositionalArgument("file", "File to play");
    parse.addHelpOption();
    parse.addVersionOption();
    parse.process(a);

    QStringList args = parse.positionalArguments();

    QString fileToPlay;

    if (args.count() == 1){
        // file provided
        if (!QFile::exists(args[0])){
            QTextStream(stdout) << QCoreApplication::translate("main","File doesn't exist") << Qt::endl;
            exit(1);
        } else {
            fileToPlay = args[0];
        }
    } else if (args.count() > 1) {
        // only takes one argument
        parse.showHelp(1);
    } else {
        // no arguments = type in name of file
        QString inp;
        do {
            QTextStream(stdout) << QCoreApplication::translate("main", "Enter file to play (Ctrl+D = exit): ");
            QTextStream(stdin) >> inp;

            if (inp == ""){
                QTextStream(stdout) << "\n";
                exit(0);
            }

            if (!QFile::exists(inp)){
                QTextStream(stdout) << QCoreApplication::translate("main","File doesn't exist") << Qt::endl;
            }
        } while (!QFile::exists(inp));
        fileToPlay = inp;
    }

    // ncurses init
    WINDOW* win = initscr();

    PlayahCli p;
    p.setWindow(win);
    p.setFileToPlay(fileToPlay);

    QObject::connect(&p, &PlayahCli::exit,
                     &a, &QCoreApplication::quit);

    QObject::connect(&a, &QCoreApplication::aboutToQuit,
                    &p, &PlayahCli::aboutToQuit);

    QTimer::singleShot(10, &p, &PlayahCli::run);

    clear();

    // set terminal settings
    cbreak();
    noecho();
    keypad(win, true);
    nodelay(win, true);

    auto exec = a.exec();

    // ncurses done
    endwin();

    return exec;
}
