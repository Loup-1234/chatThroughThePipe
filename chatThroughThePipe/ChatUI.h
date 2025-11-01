#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "raylib.h"
#include "raygui.h"
#include "Chat.h"

#include <string>
#include <vector>
#include <mutex>

using namespace std;

struct ChatSettings {
	string address;
	string receiverPort;
	string senderPort;
	bool success = false;
};

class ChatUI
{
private:
	Chat& chatSession;

	vector<pair<string, bool>> chatHistory;
	mutex chatMutex;

	float scroll = 0;

	char inputBuffer[256] = "\0";
	int inputActive = 0;

	const float screenWidth = 800;
	const float screenHeight = 600;

	const float margin = 10;
	const float inputHeight = 42;
	const float buttonWidth = 120;

	const float inputTopY = screenHeight - margin - inputHeight;
	const float sendButtonLeftX = screenWidth - margin - buttonWidth;

	// Zone de messages
	Rectangle messagePanel =
	{
		margin,
		margin,
		screenWidth - margin * 2,
		inputTopY - margin * 2
	};

	// Zone de saisie
	Rectangle textBox =
	{
		margin,
		inputTopY,
		sendButtonLeftX - margin * 2,
		inputHeight
	};

	// Bouton d'envoi
	Rectangle sendButton =
	{
		sendButtonLeftX,
		inputTopY,
		buttonWidth,
		inputHeight
	};

public:
	ChatUI(Chat& session, const char* windowTitle);
	~ChatUI();

	void Run(); // Boucle principale

	static ChatSettings RunSettingsMenu();

private:
	void LogMessage(const string& message, bool isLocal);
	void SendAndClearInput();
	void HandleKeyboardInput();
	void CheckForIncomingMessages();
	void DrawChatPanel();
	void DrawInputArea();
};