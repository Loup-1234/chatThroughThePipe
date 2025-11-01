#include "Chat.h"
#include "ChatUI.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
	string address;
	string receiverPort;
	string senderPort;

	if (argc == 4)
	{
		address = argv[1];
		receiverPort = argv[2];
		senderPort = argv[3];

		cout << "Settings loaded from command line arguments." << endl;
	}
	else
	{
		cout << "No command line arguments detected. Opening settings menu..." << endl;

		ChatSettings settings = ChatUI::RunSettingsMenu();

		if (!settings.success)
		{
			cerr << "Settings canceled. Shutting down." << endl;
			return 1;
		}

		address = settings.address;
		receiverPort = settings.receiverPort;
		senderPort = settings.senderPort;
	}

	// 1. Initialisation de la session de chat

	Chat chat(address, receiverPort, senderPort);

	if (!chat.startSession())
	{
		cerr << "Failed to start chat session. Shutting down." << endl;
		return 1;
	}

	cout << "Chat session started." << endl;

	// 2. Initialisation et lancement de l'interface utilisateur

	try {
		ChatUI chatUI(chat, "chatThroughThePipe");
		chatUI.Run();
	}
	catch (const exception& e) {
		cerr << "UI Error: " << e.what() << endl;
	}

	cout << "UI closed. Shutting down chat session..." << endl;

	chat.shutdown();

	cout << "Chat session ended." << endl;

	return 0;
}