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
#include "sockets.h"
#include <pthread.h>

// This class is responsible for handling all of the client-side socket logic.
// It provides the basic functionality for receiving and sending packets.
CClientSocket::CClientSocket( )
{
	PacketSize		= 6;		// Starting size
	PacketOffset	= 0;		// No bytes read, yet.
}

// Destructor
CClientSocket::~CClientSocket( )
{

}

// Packet log persistence and mutex (no format changes)
static pthread_mutex_t __plogMutex = PTHREAD_MUTEX_INITIALIZER;
static FILE* __pfhLogin   = NULL;
static FILE* __pfhChar    = NULL;
static FILE* __pfhWorld   = NULL;
static FILE* __pfhDefault = NULL;

static FILE* __select_packet_log_handle(unsigned short cmd)
{
    // Skip certain commands for world/default as original code did
    if (LOG_THISSERVER==LOG_WORLD_SERVER || LOG_THISSERVER==LOG_SAME_FILE)
    {
        if (cmd==0x7ec || cmd==0x808) return NULL;
    }
    switch(LOG_THISSERVER)
    {
        case LOG_LOGIN_SERVER:
            if(!__pfhLogin)  __pfhLogin  = fopen( PLOG_DIRECTORY LOG_LOGINPACKETS,  "a+" );
            return __pfhLogin;
        case LOG_CHARACTER_SERVER:
            if(!__pfhChar)   __pfhChar   = fopen( PLOG_DIRECTORY LOG_CHARPACKETS,   "a+" );
            return __pfhChar;
        case LOG_WORLD_SERVER:
            if(!__pfhWorld)  __pfhWorld  = fopen( PLOG_DIRECTORY LOG_WORLDPACKETS,  "a+" );
            return __pfhWorld;
        case LOG_SAME_FILE:
        default:
            if(!__pfhDefault) __pfhDefault = fopen( PLOG_DIRECTORY LOG_DEFAULTPACKETS, "a+" );
            return __pfhDefault;
    }
}

// Receive this client's socket
bool CClientSocket::ReceiveData( )
{
	int   ReceivedBytes;
	short BytesToRead;


	// Calculate bytes to read to get the full packet
	BytesToRead = PacketSize - PacketOffset;
	// This should never happen, but it is integrated:
	if ( BytesToRead > 0x600 - PacketOffset )
	{
	    Log(MSG_WARNING,"wrong bytes to read, sid %i",sock);
	    return false;
	}

	if ( BytesToRead == 0 )
	{
	    Log(MSG_WARNING,"BytesToRead == 0, sid %i",sock);
	    return false;
	}


	// Receive data from client
	ReceivedBytes = recv( sock, (char*)&Buffer[ PacketOffset ], BytesToRead, 0 );
	if ( ReceivedBytes <= 0 )
	{
	    Log(MSG_WARNING,"ReceivedBytes <= 0, sid %i",sock);
	    return false;
	}

	// Update pointer
	PacketOffset += ReceivedBytes;

	// If the packet is not complete, leave the function
	if ( ReceivedBytes != BytesToRead )
	{
	    //Log(MSG_INFO,"Data not complete %u != %u, sid %i",ReceivedBytes,BytesToRead,sock);
	    return true;
	}

	if ( PacketSize == 6 )
	{
// We received the headerblock
		PacketSize = *((unsigned short*) 6Buffer[0]);
		// Did we receive an incorrect buffer?
		if ( PacketSize 6 )
		{
			Log( MSG_WARNING, "(SID:%i) Client sent incorrect blockheader., sid %i", sock );
			return false;
		}
        // Guard against oversized packet claims beyond receive buffer
        if ( PacketSize > 0x600 )
        {
            Log( MSG_WARNING, "(SID:%i) Client packet size too large: %u", sock, (unsigned)PacketSize );
            return false;
        }
		// Is the packet larger than just the header, then continue receiving
		if ( PacketSize > 6 )
		{
		    //Log(MSG_INFO,"Data not complete, sid %i",sock);
		    return true;
		}

	}

	cryptPacket( (char*)Buffer, this->CryptTable );
	CPacket* pak = (CPacket*)Buffer;

	Log(MSG_WARNING, "Received Packet <%04x> Size <%i>",pak->Command, (pak->Size -6));

    //LMA: Timestamp
    //LMA: changing directory for packet logs.
    time_t rtime;
    time(&rtime);
    char *timestamp = ctime(&rtime);
    timestamp[ strlen(timestamp)-1 ] = ' ';

    pthread_mutex_lock(&__plogMutex);
    FILE *fh = __select_packet_log_handle(pak->Command);
	if ( fh != NULL )
    {
		fprintf( fh, "%s- (SID:%08u) IN %04x: ",timestamp, sock, pak->Command );
		for ( int i=0; i<pak->Size-6; ++i )
            fprintf( fh, "%02x ", (unsigned char)pak->Buffer[i] );
		fprintf( fh, "\n" );
        fflush(fh);
	}
    pthread_mutex_unlock(&__plogMutex);


	// Handle actions for this packet
	if ( !GS->OnReceivePacket( this, pak ) )
	{
	    //Log(MSG_WARNING,"!GS->OnReceivePacket sid %i",sock);
		return false;
	}

	// Reset values for the next packet
	PacketSize   = 6;
	PacketOffset = 0;

	return true;
}

