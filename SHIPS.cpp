#include <iostream>
#include <string.h>
#include <stdlib.h> 
#include <math.h>

using namespace std;

#define NORTH 'N'
#define SOUTH 'S'
#define WEST 'W'
#define EAST 'E'
#define BOARDy 21
#define BOARDx 10
#define NUMBEROFSHIPS 100

enum SHIPS {
	CAR = 5, BAT = 4, CRU = 3, DES = 2
};

struct Game {
	char winner='\0';
	char actualPlayer = 'A';
	short resetPlayersShipMoves[2][NUMBEROFSHIPS] = {};
	short resetPlayersShipShoots[2][NUMBEROFSHIPS] = {};
	bool isCustomBoard = false;
	bool exShipsON = false;
	struct Board
	{
		short y = BOARDy;
		short x = BOARDx;
	} BOARD;
}GAME;

struct Ship {
	//bool isDestroyed=false;
	char parts[5] = { 0 };
	short lenght = 0;
	short moves = 3;
	short shoots = 0;
	short type = '\0';
	short typeNumber = 0;
	short shipNumber = 0;
	short x = '\0';
	short y = '\0';
	char rotation = '\0';
};

struct Player {
	short fleet[4] = { 1,2,3,4 };
	short fleetNumber = 10;
	short initPosition[2][2] = { 0 };
	short numberOfShips = 0;
	short ships[4] = { 0 };
	short shipsNumber[4][NUMBEROFSHIPS/4] = { 0 };
	short numberOfParts = 0;
	Ship Ships[NUMBEROFSHIPS];
}A,B;

