#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>
#include <iostream>
#include <mutex>

#define DEFAULT_BUFLEN 512

using namespace std;

class TCP
{
private:
	static int wsaInstanceCount;      // Compteur statique pour savoir combien d'objets TCP (Sender/Receiver) existent
	static WSADATA wsaData;           // Données d'initialisation Winsock (partagées entre toutes les instances)
	static mutex wsaMutex;            // Mutex pour protéger l'initialisation/nettoyage (gestion thread-safe)

protected:
	bool wsaInitialized;

	bool initializeWinsock();
	void cleanupWinsock();

public:
	TCP();
	~TCP();

	bool isInitialized() const { return wsaInitialized; }
};