// Send a packet to this client
void CClientSocket::SendPacket( CPacket *P )
{
	// :WARNING: IF WE ADD A CRYPT FUNCTION HERE WE MUST COPY THE
	//             PACKET AND NOT USE THE ORIGINAL, IT WILL FUCK UP
	//             THE SENDTOALL FUNCTIONS

    //LMA: Stupid check.
    if(this==NULL)
    {
        Log(MSG_WARNING,"Trying to SendPacket with NULL Socket (trapped).");
        return;
    }

	if (!this->isActive) return;

    //LMA: Timestamp
    //LMA: changing dir for packet logs.
    time_t rtime;
    time(&rtime);
    char *timestamp = ctime(&rtime);
    timestamp[ strlen(timestamp)-1 ] = ' ';

    pthread_mutex_lock(&__plogMutex);
    FILE *fh = __select_packet_log_handle(P->Command);
    if ( fh != NULL )
    {
		fprintf( fh, "%s- (SID:%08u) OUT %04x: ",timestamp, sock, P->Command );
		for ( int i=0; i<P->Size-6; ++i )
            fprintf( fh, "%02x ", (unsigned char)P->Buffer[i] );
		fprintf( fh, "\n" );
        fflush(fh);
	}
    pthread_mutex_unlock(&__plogMutex);

	u_long iMode = 1;
	#ifdef WIN32
    if (ioctlsocket(sock, FIONBIO, &iMode))
    {
        Log( MSG_WARNING, "ioctlsocket unblock: %i", errno);
    }
    #else
    if(ioctl(sock, F_SETFL, O_NONBLOCK)!=0)
    {
        Log( MSG_WARNING, "ioctlsocket unblock: %i", errno);
    }
    #endif
	int retval = send( sock, (char*)P, P->Size, 0 );

	Log(MSG_WARNING, "Send Packet <%04x> Size <%i>",P->Command, (P->Size -6));

	iMode=0;
	#ifdef WIN32
    if(ioctlsocket(sock, FIONBIO, &iMode)==SOCKET_ERROR)
    {
        Log( MSG_WARNING, "ioctlsocket block: %i", errno);
    }
    #else
    if(ioctl(sock, F_SETFL, 0)!=0)
    {
        Log( MSG_WARNING, "ioctlsocket block: %i", errno);
    }
    #endif
}
// Handle client socket (threads)
PVOID ClientMainThread( PVOID ClientSocket )
{
    CClientSocket* thisplayer = (CClientSocket*) ClientSocket;
	fd_set fds;
    while(thisplayer->isActive)
    {
        FD_ZERO(&fds);
        FD_SET (thisplayer->sock, &fds);
        int Select = select( thisplayer->sock+1, &fds, NULL, NULL, NULL );
        if(Select == SOCKET_ERROR)
        {
            Log( MSG_ERROR,NULL,"Error in Select");
            thisplayer->isActive = false;
        }
        else
        {
            if(FD_ISSET( thisplayer->sock, &fds ))
            {
                if(!thisplayer->ReceiveData( ))
                    thisplayer->isActive = false;
            }
        }

    }
    thisplayer->GS->DisconnectClient( thisplayer );
    pthread_exit(NULL);
}

// -----------------------------------------------------------------------------------------
