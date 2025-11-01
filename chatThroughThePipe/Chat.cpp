#include "Chat.h"
#include "Sender.h"
#include "Receiver.h"

#include <chrono>

Chat::Chat(const string& address, const string& receiverP, const string& senderP)
	: address(address), receiverPort(receiverP), senderPort(senderP)
{
	// Utilisation de make_unique pour la gestion automatique de la mémoire (RAII)
	// "Je ne sais comment ça fonctionne"
	sender = make_unique<Sender>();
	receiver = make_unique<Receiver>();
}

Chat::~Chat()
{
	cout << "[Cleanup]: Chat destructor called." << endl;
	shutdown();
}

// Démarre la session de chat
bool Chat::startSession()
{
	// Crée et lance le thread de réception, lui passant l'instance receiver et le port
	// Le thread commencera à écouter en arrière-plan
	receiverThread = thread(executeReceiver, receiver.get(), receiverPort.c_str());

	cout << "Thread [Sender]: Waiting for 3s for peer server to start..." << endl;
	// Pause pour donner le temps au thread récepteur du pair distant de démarrer son écoute
	this_thread::sleep_for(chrono::seconds(3));

	// Tente de se connecter au pair distant en utilisant l'adresse et le port d'envoi
	if (!sender->connectToReceiver(address.c_str(), senderPort.c_str()))
	{
		cerr << "Thread [Sender]: Cannot connect to the target peer." << endl;

		// Si la connexion échoue, on arrête et joint le thread récepteur local
		if (receiverThread.joinable()) receiverThread.join();

		return false;
	}

	return true;
}

// Envoie un message au pair distant
bool Chat::sendMessage(const string& message)
{
	if (sender->sendMessage(message.c_str()) == SOCKET_ERROR)
	{
		cerr << "[Error]: Send failed." << endl;

		return false;
	}

	return true;
}

string Chat::getReceivedMessage()
{
	return receiver->getReceivedMessage();
}

// Implémentation de la fonction statique qui sera exécutée dans le thread de réception
void Chat::executeReceiver(Receiver* rec, const char* port)
{
	cout << "Thread [Receiver]: Starting to listen on port " << port << "..." << endl;

	// Tente de démarrer l'écoute
	if (!rec->startListening(port))
	{
		cerr << "Thread [Receiver]: Failed to start listening." << endl;
		return;
	}

	// Bloque jusqu'à ce qu'un client (le pair distant) se connecte
	if (!rec->acceptConnection())
	{
		cerr << "Thread [Receiver]: Failed to accept client." << endl;
		return;
	}

	cout << "A peer is connected. Chat started." << endl;
	cout << "[You]: " << flush;

	// Boucle de réception des messages (bloquant jusqu'à la déconnexion du pair)
	rec->receiveMessages();

	cout << "Reception finished (peer disconnected)." << endl;
}

void Chat::shutdown()
{
	cout << "[Cleanup]: Forcing socket closure..." << endl;

	if (sender) sender.reset();
	if (receiver) receiver.reset();

	if (receiverThread.joinable())
	{
		cout << "[Cleanup]: Joining receiver thread..." << endl;
		receiverThread.join();
	}
}