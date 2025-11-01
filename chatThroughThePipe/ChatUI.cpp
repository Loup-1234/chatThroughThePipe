#define RAYGUI_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include "ChatUI.h"

ChatUI::ChatUI(Chat& session, const char* windowTitle) : chatSession(session)
{
	InitWindow((int)screenWidth, (int)screenHeight, windowTitle);
	SetTargetFPS(60);

	GuiLoadStyle("style_cyber.rgs");
}

ChatUI::~ChatUI()
{
	cout << "[Cleanup]: ChatUI destructor called." << endl;
	CloseWindow();
}

// Boucle principale
void ChatUI::Run()
{
	while (!WindowShouldClose())
	{
		CheckForIncomingMessages();
		HandleKeyboardInput();

		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		DrawChatPanel();
		DrawInputArea();

		EndDrawing();
	}
}

// Menu de réglages avant le démarrage du chat
ChatSettings ChatUI::RunSettingsMenu()
{
	const int menuWidth = 250;
	const int menuHeight = 170;

	ChatSettings settings;

	InitWindow(menuWidth, menuHeight, "Chat Settings");
	SetTargetFPS(60);

	// Charger le style (important de le faire aussi ici)
	GuiLoadStyle("style_cyber.rgs");

	// Buffers pour les champs de texte
	char addressBuffer[128] = "127.0.0.1";
	char receiverPortBuffer[16] = "27015";
	char senderPortBuffer[16] = "27016";

	// États d'édition pour GuiTextBox
	int addressActive = 0;
	int receiverActive = 0;
	int senderActive = 0;

	// Définition des rectangles pour l'UI
	const float padding = 10;
	const float height = 30;
	const float width = (menuWidth - padding * 2) / 2;
	const float itemSpacing = 10;

	float currentY = padding;
	Rectangle labelIp = { padding, currentY, width, height };
	Rectangle textIp = { padding + width, currentY, width, height };

	currentY += height + itemSpacing;
	Rectangle labelRx = { padding, currentY, width, height };
	Rectangle textRx = { padding + width, currentY, width, height };

	currentY += height + itemSpacing;
	Rectangle labelTx = { padding, currentY, width, height };
	Rectangle textTx = { padding + width, currentY, width, height };

	currentY += height + itemSpacing;
	Rectangle buttonStart = { padding, currentY, menuWidth - padding * 2, height };

	bool startClicked = false;

	while (!startClicked && !WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		// Labels
		GuiLabel(labelIp, "IP Address:");
		GuiLabel(labelRx, "Receiver Port:");
		GuiLabel(labelTx, "Sender Port:");

		// Champs de texte
		if (GuiTextBox(textIp, addressBuffer, 128, addressActive)) addressActive = ~addressActive;

		if (GuiTextBox(textRx, receiverPortBuffer, 16, receiverActive)) receiverActive = ~receiverActive;

		if (GuiTextBox(textTx, senderPortBuffer, 16, senderActive)) senderActive = ~senderActive;

		// Bouton de démarrage
		if (GuiButton(buttonStart, "START CHAT")
			&& strlen(addressBuffer) > 0
			&& strlen(receiverPortBuffer) > 0
			&& strlen(senderPortBuffer) > 0)
		{
			startClicked = true;

			settings.address = addressBuffer;
			settings.receiverPort = receiverPortBuffer;
			settings.senderPort = senderPortBuffer;
			settings.success = true;
		}

		EndDrawing();
	}

	CloseWindow();

	return settings;
}

// Enregistre un message dans l'historique
void ChatUI::LogMessage(const string& message, bool isLocal)
{
	scoped_lock<mutex> lock(chatMutex);
	chatHistory.emplace_back(message, isLocal);
	scroll = 0; // Réinitialise le scroll vers le bas
}

// Vérifie et traite les messages entrants
void ChatUI::CheckForIncomingMessages()
{
	string message = chatSession.getReceivedMessage();

	// Boucle sur tous les messages reçus
	while (!message.empty())
	{
		LogMessage(message, false);
		message = chatSession.getReceivedMessage();
	}
}

// Envoie le message et vide le champ de saisie
void ChatUI::SendAndClearInput()
{
	if (strlen(inputBuffer) == 0)
		return;

	string messageToSend = inputBuffer;

	// Permet de quitter proprement avec "exit"
	if (messageToSend == "exit")
	{
		CloseWindow();
		return;
	}

	if (chatSession.sendMessage(messageToSend)) LogMessage(messageToSend, true);

	inputBuffer[0] = '\0'; // Vide le champ
}

// Gestion de l'entrée clavier
void ChatUI::HandleKeyboardInput()
{
	// Envoi rapide avec ENTER
	if (inputActive && IsKeyPressed(KEY_ENTER))
	{
		SendAndClearInput();
	}
}

// Affichage de la zone de messages
void ChatUI::DrawChatPanel()
{
	DrawRectangleLinesEx(messagePanel, 1, GetColor(GuiGetStyle(DEFAULT, BORDER)));

	const float lineHeight = 22;
	const int fontSize = 12;

	float y = messagePanel.y + messagePanel.height - scroll;

	const int maxTextWidth = (int)messagePanel.width - ((int)margin * 2);

	for (int i = (int)chatHistory.size() - 1; i >= 0; --i)
	{
		const bool isLocal = chatHistory[i].second;

		const string prefix = isLocal ? "[You]: " : "[Peer]: ";

		const Color msgColor = isLocal ? CLITERAL(Color) { 120, 255, 220, 255 } : GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));

		string msg = prefix + chatHistory[i].first;

		y -= lineHeight;

		if (y > messagePanel.y + messagePanel.height - margin) continue;

		if (y < messagePanel.y + margin) break;

		// Tronquer le texte s'il dépasse la largeur maximale
		if (MeasureText(msg.c_str(), fontSize) > maxTextWidth)
		{
			// Réduire le message char par char jusqu'à ce qu'il rentre avec "..."
			while (MeasureText(msg.c_str(), fontSize) > (maxTextWidth - MeasureText("...", fontSize))) msg.pop_back();

			msg.append("...");

			DrawText(
				msg.c_str(),
				(int)messagePanel.x + (int)margin,
				(int)y,
				fontSize,
				msgColor
			);
		}
		else
		{
			DrawText(
				msg.c_str(),
				(int)messagePanel.x + (int)margin,
				(int)y,
				fontSize,
				msgColor
			);
		}
	}

	const float contentHeight = (float)chatHistory.size() * lineHeight;

	const float maxScrollBoundary = 0;
	const float minScrollBoundary = contentHeight > messagePanel.height ? -(contentHeight - messagePanel.height) : 0;

	scroll -= GetMouseWheelMove() * 22;

	if (scroll > maxScrollBoundary) scroll = maxScrollBoundary;
	if (scroll < minScrollBoundary) scroll = minScrollBoundary;
}

// Affichage de la zone de saisie
void ChatUI::DrawInputArea()
{
	// Zone de texte
	if (GuiTextBox(textBox, inputBuffer, sizeof(inputBuffer), inputActive)) inputActive = ~inputActive;

	if (strlen(inputBuffer) == 0 && !inputActive)
	{
		const char* placeholder = "Press ENTER or click SEND to send your message.";
		DrawText(
			placeholder,
			(int)textBox.x + 8,
			(int)(textBox.y + (textBox.height / 2) - (12 / 2)), // Centré verticalement
			12,
			Fade(GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_DISABLED)), 0.7f)
		);
	}

	// Bouton d’envoi
	if (GuiButton(sendButton, "SEND"))
	{
		SendAndClearInput();
		inputActive = false;
	}
}