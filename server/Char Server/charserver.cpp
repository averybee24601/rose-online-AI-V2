/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.dev-osrose.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    depeloped with Main erose/hrose source server + some change from the original eich source
*/
#include "charserver.h"

// Constructor
CCharServer::CCharServer( string fn )
{
    LoadConfigurations( (char*)fn.c_str() );
    GServer = this;
    GServer->console_started=false;

    //LMA: chatroom code.
    chatroom_list.clear();
    last_chatroom_id=0;
}

// Deconstructor
CCharServer::~CCharServer( )
{
}

// Return a new clientsocket structure
CCharClient* CCharServer::CreateClientSocket( )
{
	CCharClient* thisclient = new (nothrow) CCharClient( );
	return thisclient;
}

//LMA BEGIN
//20070623, 224500
bool CCharServer::Ping()
{
         //Bogus request (checking if npc 1 exists).
        if(DB->QStore( "SELECT id FROM list_npcs WHERE id='1'")==NULL)
        {
             Log( MSG_INFO, "MySQL Ping Time Error on port %u",DB->Port);
			 DB->QFree( );
        }
        else
        {
            Log( MSG_INFO, "MySQL Ping Time Ok on port %u",DB->Port);
            DB->QFree( );
        }

     return true;
}
//LMA END

