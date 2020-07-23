#include "discordintegration.h"

#include <QDebug>
#include <QTimer>

#define APP_CLIENT_ID "732221711243673613"

struct DiscordIntegrationPrivate{
    QString newState;
    QString newDetails;
    QString smallImageKey;
    QString smallImageText;
};

DiscordIntegration::DiscordIntegration(QObject *parent) : QObject(parent)
{
    d = new DiscordIntegrationPrivate();

    // linking this as any old library would not work for some reason
    // TODO: link also with discord RPC dll (Win32)
    QLibrary* lib = new QLibrary("libdiscord-rpc.so");
    if (lib->load()) {
        qDebug() << "Discord RPC library loaded";
        Discord_Initialize = (DiscordInitialize) lib->resolve("Discord_Initialize");
        Discord_UpdatePresence = (DiscordUpdatePresence) lib->resolve("Discord_UpdatePresence");

        // why aren't these displaying?
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));
        handlers.ready = [](const DiscordUser* user) {
            qDebug() << "Discord initialized for " << user->username << "#" << user->discriminator;
        };
        handlers.errored = [](int errorCode, const char* message) {
            qDebug() << "Discord failed:" << errorCode << message;
        };
        handlers.disconnected = [](int errorCode, const char* message) {
            qDebug() << "Discord disconnected:" << errorCode << message;
        };
        Discord_Initialize(APP_CLIENT_ID, &handlers, true, nullptr);

        DiscordRichPresence* presence = new DiscordRichPresence();

        QTimer* timer = new QTimer(this);
        timer->setInterval(1000);

        d->newDetails = QString("No song loaded");
        d->smallImageKey = QString("idle");
        d->smallImageText = tr("Idle");

        connect(timer, &QTimer::timeout,
                this,  [=]{
            presence->state = d->newState.toUtf8().data();
            presence->details = d->newDetails.toUtf8().data();
            presence->largeImageKey = "playah_logo";
            presence->largeImageText = "Playah!";
            presence->smallImageKey = d->smallImageKey.toUtf8().data();
            presence->smallImageText = d->smallImageText.toUtf8().data();
            presence->instance = 1;
            Discord_UpdatePresence(presence);
        });

        timer->start();
        loaded = true;
    }
}

void DiscordIntegration::updateState(QString artistName, QString songName, PossibleStates state)
{
//    QList<QString> possibleStates = {
//        tr("Idle"),
//        tr("Playing"),
//        tr("Paused"),
//        tr("Stopped")
//    };
    QList<QPair<QString,QString>> possibleStates = {
        QPair<QString,QString>("idle", tr("Idle")),
        QPair<QString,QString>("play", tr("Playing")),
        QPair<QString,QString>("pause", tr("Paused")),
        QPair<QString,QString>("stop", tr("Stopped"))
    };
    if (loaded) {
        d->smallImageKey = possibleStates[state].first;
        d->smallImageText = possibleStates[state].second;
        d->newState = QString("%1")
                        .arg(artistName);
        d->newDetails = QString("%1")
                        .arg(songName);
    }
}
