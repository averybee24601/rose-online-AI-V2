#include "WorldServer.h"
#include <string>
#include <functional>
#include <map>
#include <sstream>

void CWorldServer::InitializeConsoleCommands()
{
    consoleCommands["closeserver"] = [](CWorldServer* server, char*) {
        Log(MSG_CONSOLE, "Closing server...");
        server->isActive = false;
        return false;
    };
    consoleCommands["ann"] = [](CWorldServer* server, char* args) {
        char* from = strtok(args, " ");
        if (from == NULL) {
            Log(MSG_CONSOLE, "'ann' command: ann <name> <message>");
            return true;
        }
        unsigned msgoffset = strlen(from) + 1;
        return server->pakConsoleAnn(from, &args[msgoffset]);
    };
    consoleCommands["kick"] = [](CWorldServer* server, char* args) {
        char* name = strtok(args, " ");
        return server->pakConsoleKick(name);
    };
    consoleCommands["hhon"] = [](CWorldServer* server, char* args) {
        char* tmp = strtok(args, " ");
        int hhOn = (tmp == NULL) ? 0 : atoi(tmp);
        if (hhOn == 1) {
            server->doubleExpMapActivate();
            Log(MSG_CONSOLE, "HH activated");
            Log(MSG_GMACTION, "Server :: /hhOn active");
        } else {
            server->doubleExpMapDeactivate();
            server->Config.doubleExpActive = 0;
            Log(MSG_CONSOLE, "HH deactivated");
            Log(MSG_GMACTION, "Server :: /hhOn deactive");
        }
        return true;
    };
    consoleCommands["adver"] = [](CWorldServer* server, char* args) {
        char* tmp = strtok(args, " ");
        int adver = (tmp == NULL) ? 0 : atoi(tmp);
        if (adver == 1) {
            server->Config.AdvertiseActive = 1;
            server->DB->QExecute("UPDATE list_config SET advertise_active = 1");
            server->Config.AdvertiseTime = clock();
            server->Advertise();
            Log(MSG_CONSOLE, "Advertise system active");
        } else {
            server->Config.AdvertiseActive = 0;
            server->DB->QExecute("UPDATE list_config SET advertise_active = 0");
            Log(MSG_CONSOLE, "Advertise system deactivated");
        }
        return true;
    };
    consoleCommands["loadads"] = [](CWorldServer* server, char*) {
        server->LoadAdverds();
        Log(MSG_CONSOLE, "Adverds loaded");
        return true;
    };
    consoleCommands["help"] = [](CWorldServer* server, char*) {
        Log(MSG_CONSOLE, "Console Commands Available:");
        Log(MSG_CONSOLE, "'ann' command: ann <name> <message>");
        Log(MSG_CONSOLE, "'kick' command: kick <char name> kicks a player.");
        Log(MSG_CONSOLE, "'hhon' command: hhon <0/1> turns HH on or off.");
        Log(MSG_CONSOLE, "'help' command: returns this list");
        return true;
    };
    consoleCommands["mute"] = [](CWorldServer* server, char* args) {
        char* name = strtok(args, " ");
        if (name == NULL) {
            Log(MSG_CONSOLE, "'mute' command: mute <char name> <time_in_minutes>");
            return true;
        }
        char* time_str = strtok(NULL, " ");
        int time_in_minutes = (time_str == NULL) ? 5 : atoi(time_str); // Default to 5 minutes
        return server->pakConsoleMute(name, time_in_minutes);
    };
}

bool CWorldServer::handleCommand(char* buffer)
{
    if (buffer == NULL) return true;

    std::string buffer_str(buffer);
    std::istringstream iss(buffer_str);
    std::string command_str;
    iss >> command_str;

    // Convert command to lower case for case-insensitive matching
    std::transform(command_str.begin(), command_str.end(), command_str.begin(), ::tolower);

    auto it = consoleCommands.find(command_str);
    if (it != consoleCommands.end())
    {
        // The rest of the buffer is the arguments
        char* args = nullptr;
        size_t first_space = buffer_str.find(' ');
        if (first_space != std::string::npos) {
            args = buffer + first_space + 1;
        }
        
        return it->second(this, args);
    }
    else
    {
        Log(MSG_CONSOLE, "Command not handled");
        return true;
    }
}

// CONSOLE: Announcment
bool CWorldServer::pakConsoleAnn( char* from, char* message )
{
	BEGINPACKET( pak, 0x702 );
	ADDSTRING( pak, from );
	ADDSTRING( pak, "> " );
	ADDSTRING( pak, message );
	ADDBYTE( pak, 0x00);
	SendToAll( &pak );
	Log( MSG_CONSOLE, "Announcment sent" );
	return true;
}

bool CWorldServer::pakConsoleKick(char* name)
{
    Log( MSG_GMACTION, " Server : /kick %s" , name);
    CPlayer* otherclient = GetClientByCharName( name );
    if(otherclient==NULL)
    {
        Log( MSG_CONSOLE, "Player '%s' not found.", name );
        return true;
    }
    BEGINPACKET( pak, 0x702 );
    ADDSTRING( pak, "You were disconnected from the server !" );
    ADDBYTE( pak, 0 );
    otherclient->client->SendPacket( &pak );

    RESETPACKET( pak, 0x707 );
    ADDWORD( pak, 0 );
    otherclient->client->SendPacket( &pak );

    otherclient->client->isActive = false;

    return true;
}

bool CWorldServer::pakConsoleMute(char* name, int time_in_minutes)
{
    CPlayer* otherclient = GetClientByCharName(name);
    if (otherclient == NULL)
    {
        Log(MSG_CONSOLE, "Player '%s' not found.", name);
        return true;
    }

    otherclient->is_muted = true;
    otherclient->mute_timer = time(NULL) + time_in_minutes * 60;

    Log(MSG_CONSOLE, "Player '%s' has been muted for %d minutes.", name, time_in_minutes);
    SendSysMsg(otherclient, "You have been muted.");

    return true;
}

bool CWorldServer::pakConsoleBan(char* name)
{
    // Placeholder for ban functionality
    Log(MSG_CONSOLE, "Ban command is not yet implemented.");
    return true;
}
