#include <iostream>
using namespace std;

#include <Windows.h>

#include <thread>

#include <vector>


wstring tetromino[7]; //Array der Stücke

int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr; //Pointer Zum Array vom Spielfeld

//Größe der Console
int nScreenWidth = 80;
int nScreenHeight = 30;


int Rotate(int px, int py, int rotation)
{
	
	switch (rotation % 4) // Modulo 4 um Immer zwischen 0 und 3 zu sein also auch bei 6 wird es klappen
	{
	case 0: return py * 4 + px;			// 0 
	case 1: return 12 + py - (px * 4);	// 90 
	case 2: return 15 - (py * 4) - px;	// 180 
	case 3: return 3 - py + (px * 4);	// 270 
	}
	return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// Durch Anwenden der Drehung erhalten wir den Index des Elements im Raum
			int pieceIndex = Rotate(px, py, nRotation);

			// Index auf dem Spielfeld
			int fieldIndex = (nPosY + py) * nFieldWidth + (nPosX + px);


			if (nPosX + px >= 0 && nPosX + px < nFieldWidth) // Wenn figur von der Breiter her passt
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) // Wenn Figut von der Höhe her passt
				{
					if (tetromino[nTetromino][pieceIndex] == L'X' && pField[fieldIndex] != 0) //Check ob auf einem Feld oder 0
						return false; // Belegt
				}
			}
		}

	return true;
}

int main()
{

	// Die Stücke
#pragma region Assets

	// ..X...X...X...X.
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	// ..X..XX..X......
	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	// .X...XX...X.....
	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	// .....XX..XX.....
	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	// ..X..XX...X.....
	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	// .....XX...X...X.
	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	// .....XX..X...X..
	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

#pragma endregion

	pField = new unsigned char[nFieldWidth * nFieldHeight]; 

	// Rand erstellen
#pragma region SetFieldBondaries

	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			{
			if (x == 0)
				pField[y*nFieldWidth + x] = 9;
			else if (x == nFieldWidth - 1)
				pField[y*nFieldWidth + x] = 9;
			else if (y == nFieldHeight - 1)
				pField[y*nFieldWidth + x] = 9;
			else pField[y*nFieldWidth + x] = 0;

			}

#pragma endregion
#pragma region Console

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL); 
	SetConsoleActiveScreenBuffer(hConsole); //Aktive Console
	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
	BOOL ret = GetConsoleScreenBufferInfo(hConsole, &consoleScreenBufferInfo);	// Console Infos
	COORD tailleConsole = consoleScreenBufferInfo.dwSize;
	nScreenWidth = tailleConsole.X;		// ConsoleGröße Setzen
	nScreenHeight = tailleConsole.Y;

	//Clear Console
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

	DWORD dwBytesWritten = 0;

#pragma endregion

	// Game Logic Stuff
#pragma region Variables

	bool bGameOver = false;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = (nFieldWidth / 2) - 2;
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;

#pragma endregion

	while (!bGameOver)
	{
#pragma region GameTiming

		// GAME TIMING

		this_thread::sleep_for(50ms); // Game Tick
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

#pragma endregion
#pragma region Input

		// INPUT

		for (int k = 0; k < 4; k++)								// R	L	D Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0; // Check ob ein Input

#pragma endregion
#pragma region GameLogic

		// GAME LOGIC
#pragma region PieceTransform

		nCurrentX += (bKey[0] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0);
		nCurrentX -= (bKey[1] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0);
		nCurrentY += (bKey[2] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0);

		if (bKey[3]) //Drehen
		{
			nCurrentRotation += (!bRotateHold && (DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0); //Wenn geht drehen sonst nicht
			bRotateHold = true;
		}
		else
			bRotateHold = false;

#pragma endregion

		if (bForceDown)
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) // Platz nach unten
				nCurrentY++;
			else
			{
				// Blockieren
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1; //Spielfeld Aktualliesieren

				nPieceCount++;					//Eins mehr gesetz
				if (nPieceCount % 10 == 0)		// Ein Vielfaches von 10
					if (nSpeed >= 10) nSpeed--;	//Erhöt die geschwindigkeit

				// Check nach einer vollen Linie
				for (int py = 0; py < 4; py++) 
					if (nCurrentY + py < nFieldHeight - 1) //Höhe nicht überschritten ??
					{
						bool bLine = true;
						for (int x = 0; x < nFieldWidth; x++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + x]) != 0; //Wenn was Leer ist

						if (bLine)
						{
							//Zeile löschen und ersetzen
							for (int x = 1; x < nFieldWidth - 1; x++)
								pField[(nCurrentY + py) * nFieldWidth + x] = 8;

							vLines.push_back(nCurrentY + py);
						}


					}

				nScore += 25; // 25 für ne plaziertes Stück
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100; // 100 Punkte für jede Linie


				//Neues Stück
				nCurrentX = (nFieldWidth / 2) - 2; // halbe breite
				nCurrentY = 0; // Y Oben auf dem Spielfeld
				nCurrentRotation = 0; // Rotatiosns Reseten
				nCurrentPiece = rand() % 7; // Ein zufälliges stück von den 7


				// Wenn kein Stück mehr passt Game Over
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0; // Speedcounter zurück setzen
		}

#pragma endregion
#pragma region RenderOutput

		// RENDER OUTPUT	

		for (int x = 0; x < nFieldWidth; x++)			
			for (int y = 0; y < nFieldHeight; y++)		
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]]; //Werte für den Screen setzen also für das jeweilige Stück den jeweiligen Buchstaben

		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65; //Auf alle A hinzufügen so das sie ABCDEFG .. sind

		//Score malen
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 32, L"SCORE : %8d", nScore);


		if (!vLines.empty())
		{
			// Wenn eine Zeile voll ist
			this_thread::sleep_for(400ms); // Geschwindigkeit

			for (auto& v : vLines)
				for (int x = 1; x < nFieldWidth - 1; x++) // Jede Zeile auf x außer Rahmen
				{
					for (int y = v; y > 0; y--) // Jede Zeile y
						pField[y * nFieldWidth + x] = pField[(y - 1) * nFieldWidth + x]; //Jeden Wert von einem Block von dem nach oben setzen
					pField[x] = 0; //Oberste reihe null
				}

			vLines.clear();
		}

		
		WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten); // Denn Screen Array malen

#pragma endregion
	}

	// Game Over
	CloseHandle(hConsole);
	cout << "Game Over!! Score:" << nScore << endl;
	system("pause");

	return 0;
}