// When Server is ready
bool CCharServer::OnServerReady( )
{
    MYSQL_RES *result;
    MYSQL_ROW row;

    pthread_mutex_lock( &DB->SQLMutex );
    result = DB->QStore( "SELECT id FROM list_npcs WHERE id='1'");
    if(result==NULL)
    {
        Log( MSG_FATALERROR, "Your database seems to be empty, please check your sql files." );
        pthread_mutex_unlock( &DB->SQLMutex );
        return false;
    }
    mysql_free_result( result );
    pthread_mutex_unlock( &DB->SQLMutex );
	// --

	//LMA: to add world servers to the list of channels (for clan chats and so on)
	pthread_mutex_lock( &DB->SQLMutex );
	result = DB->QStore( "SELECT host,port,lanip FROM channels WHERE id=%u and type=0", Config.ParentID );
	if(!result)
	{
		Log( MSG_FATALERROR, "Can't connect to login server." );
		pthread_mutex_unlock( &DB->SQLMutex );
		return false;
	}
	if (mysql_num_rows( result ) == 1)
	{
    	row = mysql_fetch_row( result );
    	switch(Config.Connection)
    	{
            case 0://wanip
               	Config.LoginIP = row[0];
            break;
            case 1://lanip
               	Config.LoginIP = row[2];
            break;
            default://localhost
               	Config.LoginIP = (char*) "127.0.0.1";
            break;
        }
    	Config.LoginPort =  atoi(row[1]);
        pflag = true;
    }
    DB->QFree( );
    if(pflag)
    {
        Log(  MSG_INFO, "Initialized loginserver connection" );
    	// Connect To LoginServer
    	lsock = socket( AF_INET, SOCK_STREAM, 0 );
    	if (lsock == INVALID_SOCKET)
        {
            Log( MSG_WARNING, "Could not access login server" );
        }
    	struct sockaddr_in ain;
    	ain.sin_family		= AF_INET;
        ain.sin_addr.s_addr	= inet_addr( Config.LoginIP );
    	ain.sin_port		= htons( Config.LoginPort );
       	memset(&(ain.sin_zero), '\0', 8);
    	if ( connect( lsock, (SOCKADDR*) &ain, sizeof(ain) ) == SOCKET_ERROR )
    		Log( MSG_WARNING, "Could not access login server" );


    	BEGINPACKET( pak, 0x500 );
        ADDDWORD   ( pak, Config.LoginPass );
        ADDDWORD   ( pak, Config.ServerID );
        ADDWORD    ( pak, Config.CharPort );
    	cryptPacket( (char*)&pak, NULL );
    	send( lsock, (char*)&pak, pak.Size, 0 );
    }
	// Load all our clans
	pthread_mutex_lock( &DB->SQLMutex );
	result = DB->QStore("SELECT id,logo,back,name,cp,grade,slogan,news,rankingpoints,siglogo FROM list_clan");
	if(result)
	{
	while(row = mysql_fetch_row(result))
    {
		CClans* thisclan = new CClans;
        assert(thisclan);
		thisclan->id = atoi(row[0]);
		thisclan->logo = atoi(row[1]);
		thisclan->back = atoi(row[2]);
		strcpy(thisclan->name,row[3]);
		thisclan->cp = atoi(row[4]);
		thisclan->grade = atoi(row[5]);
        strcpy(thisclan->slogan,row[6]);
        strcpy(thisclan->news,row[7]);
        thisclan->rankingpoints=atoi(row[8]);
        thisclan->siglogo=atoi(row[9]);
		ClanList.push_back( std::unique_ptr<CClans>(thisclan) );
	}
    mysql_free_result( result );
    pthread_mutex_unlock( &DB->SQLMutex );
	// --
    // Load Clans
	for(UINT i=0;i<ClanList.size( );i++)
	{
        CClans*	Clan = ClanList.at( i ).get();
        pthread_mutex_lock( &DB->SQLMutex );
        result = DB->QStore("SELECT id,char_name,clan_rank FROM characters WHERE clanid=%i order by clan_rank",Clan->id);
        if(result)
        {
	    while(row = mysql_fetch_row(result))
        {
            CClanMembers* newmember = new CClanMembers;
            assert(newmember);
            newmember->id = atoi(row[0]);
            strcpy(newmember->name,row[1]);
            newmember->clan_rank = atoi(row[2]);
            Clan->ClanMembers.push_back( newmember );
        }
        mysql_free_result( result );
        pthread_mutex_unlock( &DB->SQLMutex );
    }

	//Load items for new chars
	pthread_mutex_lock( &DB->SQLMutex );
    result = DB->QStore("SELECT itemnum,itemtype,amount,slotnum,durability,sex,premium FROM items_newchar WHERE active='1'");
    if(result)
    {
	while(row = mysql_fetch_row(result))
	{
		CItemNewChar* tmp = new (nothrow) CItemNewChar();
        if(tmp)
        {
		    tmp->itemnum = atoi(row[0]);
		    tmp->itemtype = atoi(row[1]);
		    tmp->amount = atoi(row[2]);
		    tmp->durability = atoi(row[3]);
		    tmp->slotnum = atoi(row[4]);
		    tmp->sex = atoi(row[5]);
		    tmp->premium = (atoi(row[6]) != 0);
		    ItemNewChar.push_back(std::unique_ptr<CItemNewChar>(tmp));
        }
		row = mysql_fetch_row( result );
	}
    mysql_free_result( result );
    pthread_mutex_unlock( &DB->SQLMutex );
	// --

	Log( MSG_INFO, "Clans Information Loaded" );
    return true;
}

//LMA: disconnect a client from chatroom.
 bool CCharServer::DisconnectClientFromChat( CCharClient* thisclientwc )
 {
        //LMA: resetting chatroom ID.
        if(thisclientwc->chatroom_id!=0)
        {
            auto it = chatroom_list.find(thisclientwc->chatroom_id);
            if(it != chatroom_list.end())
            {
                CChatroom* mychatroom = it->second.get();
                if(mychatroom->master_charid==thisclientwc->charid)
                {
                    //the master has left the building...
                    //TODO: warning other people about that.
                }
                else
                {
                    //Warning other people someone has left.
                    BEGINPACKET( pak, 0x7e3 );
                    ADDBYTE    ( pak, 0x05 );
                    ADDDWORD   ( pak, thisclientwc->chatroom_id );
                    ADDSTRING  ( pak, thisclientwc->charname.c_str() );
                    SendToChatroom(thisclientwc->chatroom_id,&pak,thisclientwc->charid);
                }
            }
        }

        for (size_t k=0;k<thisclientwc->FriendList.size();k++)
    	{
            CFriendList* Friend = thisclientwc->FriendList.at( k );;
            CCharClient* otherclient = GetClientByID( Friend->id );
            if(otherclient!=NULL)
            {
                ChangeMessengerStatus ( thisclientwc, otherclient, 0x08);
            }
    	}
        //set logout messga to clan
        CClans* thisclan = GetClanByID(thisclientwc->clanid);
        if(thisclan==NULL)
            return;
        for(UINT i=0;i<thisclan->ClanMembers.size( );i++)
    	{
            CClanMembers*   ClanMember = thisclan->ClanMembers.at( i );
            CCharClient* otherclient = GetClientByID( ClanMember->id );
            if(otherclient!=NULL)
                ChangeClanStatus (thisclientwc, otherclient, 0xff);
    	}

    	//LMA: Disconnecting a client from chatroom.
    	DisconnectClientFromChat(thisclientwc );
    }
}

// Deletes an old clientsocket structure
void CCharServer::DeleteClientSocket( CClientSocket* thisclient )
{
    CCharClient* client = (CCharClient*)thisclient;
    //LMA: no more crash with this.
    if(client==NULL) return;
    if(client->accesslevel==0xffff) //is a channel
    {
        for(UINT i=0;i<ChannelList.size();i++)
        {
            CChanels* thischannel = ChannelList.at( i ).get();
            if(thischannel)
            {
                if(thischannel->sock==client->sock)
                {
                    ChannelList.erase( ChannelList.begin()+i );
                    Log(MSG_INFO,"Channel %i disconnected",thischannel->id);
                }
            }
        }
    }
    else
    {
    	Log( MSG_INFO, "User disconnected CCharServer::DeleteClientSocket %s",client->charname.c_str());
    }
	delete (CCharClient*)thisclient;
}

/**
	* check on the database if a client is banned
	* @param ClientInfo sockaddr_in structure
	* @return true if the client is banned else false
*/
bool CCharServer::isBanned( sockaddr_in* ClientInfo )
{
	// Check ban list
	pthread_mutex_lock( &DB->SQLMutex );
	MYSQL_RES* result = DB->QStore( "SELECT id, ip, startban, bantime FROM ban_list" );
	if(!result)
	{
        pthread_mutex_unlock( &DB->SQLMutex );
		return false;
	}
	MYSQL_ROW row = mysql_fetch_row( result );
	while(row=mysql_fetch_row( result ))
	{
		unsigned int strsize = strlen( (char*)row[1] );
		if(strncmp( row[1], (char*)ip.c_str( ), strsize)==0)
		{	// ip match is banned, now check if is permant ban or temporal
			if(row[3]==NULL || atoi(row[3])==0)
			{ // is permant ban
				DB->QFree( );
				return true;
			}
			unsigned long int finishban = atol(row[2]) + atol(row[3]);
			if(finishban>=GetServerTime( ))
			{
				unsigned int id = atoi(row[0]);
				DB->QFree( );
				DB->QExecute( "DELETE FROM ban_list WHERE id=%u", id );
				return false;
			}
			else
			{
				DB->QFree( );
				return true;
			}
		}
	}
	mysql_free_result(result);
    pthread_mutex_unlock( &DB->SQLMutex );
	return false;
}

// Load Server configuration
void CCharServer::LoadConfigurations( char* file )
{
    //Database
	Config.SQLServer.pcServer   = ConfigGetString ( file, "mysql_host", "localhost" );
	Config.SQLServer.pcDatabase = ConfigGetString ( file, "mysql_database", "fate" );
    Config.SQLServer.pcUserName = ConfigGetString ( file, "mysql_user", "root" );
	Config.SQLServer.pcPassword = ConfigGetString ( file, "mysql_pass", "0urf4t3" );
	Config.SQLServer.pcPort     = ConfigGetInt    ( file, "mysql_port", 3306 );
    //Server
    Config.ServerID             = ConfigGetInt    ( file, "serverid", 1 );
    Config.ServerType           = ConfigGetInt    ( file, "servertype", 1 );
    Config.CharIP               = ConfigGetString ( file, "serverip", "209.141.43.6" );
	Config.CharPort             = ConfigGetInt    ( file, "serverport", 29100 );
	Config.ParentID             = ConfigGetInt    ( file, "parentid", 0 );
	Config.ServerName           = ConfigGetString ( file, "servername", "FableRose" );
    Config.Connection           = ConfigGetInt    ( file, "connection", 0 );
    Config.LanIP                = ConfigGetString ( file, "lanip", "192.168.1.20" );
    Config.LanSubnet            = ConfigGetString ( file, "lansubmask", "255.255.255.0" );
	//Char
	Config.usethreads           = ConfigGetInt    ( file, "usethreads", 0 )==0?false:true;
	Config.DeleteTime           = ConfigGetInt    ( file, "deletetime", 36000 );
    Config.MinimumAccessLevel   = ConfigGetInt    ( file, "accesslevel", 100 );
	//Password
	Config.LoginPass            = ConfigGetInt    ( file, "loginpass", 123456 );
	Config.CharPass             = ConfigGetInt    ( file, "charpass", 123456 );
	Config.WorldPass            = ConfigGetInt    ( file, "worldpass", 123456 );
}

// Incoming packet
bool CCharServer::OnReceivePacket( CClientSocket* thisclient, CPacket *P )
{
    switch( P->Command )
	{
    	case 0x0500: return pakWSReady          ( (CCharClient*)thisclient, P );
    	case 0x0501: return pakLoginConnected   ( (CCharClient*)thisclient, P );
    	case 0x0502: return pakLoginDSClient    ( (CCharClient*)thisclient, P );
    	case 0x0505: return pakWSCharSelect     ( (CCharClient*)thisclient, P );

    	case 0x070b: return pakDoIdentify       ( (CCharClient*)thisclient, P );

    	case 0x0712: return pakGetCharacters    ( (CCharClient*)thisclient, P );
    	case 0x0713: return pakCreateChar       ( (CCharClient*)thisclient, P );
    	case 0x0714: return pakDeleteChar       ( (CCharClient*)thisclient, P );
    	case 0x0715: return pakRequestWorld     ( (CCharClient*)thisclient, P );

		case 0x0787: return pakClanChat         ( (CCharClient*)thisclient, P );

        case 0x079e: return pakUpdateLevel      ( (CCharClient*)thisclient, P );

		case 0x07e0: return pakClanManager      ( (CCharClient*)thisclient, P );
        case 0x07e1: return pakMessengerManager ( (CCharClient*)thisclient, P );
        case 0x07e2: return pakMessengerChat    ( (CCharClient*)thisclient, P );
        case 0x07e3: return pakChatrooms        ( (CCharClient*)thisclient, P );
        case 0x07e4: return pakTalkChatroom     ( (CCharClient*)thisclient, P );
        case 0x07e5: return pak7e5              ( (CCharClient*)thisclient, P );
        case 0x07e6: return pakUploadCM         ( (CCharClient*)thisclient, P );
        case 0x07e7: return pakDownloadCM       ( (CCharClient*)thisclient, P );
        case 0x07e8: return pakClanIconTime     ( (CCharClient*)thisclient, P );

        case 0x0808: return true; //LMA: GG Packet here too.
     	default:
     	Log( MSG_WARNING, "Unknown Packet <%04x> Size <%i>", P->Command, P->Size-6 );
		break;
	}
	return true;
}
