#include "TCP.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

// Initialisation des membres statiques pour la gestion thread-safe de Winsock
int TCP::wsaInstanceCount = 0;
WSADATA TCP::wsaData;
mutex TCP::wsaMutex;

TCP::TCP() : wsaInitialized(false)
{
	wsaInitialized = initializeWinsock();
}

TCP::~TCP()
{
	cout << "[Cleanup]: TCP destructor called." << endl;
	if (wsaInitialized) cleanupWinsock();
}

bool TCP::initializeWinsock()
{
	// Verrouille le mutex pour assurer l'atomicité de la section critique
	scoped_lock<mutex> lock(wsaMutex);

	// Première instance
	if (wsaInstanceCount == 0)
	{
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (iResult != 0)
		{
			cerr << "WSAStartup failed with error: " << iResult << endl;
			return false;
		}
	}

	wsaInstanceCount++;

	return true;
}

void TCP::cleanupWinsock()
{
	// Verrouille le mutex pour assurer l'atomicité de la section critique
	scoped_lock<mutex> lock(wsaMutex);

	wsaInstanceCount--;

	if (wsaInstanceCount == 0) WSACleanup();
}