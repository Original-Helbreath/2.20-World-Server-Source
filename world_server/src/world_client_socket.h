#pragma once

// A game client's connection as the legacy game logic sees it (WORLD-005): the few XSocket calls the game makes on
// CClient::m_pXSock, over the shared hb::net connection. The implementation (client_gate.cpp) carries each legacy
// message as one client_game_message frame, without the old cipher; the key argument is accepted and ignored.
// No Asio, Windows or Schemata types here, so the legacy translation units can include it.
class world_client_socket
{
public:
	virtual ~world_client_socket() = default;

	// Queues one legacy message. Returns dwSize when queued, or a negative DEF_XSOCKEVENT_* code (SOCKETCLOSED,
	// MSGSIZETOOLARGE) as XSocket did.
	virtual int iSendMsg(char * cData, unsigned long dwSize, char cKey = 0) = 0;

	// Writes the peer's address (at most 15 characters, as XSocket's IPv4 text was) and a terminating zero.
	virtual int iGetPeerAddress(char * pAddrString) = 0;
};
