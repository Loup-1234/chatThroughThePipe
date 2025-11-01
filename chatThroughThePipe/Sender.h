#pragma once

#include "TCP.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>
#include <iostream>

using namespace std;

class Sender : public TCP
{
private:
	SOCKET ConnectSocket;		// Le socket utilisé pour la connexion au destinataire

	struct addrinfo* result;	// Pointeur vers les résultats de la résolution d'adresse (getaddrinfo)
	struct addrinfo* ptr;		// Pointeur pour itérer sur la liste des résultats d'adresse
	struct addrinfo hints;		// Structure pour configurer getaddrinfo (famille, type, protocole, etc.)

	string receiverAddress;
	string receiverPort;

public:
	Sender();
	~Sender();

	bool connectToReceiver(const char* receiverAddress, const char* port = "27016");
	int sendMessage(const char* message);

private:
	bool resolveAddress();
	bool attemptConnect();

	void closeConnectSocket();
};