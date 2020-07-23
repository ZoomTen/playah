#pragma once

#include <QObject>
#include <discord_rpc.h>
#include <QLibrary>

typedef void (*DiscordInitialize)(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId);
typedef void (*DiscordUpdatePresence)(const DiscordRichPresence* presence);
typedef void (*DiscordShutdown)(void);

struct DiscordIntegrationPrivate;
class DiscordIntegration : public QObject
{
        Q_OBJECT
    public:
    enum PossibleStates{
        DI_Idle,
        DI_Play,
        DI_Paused,
        DI_Stopped
    };
        explicit DiscordIntegration(QObject *parent = nullptr);

    public slots:
        void updateState(QString artistName, QString songName, PossibleStates state);

    private:
        bool loaded = false;

        DiscordInitialize Discord_Initialize;
        DiscordUpdatePresence Discord_UpdatePresence;
        DiscordShutdown Discord_Shutdown;

        DiscordIntegrationPrivate* d;
};