void freeMemory(char** board) //Zwalnia pamiêæ
{
	for (short i = 0; i < GAME.BOARD.y; i++)
		delete[] board[i];
	delete[] board;
}
void setBoard(char** board) //Wype³nia planszê na start
{
	for (int i = 0; i < GAME.BOARD.y; i++)
	{
		for (int j = 0; j < GAME.BOARD.x; j++)
		{
			board[i][j] = ' ';
		}
	}
}
short shipTypeLenght(short typeOfShip) //Zwraca d³ugoœæ statku na podstawie jego numeru
{
	if (typeOfShip == 0)
		return CAR;
	else if (typeOfShip == 1)
		return BAT;
	else if (typeOfShip == 2)
		return CRU;
	else if (typeOfShip == 3)
		return DES;
	return '\0';
}
short shipTypeNumber(char* shipType) //Zamienia typ statku na liczbê
{
	if (shipType[2] == 'R')
		return 0;
	else if (shipType[2] == 'T')
		return 1;
	else if (shipType[2] == 'U')
		return 2;
	else if (shipType[2] == 'S')
		return 3;
	return '\0';
}
bool checkType(short typeOfShip, char player) //Sprawdza czy dany gracz mo¿e posiadaæ kolejny statek danego typu
{
	if (player == 'A')
	{
		if (A.ships[typeOfShip] + 1 > A.fleet[typeOfShip])
			return false;
	}
	else
	{
		if (B.ships[typeOfShip] + 1 > B.fleet[typeOfShip])
			return false;
	}
	return true;
}
bool checkAmount(short typeOfShip, short number, char player) //Sprawdza czy gracz ma ju¿ statek o danym numerze
{
	if (player == 'A')
	{
		if (A.shipsNumber[typeOfShip][number] > 0)
			return false;
	}
	else
	{
		if (B.shipsNumber[typeOfShip][number] > 0)
			return false;
	}
	return true;
}
bool checkReef(short y, short x, char direction, short lenght, char** board) //Sprawdza czy nowy statek nie nachodzi na rafy
{
	if (direction == WEST)
	{
		for (short i = x; i < x + lenght; i++)
		{
			if (board[y][i] == '#')
				return false;
		}
		return true;
	}
	else if (direction == EAST)
	{
		for (short i = x; i > x - lenght; i--)
		{
			if (board[y][i] == '#')
				return false;
		}
		return true;
	}
	else if (direction == SOUTH)
	{
		for (short i = y; i > y - lenght; i--)
		{
			if (board[i][x] == '#')
				return false;
		}
		return true;
	}
	else
	{
		for (short i = y; i < y + lenght; i++)
		{
			if (board[i][x] == '#')
				return false;
		}
		return true;
	}
}
bool extendedSpaceCheck(short y, short x, char direction, short lenght, char**exBoard) //Sprawdza czy statek nie ma s¹siedztwa innego statku lub siê na nim nie znajdzie
{
	x += 1;
	y += 1;
	if (direction == WEST)
	{
		for (short i = y-1; i <= (y-1) + 2; i++)
		{
			for (short j = x-1; j <= (x-1) + lenght +1; j++)
			{
				if (exBoard[i][j] == '+')
				{
					freeMemory(exBoard);
					return false;
				}
			}
		}
		freeMemory(exBoard);
		return true;
	}
	else if (direction == EAST)
	{
		for (short i = y-1; i <= (y-1) + 2; i++)
		{
			for (short j = x + 2; j >= (x+2) - lenght - 1; j--)
			{
				if (exBoard[i][j] == '+')
				{
					freeMemory(exBoard);
					return false;
				}
			}
		}
		freeMemory(exBoard);
		return true;
	}
	else if (direction == SOUTH)
	{
		for (short i = y + 1; i >= (y+1) - lenght - 1; i--)
		{
			for (short j = x-1; j <= (x-1)+2; j++)
			{
				if (exBoard[i][j] == '+')
				{
					freeMemory(exBoard);
					return false;
				}
			}
		}
		freeMemory(exBoard);
		return true;
	}
	else if(direction == NORTH)
	{
		for (short i = y-1; i <= (y-1) + lenght + 1; i++)
		{
			for (short j = x-1; j <= (x-1)+2; j++)
			{
				if (exBoard[i][j] == '+')
				{
					freeMemory(exBoard);
					return false;
				}
			}
		}
		freeMemory(exBoard);
		return true;
	}
	freeMemory(exBoard);
	return true;
}
char** setExBoard(char** board) //Specjalnie powiêkszona plansza dla funkcji exSpaceChceck
{
	char** exBoard;
	exBoard = new char* [GAME.BOARD.y + 2];
	for (int i = 0; i < GAME.BOARD.y + 2; i++)
		exBoard[i] = new char[GAME.BOARD.x + 2];
	for (int i = 0; i < GAME.BOARD.y + 2; i += GAME.BOARD.y + 1)
	{
		for (int j = 0; j < GAME.BOARD.x + 2; j++)
		{
			exBoard[i][j] = ' ';
		}
	}
	for (int i = 0; i < GAME.BOARD.x + 2; i += GAME.BOARD.x + 1)
	{
		for (int j = 0; j < GAME.BOARD.y+2; j++)
		{
			exBoard[j][i] = ' ';
		}
	}
	for (int i = 1; i < GAME.BOARD.y + 1; i++)
	{
		for (int j = 1; j < GAME.BOARD.x + 1; j++)
		{
			exBoard[i][j] = board[i - 1][j - 1];
		}
	}
	return exBoard;
}
bool checkSpace(short y, short x, char direction, short lenght, char player) //Sprawdza czy statek ma wolne miejsce na planszy i nie wychodzi poza ni¹
{
	lenght -= 1;
	int test; //Zmienna decyduj¹ca czy dany kierunek statku wp³ynie na jego wyjœcie poza planszê lub czêœæ gracza

	if (direction == EAST || direction == SOUTH)
		test = -1;
	else
		test = 1;
	if (y == GAME.BOARD.y / 2 || x > GAME.BOARD.x)
		return false;
	else if (player == 'A' && direction == NORTH)
	{
		if (y + lenght > A.initPosition[1][0])
			return false;
	}
	else if (player == 'B' && direction == SOUTH)
	{
		if (y - lenght < B.initPosition[0][0])
			return false;
	}
	else if (player == 'A' && y > A.initPosition[1][0])
		return false;
	else if (player == 'B' && y < B.initPosition[0][0])
		return false;
	else if (direction == EAST || direction == WEST)
	{
		if ((player == 'A' && (x + (test * lenght) > A.initPosition[1][1] || x + (test * lenght) < A.initPosition[0][1])) || (player == 'B' && (x + (test * lenght) > B.initPosition[1][1] || x + (test * lenght) < B.initPosition[0][1])))
			return false;
	}
	else if (direction == NORTH || direction == SOUTH)
	{
		if ((player=='A' && (y + (test * lenght) > A.initPosition[1][0] || y + (test * lenght) < A.initPosition[0][0])) || (player == 'B' && (y + (test * lenght) > B.initPosition[1][0] || y + (test * lenght) < B.initPosition[0][0])))
			return false;
	}
	return true;
}
void setVariablesExSC(short* variables, char* direction, char rotation) //Ustawia zmienne pod poruszanie siê statku
{
	if (rotation == 'F')
	{
		if (*direction == NORTH)
		{
			variables[0] -= 1;
		}
		else if (*direction == SOUTH)
		{
			variables[0] += 1;
		}
		else if (*direction == WEST)
		{
			variables[1] -= 1;
		}
		else
		{
			variables[1] += 1;
		}
	}
	else if (rotation == 'R')
	{
		if (*direction == NORTH)
		{
			variables[0] -= 1;
			variables[1] += (variables[2] - 1);
			*direction = EAST;
		}
		else if (*direction == SOUTH)
		{
			variables[0] += 1;
			variables[1] -= (variables[2] - 1);
			*direction = WEST;
		}
		else if (*direction == WEST)
		{
			variables[0] -= (variables[2] - 1);
			variables[1] -= 1;
			*direction = NORTH;
		}
		else
		{
			variables[0] += (variables[2] - 1);
			variables[1] += 1;
			*direction = SOUTH;
		}
	}
	else
	{
		if (*direction == NORTH)
		{
			variables[0] -= 1;
			variables[1] -= (variables[2] - 1);
			*direction = WEST;
		}
		else if (*direction == SOUTH)
		{
			variables[0] += 1;
			variables[1] += (variables[2] - 1);
			*direction = EAST;
		}
		else if (*direction == WEST)
		{
			variables[0] += (variables[2] - 1);
			variables[1] -= 1;
			*direction = SOUTH;
		}
		else
		{
			variables[0] -= (variables[2] - 1);
			variables[1] += 1;
			*direction = NORTH;
		}
	}
}
void setVariables(short shipNumber, short* variables, char* direction) //Ustawia zmienne na te przypisane do danego statku
{
	if (GAME.actualPlayer == 'A')
	{
		variables[0] = A.Ships[shipNumber].y;
		variables[1] = A.Ships[shipNumber].x;
		variables[2] = A.Ships[shipNumber].lenght;
		*direction = A.Ships[shipNumber].rotation;
	}
	else
	{
		variables[0] = B.Ships[shipNumber].y;
		variables[1] = B.Ships[shipNumber].x;
		variables[2] = B.Ships[shipNumber].lenght;
		*direction = B.Ships[shipNumber].rotation;
	}
}
void placeOnBoard(short shipNumber, char* shipParts, char** board) //Umieszcza statek na planszy
{
	short x, y, lenght, variables[3] = { 0 };
	short(*ptr)[3];
	ptr = &variables;
	char direction;
	char* dir = &direction;
	setVariables(shipNumber, *ptr, dir);
	y = variables[0];
	x = variables[1];
	lenght = variables[2];

	if (direction == WEST)
	{
		for (short i = x, j=0; i < x+ lenght; i++,j++)
		{
			board[y][i] = shipParts[j];
		}
	}
	else if (direction == EAST)
	{
		for (short i = x, j=0; i > x- lenght; i--,j++)
		{
			board[y][i] = shipParts[j];
		}
	}
	else if (direction == SOUTH)
	{
		for (short i = y, j=0; i > y - lenght; i--,j++)
		{
			board[i][x] = shipParts[j];
		}
	}
	else
	{
		for (short i = y, j=0; i < y + lenght; i++,j++)
		{
			board[i][x] = shipParts[j];
		}
	}
}
short numberOfParts(char* shipParts) //Zwraca liczbê reprezentuj¹c¹ iloœæ "¿ywych" czêœci statku
{
	short suma = 0;
	for (int i = 0; i < 5; i++)
	{
		if (shipParts[i] == '+')
			suma++;
	}
	return suma;
}
void createShip(short y, short x, char direction, short number, short typeOfShip, char player, short i, short lenght, char* shipParts, char** board) //Tworzy dla gracza nowy statek
{
	short trueLenght = numberOfParts(shipParts);
	if (player == 'A')
	{
		A.numberOfShips++;
		A.ships[typeOfShip]++;
		A.shipsNumber[typeOfShip][number] = i+1;
		A.numberOfParts += trueLenght;

		for (int j = 0; j < lenght; j++)
		{
			A.Ships[i].parts[j] = shipParts[j];
		}
		A.Ships[i].lenght = lenght;
		if (typeOfShip == 0)
			A.Ships[i].moves = 2;
		A.Ships[i].shoots = lenght;
		A.Ships[i].type = typeOfShip;
		A.Ships[i].typeNumber = number;
		A.Ships[i].shipNumber = i + 1;
		A.Ships[i].x = x;
		A.Ships[i].y = y;
		A.Ships[i].rotation = direction;
	}
	else
	{
		B.numberOfShips++;
		B.ships[typeOfShip]++;
		B.shipsNumber[typeOfShip][number] = i+1;
		B.numberOfParts += trueLenght;

		for (int j = 0; j < lenght; j++)
		{
			B.Ships[i].parts[j] = shipParts[j];
		}
		B.Ships[i].lenght = lenght;
		if (typeOfShip == 0)
			B.Ships[i].moves = 2;
		B.Ships[i].shoots = lenght;
		B.Ships[i].type = typeOfShip;
		B.Ships[i].typeNumber = number;
		B.Ships[i].shipNumber = i + 1;
		B.Ships[i].x = x;
		B.Ships[i].y = y;
		B.Ships[i].rotation = direction;
	}
	placeOnBoard(i, shipParts, board);
}
void changeParts(char* shipParts, short lenght) //Zmienia czêœci statku z liczba na znaki
{
	for (short i = 0; i < lenght; i++)
	{
		if (shipParts[i] == '1' || shipParts[i] == 0)
			shipParts[i] = '+';
		else
			shipParts[i] = 'x';
	}
	shipParts[5] = '\0';
}
void placeShip(short y, short x, char direction, short number, char* shipType, char player, short isState, char** board) //Funkcja umieszczaj¹ca statek gracza na planszy
{
	short typeOfShip = shipTypeNumber(shipType);
	short lenght = shipTypeLenght(typeOfShip);
	char shipParts[6] = { 0 };
	char(*ptr)[6];
	ptr = &shipParts;
	if (isState == 1)
		cin >> shipParts;
	char** exBoard;
	exBoard = setExBoard(board);
	if (checkAmount(typeOfShip, number, player) == false)
	{
		cout << "INVALID OPERATION \"PLACE_SHIP " << y << " " << x << " " << direction << " " << number << " " << shipType << "\": SHIP ALREADY PRESENT" << endl;
		freeMemory(board); 
		exit(0);
	}
	else if (checkType(typeOfShip, player) == false)
	{
		cout << "INVALID OPERATION \"PLACE_SHIP " << y << " " << x << " " << direction << " " << number << " " << shipType << "\": ALL SHIPS OF THE CLASS ALREADY SET" << endl;
		freeMemory(board); 
		exit(0);
	}
	else if (checkSpace(y, x, direction, lenght, player) == false)
	{
		cout << "INVALID OPERATION \"PLACE_SHIP " << y << " " << x << " " << direction << " " << number << " " << shipType << "\": NOT IN STARTING POSITION" << endl;
		freeMemory(board); 
		exit(0);
	}
	else if (checkReef(y, x, direction, lenght, board) == false)
	{
		cout << "INVALID OPERATION \"PLACE_SHIP " << y << " " << x << " " << direction << " " << number << " " << shipType << "\": PLACING SHIP ON REEF" << endl;
		freeMemory(board); 
		exit(0);
	}
	else if (extendedSpaceCheck(y, x, direction, lenght, exBoard) == false)
	{
		if (isState == 0)
		{
			cout << "INVALID OPERATION \"PLACE_SHIP " << y << " " << x << " " << direction << " " << number << " " << shipType << "\": PLACING SHIP TOO CLOSE TO OTHER SHIP" << endl;
			freeMemory(board); 
			exit(0);
		}
		else
		{
			cout << "INVALID OPERATION \"SHIP " << player <<" "<< y << " " << x << " " << direction << " " << number << " " << shipType << " " << shipParts << "\": PLACING SHIP TOO CLOSE TO OTHER SHIP" << endl;
			freeMemory(board); 
			exit(0);
		}
	}
	else
	{
		if (player == 'A')
		{
			for (short i = 0; i < NUMBEROFSHIPS; i++)
			{
				if (A.Ships[i].shipNumber < 1)
				{
					changeParts(*ptr, lenght);
					createShip(y, x, direction, number, typeOfShip, player, i, lenght, *ptr, board);
					break;
				}
			}
		}
		else
		{
			for (short i = 0; i < NUMBEROFSHIPS; i++)
			{
				if (B.Ships[i].type < 1)
				{
					changeParts(*ptr, lenght);
					createShip(y, x, direction, number, typeOfShip, player, i, lenght, *ptr, board);
					break;
				}
			}
		}
	}
}
void removeShip(short* variables, char* direction, char** board) //Usuwa statek gracza, wymagane dla innej funkcji
{
	if (*direction == WEST)
	{
		for (int i = variables[1], j = 0; i < variables[1] + variables[2]; i++, j++)
		{
			board[variables[0]][i] = ' ';
		}
	}
	else if (*direction == EAST)
	{
		for (int i = variables[1], j = 0; i > variables[1] - variables[2]; i--, j++)
		{
			board[variables[0]][i] = ' ';
		}
	}
	else if (*direction == SOUTH)
	{
		for (int i = variables[0], j = 0; i > variables[0] - variables[2]; i--, j++)
		{
			board[i][variables[1]] = ' ';
		}
	}
	else
	{
		for (int i = variables[0], j = 0; i < variables[0] + variables[2]; i++, j++)
		{
			board[i][variables[1]] = ' ';
		}
	}
}
void moveShip(short shipNumber, short* variables, char* direction, char rotation, char** board) //Przesuwa statek
{
	removeShip(variables, direction, board);
	setVariablesExSC(variables, direction, rotation);
	if (GAME.actualPlayer == 'A')
	{
		A.Ships[shipNumber].y = variables[0];
		A.Ships[shipNumber].x = variables[1];
		A.Ships[shipNumber].rotation = *direction;
		A.Ships[shipNumber].moves -= 1;
	}
	else
	{
		B.Ships[shipNumber].y = variables[0];
		B.Ships[shipNumber].x = variables[1];
		B.Ships[shipNumber].rotation = *direction;
		B.Ships[shipNumber].moves -= 1;
	}
	char shipParts[6] = { 0 };
	char(*ptr)[6] = &shipParts;
	changeParts(*ptr, variables[2]);
	placeOnBoard(shipNumber, *ptr, board);
	if (GAME.actualPlayer == 'A')
	{
		GAME.resetPlayersShipMoves[0][shipNumber] = 1;
	}
	else
	{
		GAME.resetPlayersShipMoves[1][shipNumber] = 1;
	}
}
bool fitInBoard(short y, short x, char direction) //Czy statek po ruchu zmieœci siê na planszy
{
	if (direction == NORTH)
	{
		if (y - 1 < 0)
			return false;
	}
	else if (direction == SOUTH)
	{
		if (y + 1 > GAME.BOARD.y)
			return false;
	}
	else if (direction == WEST)
	{
		if (x - 1 < 0)
			return false;
	}
	else
	{
		if (x + 1 > GAME.BOARD.x)
			return false;
	}
	return true;
}
bool checkEngine(short shipNumber) //Sprawdza czy statek ma uszkodzony silnik
{
	if (GAME.actualPlayer == 'A')
	{
		if (A.Ships[shipNumber].parts[A.Ships[shipNumber].lenght-1] == 'x')
			return false;
	}
	else
	{
		if (B.Ships[shipNumber].parts[B.Ships[shipNumber].lenght - 1] == 'x')
			return false;
	}
	return true;
}
void checkMoveOfShipAddon(char* direction, short* variables, char rotation, short number, short shipNumber, char* ship, char** board) //Podfunkcja checkMoveOfShip
{
	char copyDirection = *direction; short copyVariables[3];
	copyVariables[0] = variables[0]; copyVariables[1] = variables[1]; copyVariables[2] = variables[2];
	if (fitInBoard(variables[0], variables[1], *direction) == false)
	{
		cout << "INVALID OPERATION \"MOVE " << number << " " << ship << " " << rotation << "\": SHIP WENT FROM BOARD" << endl;
		freeMemory(board); 
		exit(0);
	}
	else if (checkEngine(shipNumber) == false)
	{
		cout << "INVALID OPERATION \"MOVE " << number << " " << ship << " " << rotation << "\": SHIP CANNOT MOVE" << endl;
		freeMemory(board);
		exit(0);
	}
	else
	{
		removeShip(variables, direction, board);
		setVariablesExSC(variables, direction, rotation);
		char** exBoard;
		exBoard = setExBoard(board);
		if (extendedSpaceCheck(variables[0], variables[1], *direction, variables[2], exBoard) == false)
		{
			cout << "INVALID OPERATION \"MOVE " << number << " " << ship << " " << rotation << "\": PLACING SHIP TOO CLOSE TO OTHER SHIP" << endl;
			freeMemory(board); 
			exit(0);
		}
		char shipParts[6] = { 0 };
		char(*wsk)[6] = &shipParts;
		changeParts(*wsk, variables[2]);
		placeOnBoard(shipNumber, *wsk, board);
	}
	*direction = copyDirection; variables[0] = copyVariables[0]; variables[1] = copyVariables[1]; variables[2] = copyVariables[2];
	moveShip(shipNumber, variables, direction, rotation, board);
}
void checkMoveOfShip(char** board) //Sprawdza czy statek mo¿e siê poruszyæ i jeœli tak to go przesuwa
{
	short number, shipNumber, shipType, variables[3] = { 0 };
	short (*ptr)[3];
	ptr = &variables;
	char ship[4], rotation, direction;
	char* dir = &direction;
	cin >> number >> ship >> rotation;
	shipType = shipTypeNumber(ship);
	if(GAME.actualPlayer=='A')
		shipNumber = A.shipsNumber[shipType][number]-1;
	else
		shipNumber = B.shipsNumber[shipType][number] - 1;
	setVariables(shipNumber, *ptr, dir);

	if (GAME.actualPlayer == 'A')
	{
		if (A.Ships[shipNumber].moves < 1)
		{
			cout << "INVALID OPERATION \"MOVE " << number << " " << ship << " " << rotation << "\": SHIP MOVED ALREADY" << endl;
			freeMemory(board); 
			exit(0);
		}
		else
		{
			checkMoveOfShipAddon(dir, *ptr, rotation, number, shipNumber, ship, board);
		}
	}
	else
	{
		if (B.Ships[shipNumber].moves < 1)
		{
			cout << "INVALID OPERATION \"MOVE " << number << " " << ship << " " << rotation << "\": SHIP MOVED ALREADY" << endl;
			freeMemory(board); 
			exit(0);
		}
		else 
		{
			checkMoveOfShipAddon(dir, *ptr, rotation, number, shipNumber, ship, board);
		}
	}
}
void searchForShip(short y, short x) //Wyszukuje statek i zmienia jego czêœci na zniszczone
{
	if (GAME.actualPlayer == 'A')
	{
		for (short i = 0; i < NUMBEROFSHIPS; i++)
		{
			if (B.Ships[i].rotation == NORTH)
			{
				for (short j = B.Ships[i].y, a=0; j < B.Ships[i].y + B.Ships[i].lenght; j++, a++)
				{
					if (j == y)
						B.Ships[i].parts[a] = 'x';
				}
			}
			else if (B.Ships[i].rotation == SOUTH)
			{
				for (short j = B.Ships[i].y, a = 0; j > B.Ships[i].y - B.Ships[i].lenght; j--, a++)
				{
					if (j == y)
						B.Ships[i].parts[a] = 'x';
				}
			}
			else if (B.Ships[i].rotation == WEST)
			{
				for (short j = B.Ships[i].x, a = 0; j < B.Ships[i].x + B.Ships[i].lenght; j++, a++)
				{
					if (j == x)
						B.Ships[i].parts[a] = 'x';
				}
			}
			else if (B.Ships[i].rotation == EAST)
			{
				for (short j = B.Ships[i].x, a = 0; j > B.Ships[i].x - B.Ships[i].lenght; j--, a++)
				{
					if (j == x)
						B.Ships[i].parts[a] = 'x';
				}
			}
		}
	}
	else
	{
		for (short i = 0; i < NUMBEROFSHIPS; i++)
		{
			if (A.Ships[i].rotation == NORTH)
			{
				for (short j = A.Ships[i].y, a = 0; j < A.Ships[i].y + A.Ships[i].lenght; j++, a++)
				{
					if (j == y)
						A.Ships[i].parts[a] = 'x';
				}
			}
			else if (A.Ships[i].rotation == SOUTH)
			{
				for (short j = A.Ships[i].y, a = 0; j > A.Ships[i].y - A.Ships[i].lenght; j--, a++)
				{
					if (j == y)
						A.Ships[i].parts[a] = 'x';
				}
			}
			else if (A.Ships[i].rotation == WEST)
			{
				for (short j = A.Ships[i].x, a = 0; j < A.Ships[i].x + A.Ships[i].lenght; j++, a++)
				{
					if (j == x)
						A.Ships[i].parts[a] = 'x';
				}
			}
			else if (A.Ships[i].rotation == EAST)
			{
				for (short j = A.Ships[i].x, a = 0; j > A.Ships[i].x - A.Ships[i].lenght; j--, a++)
				{
					if (j == x)
						A.Ships[i].parts[a] = 'x';
				}
			}
		}
	}
}
void exShooting(char** board, short shipNumber, short y, short x) //Zaawansowane strzelanie
{
	if (GAME.actualPlayer == 'A')
	{
		A.Ships[shipNumber].shoots -= 1;
		if (board[y][x] == '+')
			board[y][x] = 'x';
		B.numberOfParts -= 1;
		searchForShip(y, x);
		GAME.resetPlayersShipShoots[0][shipNumber] = 1;
	}
	else
	{
		B.Ships[shipNumber].shoots -= 1;
		if (board[y][x] == '+')
			board[y][x] = 'x';
		A.numberOfParts -= 1;
		searchForShip(y, x);
		GAME.resetPlayersShipShoots[1][shipNumber] = 1;
	}
}
bool checkDistance(short shipNumber, short x2, short y2) //Sprawdza dystans strza³u
{
	if (A.Ships[shipNumber].type == 0)
		return true;
	double distance=0; short x1=0, y1=0;
	if (GAME.actualPlayer == 'A')
	{
		if (A.Ships[shipNumber].rotation == NORTH)
		{
			y1 = A.Ships[shipNumber].y - 1;
			x1 = A.Ships[shipNumber].x;
		}
		else if (A.Ships[shipNumber].rotation == SOUTH)
		{
			y1 = A.Ships[shipNumber].y + 1;
			x1 = A.Ships[shipNumber].x;
		}
		else if (A.Ships[shipNumber].rotation == WEST)
		{
			x1 = A.Ships[shipNumber].x + 1;
			y1 = A.Ships[shipNumber].y;
		}
		else if (A.Ships[shipNumber].rotation == EAST)
		{
			x1 = A.Ships[shipNumber].x - 1;
			y1 = A.Ships[shipNumber].y;
		}
		distance = sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2));
		if (distance <= A.Ships[shipNumber].lenght)
			return true;
	}
	else
	{
		if (B.Ships[shipNumber].rotation == NORTH)
		{
			y1 = B.Ships[shipNumber].y - 1;
			x1 = B.Ships[shipNumber].x;
		}
		else if (B.Ships[shipNumber].rotation == SOUTH)
		{
			y1 = B.Ships[shipNumber].y + 1;
			x1 = B.Ships[shipNumber].x;
		}
		else if (B.Ships[shipNumber].rotation == WEST)
		{
			x1 = B.Ships[shipNumber].x + 1;
			y1 = B.Ships[shipNumber].y;
		}
		else if (B.Ships[shipNumber].rotation == EAST)
		{
			x1 = B.Ships[shipNumber].x - 1;
			y1 = B.Ships[shipNumber].y;
		}
		distance = sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2));
		if (distance <= B.Ships[shipNumber].lenght)
			return true;
	}
	return false;
}
bool checkCannon(short shipNumber) //Sprawdza czy statek moze strzelaæ
{
	if (GAME.actualPlayer == 'A')
	{
		if (A.Ships[shipNumber].parts[1] == 'x')
			return false;
	}
	else
	{
		if (B.Ships[shipNumber].parts[1] == 'x')
			return false;
	}
	return true;
}
bool checkShootsNumber(short shipNumber) //Sprawdza czy statek ma jeszcze amunicjê
{
	if (GAME.actualPlayer == 'A')
	{
		if (A.Ships[shipNumber].shoots < 1)
			return false;
	}
	else
	{
		if (B.Ships[shipNumber].shoots < 1)
			return false;
	}
	return true;
}
bool checkShoot(short y, short x, char** board) //Sprawdza czy gracz mo¿e strzelaæ
{
	if (y < 0 || y > GAME.BOARD.y || x < 0 || x > GAME.BOARD.x)
	{
		cout << "INVALID OPERATION \"SHOOT " << y << " " << x << "\": FIELD DOES NOT EXIST" << endl;
		freeMemory(board);
		exit(0);
	}
	else if (A.numberOfShips != A.fleetNumber || B.numberOfShips != B.fleetNumber)
	{
		cout << "INVALID OPERATION \"SHOOT " << y << " " << x << "\": NOT ALL SHIPS PLACED" << endl;
		freeMemory(board);
		exit(0);
	}
	return true;
}
void exCheckShoot(char** board) //Sprawdza warunki dla extended shooting
{
	short number, x, y, shipType, shipNumber;
	char ship[4];
	cin >> number >> ship >> y >> x;
	shipType = shipTypeNumber(ship);
	if (GAME.actualPlayer == 'A')
		shipNumber = A.shipsNumber[shipType][number] - 1;
	else
		shipNumber = B.shipsNumber[shipType][number] - 1;
	checkShoot(y, x, board);
	if (checkShootsNumber(shipNumber) == false)
	{
		cout << "INVALID OPERATION \"SHOOT " << number << " " << ship << " " << y << " " << x << "\": TOO MANY SHOOTS" << endl;
		freeMemory(board);
		exit(0);
	}
	else if (checkCannon(shipNumber) == false)
	{
		cout << "INVALID OPERATION \"SHOOT " << number << " " << ship << " " << y << " " << x << "\": SHIP CANNOT SHOOT" << endl;
		freeMemory(board);
		exit(0);
	}
	else if (checkDistance(shipNumber, x, y) == false)
	{
		cout << "INVALID OPERATION \"SHOOT " << number << " " << ship << " " << y << " " << x << "\": SHOOTING TOO FAR" << endl;
		freeMemory(board);
		exit(0);
	}
	exShooting(board, shipNumber, y, x);
}
void shoot(short y, short x, char** board) //Strzelanie podstawowe
{
	if (board[y][x] == '+')
	{
		board[y][x] = 'x';
		if (y < GAME.BOARD.y/2)
		{
			A.numberOfParts -= 1;
		}
		else
		{
			B.numberOfParts -= 1;
		}
	}
}
void checkWin(char** board) //Sprawdza czy ktoœ wygra³
{
	if (A.numberOfShips == A.fleetNumber && B.numberOfShips == B.fleetNumber && A.numberOfParts < 1)
	{
		GAME.winner = 'B';
		cout << "B won" << endl;
		freeMemory(board); 
		exit(0);
	}
	else if (A.numberOfShips == A.fleetNumber && B.numberOfShips == B.fleetNumber && B.numberOfParts < 1)
	{
		GAME.winner = 'A';
		cout << "A won" << endl;
		freeMemory(board); 
		exit(0);
	}
}
void resetShipMoves() //Resetuje ruchy statków
{
	if (GAME.actualPlayer == 'A')
	{
		for (short i = 0; i < 50; i++)
		{
			if (GAME.resetPlayersShipMoves[0][i] == 1)
			{
				if (A.Ships[i].type == 0)
					A.Ships[i].moves = 2;
				else
					A.Ships[i].moves = 3;
			}
		}
	}
	else
	{
		for (short i = 0; i < 50; i++)
		{
			if (GAME.resetPlayersShipMoves[1][i] == 1)
			{
				if (B.Ships[i].type == 0)
					B.Ships[i].moves = 2;
				else
					B.Ships[i].moves = 3;
			}
		}
	}
}
void resetShipShoots() //Resetuje amunicjê statków
{
	if (GAME.actualPlayer == 'A')
	{
		for (short i = 0; i < 50; i++)
		{
			if (GAME.resetPlayersShipShoots[0][i] == 1)
			{
				A.Ships[i].shoots = A.Ships[i].lenght;
				break;
			}
		}
	}
	else
	{
		for (short i = 0; i < 50; i++)
		{
			if (GAME.resetPlayersShipShoots[1][i] == 1)
			{
				B.Ships[i].shoots= B.Ships[i].lenght;
				break;
			}
		}
	}
}
void endOfTurn(char** board) //Koniec tury
{
	checkWin(board);
	resetShipMoves();
	resetShipShoots();
	if (GAME.actualPlayer == 'A')
		GAME.actualPlayer = 'B';
	else
		GAME.actualPlayer = 'A';
}
bool doPlayer(char player, char* commandP, char** board) //Wykonywanie komend gracza
{
	char command[15] = { 0 };
	while (cin.good())
	{
		if (player == GAME.actualPlayer)
		{
			short y, x;
			cin >> command;
			if (strcmp("PLACE_SHIP", command) == 0)
			{
				short number;
				char direction;
				char(*ptr)[4];
				char shipType[4];
				ptr = &shipType;
				cin >> y >> x >> direction >> number >> shipType;
				placeShip(y, x, direction, number, *ptr, player, 0, board);
			}
			else if (strcmp("SHOOT", command) == 0)
			{
				if (GAME.exShipsON == false)
				{
					cin >> y >> x;
					if (checkShoot(y, x, board) == true)
						shoot(y, x, board);
				}
				else
					exCheckShoot(board);
			}
			else if (strcmp("MOVE", command) == 0)
			{
				checkMoveOfShip(board);
			}
			else if (strcmp("[playerA]", command) == 0 || strcmp("[playerB]", command) == 0)
			{
				endOfTurn(board);
				return true;
			}
		}
		else
		{
			cout << "INVALID OPERATION \"" << commandP << " " << "\": THE OTHER PLAYER EXPECTED" << endl;
			freeMemory(board); 
			exit(0);
		}
	}
	return true;
}
void setPosition()
{
	char player;
	short init[4];
	cin >> player >> init[0] >> init[1] >> init[2] >> init[3];
	if (player == 'A')
	{
		A.initPosition[0][0] = init[0];
		A.initPosition[1][0] = init[2];
		A.initPosition[0][1] = init[1];
		A.initPosition[1][1] = init[3];
	}
	else
	{
		B.initPosition[0][0] = init[0];
		B.initPosition[1][0] = init[2];
		B.initPosition[0][1] = init[1];
		B.initPosition[1][1] = init[3];
	}
}
void printBoard(char** board) //Wypisanie planszy
{
	for (int i = 0; i <GAME.BOARD.y; i++)
	{
		for (int j = 0; j < GAME.BOARD.x; j++)
		{
			cout << board[i][j];
		}
		cout << endl;
	}
	cout << "PARTS REMAINING:: A : " << A.numberOfParts << " B : " << B.numberOfParts << endl;
}
void statePlaceShip(char** board) //Wstawianie statku z komend typu [state]
{
	char player, playercopy, direction, ship[4];
	short y, x, number;
	char(*ptr)[4];
	ptr = &ship;
	cin >> player >> y >> x >> direction >> number >> ship;
	playercopy = GAME.actualPlayer;
	GAME.actualPlayer = player;
	placeShip(y, x, direction, number, *ptr, player, 1, board);
	GAME.actualPlayer = playercopy;
}
void setFleet() //Ustawia ograniczenie statków
{
	char player;
	short fleet[4];
	cin >> player;
	cin >> fleet[0] >> fleet[1] >> fleet[2] >> fleet[3];

	if (player == 'A')
	{
		A.fleet[0] = fleet[0];
		A.fleet[1] = fleet[1];
		A.fleet[2] = fleet[2];
		A.fleet[3] = fleet[3];
		A.fleetNumber = fleet[0] + fleet[1] + fleet[2] + fleet[3];
	}
	else
	{
		B.fleet[0] = fleet[0];
		B.fleet[1] = fleet[1];
		B.fleet[2] = fleet[2];
		B.fleet[3] = fleet[3];
		B.fleetNumber = fleet[0] + fleet[1] + fleet[2] + fleet[3];
	}
}
void placeReef(char** board)
{
	short y, x;
	cin >> y >> x;
	if (y<0||y>GAME.BOARD.y|| x<0 || x>GAME.BOARD.x)
	{
		cout << "INVALID OPERATION REEF \"" << y << " " << x << "\": REEF IS NOT PLACED ON BOARD" << endl;
		freeMemory(board);
		exit(0);
	}
	else
	{
		board[y][x] = '#';
	}
}
void changePlayer() //Zmienia gracza
{
	char player;
	cin >> player;
	if (player == 'A')
	{
		GAME.actualPlayer = 'A';
	}
	else
	{
		GAME.actualPlayer = 'B';
	}
}
char** createCustomBoard(bool create, char** board) //Tworzy customow¹ planszê
{
	if (create==true)
	{
		short y, x;
		cin >> y >> x;
		(char**) realloc(board, y * sizeof* board);
		for (int i = 0; i < y; i++)
			board[i] = (char*) realloc(board[i], x * sizeof* *board);
		GAME.isCustomBoard = true;
		GAME.BOARD.y = y;
		GAME.BOARD.x = x;
		setBoard(board);
	}
	else
	{
		char** customBoard;
		customBoard = new char* [BOARDy];
		for (int i = 0; i < BOARDy; i++)
			customBoard[i] = new char[BOARDx];
		return customBoard;
	}
	return 0;
}
bool doState(char** board) //Wykonywanie komend stanu gry
{
	char command[15] = { 0 };
	while (cin.good())
	{
		cin >> command;
		if (strcmp("PRINT", command) == 0)
		{
			short x; cin >> x;
			if (x == 0)
				printBoard(board);
		}
		else if (strcmp("SET_FLEET", command) == 0)
		{
			setFleet();
		}
		else if (strcmp("NEXT_PLAYER", command) == 0)
		{
			changePlayer();
		}
		else if (strcmp("SHIP", command) == 0)
		{
			statePlaceShip(board);
		}
		else if (strcmp("REEF", command) == 0)
		{
			placeReef(board);
		}
		else if (strcmp("INIT_POSITION", command) == 0)
		{
			setPosition();
		}
		else if (strcmp("BOARD_SIZE", command) == 0)
		{
			createCustomBoard(true, board);
		}
		else if (strcmp("EXTENDED_SHIPS", command) == 0)
		{
			GAME.exShipsON = true;
		}
		else if (strcmp("[state]", command) == 0)
		{
			return true;
		}
	}
	return true;
}
void run(char** board) //Podrzêdna funkcja g³ówna
{
	char command[10];
	cin >> command;
	if (strcmp(command, "") == 0)
	{
		freeMemory(board);
		exit(0);
	}
	if (strcmp("[state]", command) == 0)
	{
		if (doState(board) == true)
			run(board);
	}
	else
	{
		char player;
		char(*ptr)[10];
		ptr = &command;
		player = command[7];
		if (doPlayer(player, *ptr, board) == true)
			run(board);
	}
}
void Game() //Ustawia warunki gry
{
	char** board = 0;
	board = createCustomBoard(false, board);
	setBoard(board);

	GAME.actualPlayer = 'A';

	A.initPosition[0][0] = 0;
	A.initPosition[1][0] = 9;
	A.initPosition[0][1] = 0;
	A.initPosition[1][1] = 9;

	B.initPosition[0][0] = 11;
	B.initPosition[1][0] = 20;
	B.initPosition[0][1] = 0;
	B.initPosition[1][1] = 9;
	run(board);
}
int main()
{
	Game();
}