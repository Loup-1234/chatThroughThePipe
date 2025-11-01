#pragma once

#include "TCP.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>
#include <iostream>
#include <queue>
#include <mutex>

using namespace std;

class Receiver : public TCP
{
private:
	SOCKET ListenSocket;        // Le socket utilisé pour écouter les connexions entrantes
	SOCKET ClientSocket;        // Le socket dédié à la communication avec un client connecté

	struct addrinfo* result;    // Pointeur vers les résultats de la résolution d'adresse (getaddrinfo)
	struct addrinfo hints;      // Structure pour configurer getaddrinfo (type de socket, protocole, etc.)

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	string senderPort;

	queue<string> receivedMessages;		// File d'attente des messages reçus
	mutex messagesMutex;				// Mutex pour protéger l'initialisation/nettoyage (gestion thread-safe)

public:
	Receiver();
	~Receiver();

	bool startListening(const char* port = "27015");
	bool acceptConnection();
	bool receiveMessages();

	string getReceivedMessage();

private:
	bool resolveAddress();
	bool setupListener();

	void closeClientSocket();
	void closeListenSocket();
};