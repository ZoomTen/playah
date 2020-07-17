#include "discordintegration.h"

#include <QDebug>

#define APP_CLIENT_ID "732221711243673613"

DiscordIntegration::DiscordIntegration(QObject *parent) : QObject(parent)
{
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

        DiscordRichPresence presence;
        memset(&presence, 0, sizeof(presence));
        presence.state = "Idle";
        presence.instance = 1;
        Discord_UpdatePresence(&presence);
        loaded = true;
    }
}

void DiscordIntegration::updateState(QString artistName, QString songName)
{
    if (loaded) {
        // update code here
    }
}
