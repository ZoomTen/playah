#pragma once

#include <QObject>
#include <discord_rpc.h>
#include <QLibrary>

typedef void (*DiscordInitialize)(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId);
typedef void (*DiscordUpdatePresence)(const DiscordRichPresence* presence);
typedef void (*DiscordShutdown)(void);

class DiscordIntegration : public QObject
{
        Q_OBJECT
    public:
        explicit DiscordIntegration(QObject *parent = nullptr);

    signals:

    public slots:
        void updateState(QString artistName, QString songName);

    private:
        bool loaded = false;

        DiscordInitialize Discord_Initialize;
        DiscordUpdatePresence Discord_UpdatePresence;
        DiscordShutdown Discord_Shutdown;
};
