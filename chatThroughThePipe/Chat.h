#pragma once

#include <thread>
#include <memory>
#include <string>
#include <iostream>

using namespace std;

class Sender;
class Receiver;

class Chat
{
private:
	unique_ptr<Sender> sender;
	unique_ptr<Receiver> receiver;
	thread receiverThread;

	string address;
	string receiverPort;
	string senderPort;

	// Fonction statique pour lancer le récepteur dans un thread
	static void executeReceiver(Receiver* rec, const char* port);

public:
	Chat(const string& address, const string& receiverP, const string& senderP);
	~Chat();

	bool startSession();
	bool sendMessage(const string& message);
	string getReceivedMessage();
	void shutdown();
};