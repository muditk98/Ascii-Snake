#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <math.h>

#define FOOD -1
#define MAX_LEN 200

#define UP    0
#define LEFT  1
#define DOWN  2
#define RIGHT 3
#define PAUSE 4
#define MENU  5
 
#define WHITE    15
#define RED      12
#define GREEN    10
#define DARK_RED 4 

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);	//	to modify console window

void gotoxy(short x, short y)						
{
  COORD c = { x, y };  
  SetConsoleCursorPosition(console, c);
}


class Preferences 									// to save and read keyboard bindings
{
	char bindings[6];
public:
	Preferences();
	void getnew();
	void display();
	void defaults();								// get new keyboard bindings from user
	int retbinding(char);
} prefs;

Preferences::Preferences()
{
	defaults();
}

int Preferences::retbinding(char b)
{
	for (int i = 0; i < 6; ++i)
	{
		if (b == bindings[i])
		{
			return i;
		}
	}
	return -1;
}

void Preferences::defaults()
{
	bindings[UP]    = 'w';
	bindings[LEFT]  = 'a';
	bindings[DOWN]  = 's';
	bindings[RIGHT] = 'd';
	bindings[PAUSE] = 'p';
	bindings[MENU]  = 'm';
}

void Preferences::getnew()
{
	system("cls");
	char a[6];
	for (int i = 0; i < 6; ++i)
	{
		a[i] = bindings[i];
	}
	char temp;
	cout << setw(10) << std::left << "UP  "    << a[UP]    << endl;
	cout << setw(10) << std::left << "LEFT  "  << a[LEFT]  << endl;
	cout << setw(10) << std::left << "DOWN  "  << a[DOWN]  << endl;
	cout << setw(10) << std::left << "RIGHT  " << a[RIGHT] << endl;
	cout << setw(10) << std::left << "PAUSE  " << a[PAUSE] << endl;
	cout << setw(10) << std::left << "MENU  "  << a[MENU]  << endl;
	for (int i = 0; i < 6; ++i)
	{	
		gotoxy(11, i);
		temp = a[i];
		while (temp != '\r'){
			a[i] = temp;
			cout << "\b \b";
			cout << a[i];
			temp = getch();
			if(temp == '\b')
				cout << "  ";
		}
	}
	for (int i = 0; i < 6; ++i)
	{
		bindings[i] = a[i];
	}
}

void Preferences::display()
{
	char temp;
	system("cls");
	cout << setw(10) << std::left << "UP  "    << bindings[UP]    << endl;
	cout << setw(10) << std::left << "LEFT  "  << bindings[LEFT]  << endl;
	cout << setw(10) << std::left << "DOWN  "  << bindings[DOWN]  << endl;
	cout << setw(10) << std::left << "RIGHT  " << bindings[RIGHT] << endl;
	cout << setw(10) << std::left << "PAUSE  " << bindings[PAUSE] << endl;
	cout << setw(10) << std::left << "MENU  "  << bindings[MENU]  << endl;
	temp = getch();
}


class Score  									// for output of highscores to file
{
	private:
		unsigned int score;
		char name[30];
	public:
		Score();
		void setscore(unsigned int);
		void setname(char*);
		int retscore();
		void display();
		int cmpname(char*);
};

Score::Score()
{
	score = 0;
	strcpy(name, "\0");
}

void Score::setscore(unsigned int score)
{
	this->score = score;
}

void Score::setname(char* name)
{
	strcpy(this->name, name);
}

int Score::retscore()
{
	return score;
}

void Score::display()
{
	cout << left << setw(30) << name << right << setw(3) << score << endl;
}

int Score::cmpname(char* str)
{
	return strcmp(name, str);
}


class Snake 							// The core of the snake game
{
	private:
		int length;						// stores length of snake, also used for score
		int head_x, head_y;  			
		int move_dir;					
		bool grow;
		short arena[25][80];			// is the snake arena, the array in which the snake moves
		clock_t timer;		  			// timer to move forward	
		void move_forward();			// moves the snake forward, increases the length if grow is set true and kills the snake if end_parameter is true 
		bool end_parameter;				// 1 = end
		void spawn_food();     			// randomly spawns food in empty area after food is eaten
		void input(char);				// sets move_dir or exits
		void display();					// displays the snake arena
		void check_timer();				// checks timer and calls move_forward if enough time has passed
		void reset();
	public:
		Snake();						// constructor, sets default values
		int run();						// runs the snake game and returns length used for score
};

Snake::Snake()
{
	reset();
}

void Snake::reset()
{
	srand(time(0));
	length = 1;
	end_parameter = 0;
	move_dir = (rand() % 4);
	grow = false;
	for(int i = 0; i < 25; i++)
		for(int j = 0; j < 80; j++)
			arena[i][j] = 0;
	head_x = 40;
	head_y = 12;
	arena[head_y][head_x] = 1;
	timer = clock();
}

void Snake::check_timer()
{
	double _time = (clock() - timer) / (double)CLOCKS_PER_SEC;
	if( _time >= (0.22 - (length/(MAX_LEN * 5.0))))
	{
		timer = clock();
		move_forward();
	}
}

void Snake::input(char temp)
{
	int inp = prefs.retbinding(temp);
	if (inp == MENU)
	{
		system("cls");
		cout << "Do you really want to quit?(y/n)\n";
		char ans;
		cin >> ans;
		if(tolower(ans) == 'y')
		{
			end_parameter = true;
			system("cls");
			display();
		}
		else
			display();
	}
	else if (inp == PAUSE)
	{
		int i = 3;
		system("cls");
		system("pause");
		system("cls");
		while(i > 0)
		{
			gotoxy(30, 12);
			cout << "Game will resume in " << i << " ...";
			Sleep(1000);
			i--;
		}
		display();
	}
	else if(inp != -1 && abs(inp - move_dir) != 2)
	{
		move_dir = inp;
		timer = clock();
		move_forward();
	}
}

void Snake::spawn_food()
{
	int x, y;
	x = rand() % 80;
	y = rand() % 24;
	while(arena[y][x]){
		x = rand() % 80;
		y = rand() % 24;
	}
	arena[y][x] = FOOD;
	gotoxy(x,y);
	SetConsoleTextAttribute(console, GREEN);
	cout << '$';
	SetConsoleTextAttribute(console, WHITE);
}

void Snake::move_forward()
{
	int x, y;
	if (move_dir == UP)
		head_y = (head_y + 23) % 24;
	else if (move_dir == LEFT)
		head_x = (head_x + 79) % 80;
	else if (move_dir == DOWN)
		head_y = (head_y + 1) % 24;
	else if (move_dir == RIGHT)
		head_x = (head_x + 1) % 80;
	x = head_x;
	y = head_y;
	if(arena[y][x] == 0)
		arena[y][x] = 1;
	else if(arena[y][x] == FOOD)
	{
		grow = true;
		arena[y][x] = 1;
		spawn_food();
	}
	else 
		end_parameter = true;
	if(end_parameter == false)
	{
		gotoxy(x,y);
		SetConsoleTextAttribute(console, RED);
		cout << '@';
		SetConsoleTextAttribute(console, DARK_RED);
		for(int count = 1; count <= length ; count ++)
		{
			if(arena[(y + 23) % 24][x] == arena[y][x])
				y = (y + 23) % 24;
			else if(arena[y][(x + 79) % 80] == arena[y][x])
				x = (x + 79) % 80;
			else if(arena[(y + 1) % 24][x] == arena[y][x])
				y = (y + 1) % 24;
			else if(arena[y][(x + 1) % 80] == arena[y][x])
				x = (x + 1) % 80;
			arena[y][x]++;
			if(count == 1)
			{
				gotoxy(x,y);
				cout << 'O';
			}
		}
		SetConsoleTextAttribute(console, WHITE);
		if(grow)
		{
			length++;
			grow = false;
			if(length == MAX_LEN)
				end_parameter = true;
			gotoxy(0, 24);
			cout << "\t\t\r" << "SCORE : " << length;
		}
		else
			arena[y][x] = 0;
		gotoxy(x,y);
		cout << ' ';
	}
	if(end_parameter == true)
	{
		int counter = 0;
		char ch;
		while(counter < 2)
		{
			if(kbhit())
			{
				ch = getch();
				break;
			}
			display();
			Sleep(300);
			system("cls");
			Sleep(700);
			counter ++;
		}
	}
}

void Snake::display()
{
	system("cls");
	for(int y = 0; y < 24; y++)
	{
		for(int x = 0; x < 80; x++)
		{
			if(arena[y][x] == 0)
				cout << ' ';
			else if(arena[y][x] == -1)
			{
				SetConsoleTextAttribute(console, GREEN);
				cout << '$';
			}
			else if(arena[y][x] == 1)
			{	
				SetConsoleTextAttribute(console, RED);
				cout << '@';
			}
			else 
			{	
				SetConsoleTextAttribute(console, DARK_RED);
				cout << 'O';
			}
		}
	}
	SetConsoleTextAttribute(console, WHITE);
	gotoxy(0, 24);
	cout << "SCORE : " << length;
}

int Snake::run()
{
	char temp;
	display();
	spawn_food();
	while (end_parameter == false)
	{
		if(kbhit())
		{
			temp = getch();
			input(temp);
		}
		check_timer();			
	}
	int templen = length;
	reset();
	return templen;
}


class Game 										// is the overlay of the game, handles the files, menu, scores etc.
{
	private:
		Score highest_score, lowest_score;
		Snake snake;
		ifstream fin;
		ofstream fout;
		int recordcount;						// counts the number of records in the snake.dat file which holds the records
		void set_cursor(bool, int); 			// toggles cursor visibility, also allow changing cursor width. visible = 1 => visible
		void set_screen_size();					// sets the screen size and buffer to 80 x 25
		void readhl();							// sets highest_score, lowest_score, recordcount from snake.dat. also creates snake.dat if it does not exist.
		void setprefs();						// sets new preferences
		void readprefs();						// reads the preferences from snakeprefs.dat
		void showprefs();						// displays preferences to user
		void hlt(const char*, int); 			// clears line then display text with color k.
		const char* message(short);				// returns pre stored messages for menu
		void start();							// starts the snake game
		void menu();							// displays menu and allows starting game, viewing hscore, preferences, help etc.
		void help();							// displays controls
		void show_hscore(); 					// shows hscores from file 
		void insert_hscore(int);				// inserts hscore into file
		void remove_hscore();					// deletes highscores
	public:
		Game();														
};

Game::Game()
{
	SetConsoleTextAttribute(console, WHITE);
	set_screen_size();
	readhl();
	readprefs();
	menu();
}

void Game::readhl()
{
	fout.open("snake.dat", ios::binary | ios::app);
	fout.close();
	fin.open("snake.dat", ios::binary);
	fin.seekg(0, ios::end);
	if(fin.tellg() == 0)
	{
		recordcount = 0;
	}
	else
	{
		fin.seekg(0, ios::beg);
		fin.read((char*)&highest_score, sizeof(Score));
		fin.seekg(-sizeof(Score), ios::end);
		fin.read((char*)&lowest_score, sizeof(Score));
		fin.seekg(0, ios::end);
		recordcount = fin.tellg() / sizeof(Score);
	}
	fin.close();
}

void Game::remove_hscore()
{
	system("cls");
	cout << "Do you want to delete all highscores?(y/n)\n";
	char ans = 'n';
	cin >> ans;
	if(tolower(ans) == 'y')
	{	
		cout << "Are you sure you want to delete all highscores?(y/n)\nThis action cannot be undone.\n";
		cin >> ans;
		if(tolower(ans) == 'y')
		{
			fout.open("snake.dat", ios::trunc);
			cout << "All Highscores deleted\n";
			fout.close();
			readhl();
		}
	}
	else
	{
		cout << "Do you want to delete a specific highscore?(y/n)\n";
		cin >> ans;
		bool found = false;
		if (tolower(ans) == 'y')
		{
			fin.open("snake.dat", ios::binary);
			fout.open("temp.dat", ios::binary);
			Score temp;
			cout << "Enter the name of the highscorer:\n";
			char name[30];
			cin >> name;
			while(fin.read((char*)&temp, sizeof(temp)))
			{
				if(!(temp.cmpname(name)))
				{
					cout << "Are you sure you want to delete this highscore?(y/n)\nThis action cannot be undone.\n";
					temp.display();
					cin >> ans;
					found = true;
					if(tolower(ans) == 'n')
						fout.write((char*)&temp, sizeof(temp));
					else if(tolower(ans) == 'y')
						cout << "The highscore has been deleted.\n";
				}
				else
					fout.write((char*)&temp, sizeof(temp));
			}
			fin.close();
			fout.close();
			remove("snake.dat");
			rename("temp.dat", "snake.dat");
		}
			if(!found)
				cout << "Highscorer not found\n";
	}

	ans = getch();
}

void Game::readprefs()
{
	fout.open("snakeprefs.dat", ios::binary | ios::app);
	fout.close();
	fin.open("snakeprefs.dat", ios::binary);
	fin.seekg(0, ios::end);
	if(fin.tellg() == sizeof(Preferences))
	{
		fin.seekg(0, ios::beg);
		fin.read((char*)&prefs, sizeof(Preferences));
	}
	fin.close();
}

void Game::setprefs()
{
	system("cls");
	cout << "Do you want to see the default values?(y/n)\n";
	char ans = 'n';
	cin >> ans;
	if(tolower(ans) == 'y')
		Preferences().display();
	ans = 'o';
	cout << "Do you want to use the default values or set your own?(d/o)\n";
	cin >> ans;
	if(tolower(ans) == 'o')
		prefs.getnew();
	else
		prefs.defaults();
	fout.open("snakeprefs.dat", ios::binary);
	fout.write((char*)&prefs, sizeof(Preferences));
	fout.close();
	ans = getch();
}

void Game::showprefs()
{
	prefs.display();
}

void Game::show_hscore()
{
	system("cls");
	if(recordcount > 0)
	{
		Score temp;
		fin.open("snake.dat", ios::binary);
		int counter = 1;
		while(fin.read((char*)&temp, sizeof(Score)))
		{
			cout << left << setw(2) << counter << ". ";
			temp.display();
			counter ++;
		}
		fin.close();
	}
	else
		cout << "No Highscores";
	char wait;
	wait = getch();
}

void Game::insert_hscore(int length)
{
	system("cls");
	char wait;
	Score newscore;
	if (length == MAX_LEN)
	{
		cout << "Congratulations!! You beat the game!!" << endl;
	}
	if(length > highest_score.retscore())
	{
		cout << "Congratulations!! New highscore!! You scored: " << length << endl;
	}
	else
	{
		cout << "You scored: " << length << endl;
		wait = getch();
	}
	if(length > lowest_score.retscore() || recordcount < 10)
	{
		fin.open("snake.dat", ios::binary);
		fout.open("temp.dat", ios::binary);
		cout << "Enter your name: \n";
		char name[30];
		cin >> name;
		newscore.setname(name);
		newscore.setscore(length);
		Score temp;
		bool written = false;
		while(fin.read((char*)&temp, sizeof(Score)))
		{
			if(!written && (newscore.retscore() > temp.retscore()))
			{
				fout.write((char*)&newscore, sizeof(Score));
				written = true;
			}
			fout.write((char*)&temp, sizeof(Score));
		}
		if(!written)
			fout.write((char*)&newscore, sizeof(Score));
		fout.close();
		fin.close();
		fin.open("temp.dat", ios::binary);
		fout.open("snake.dat", ios::binary);
		int counter = 0;
		while(fin.read((char*)&temp, sizeof(Score)) && counter < 10){
			fout.write((char*)&temp, sizeof(Score));
			counter ++;
		}
		fin.close();
		fout.close();
		remove("temp.dat");
		readhl();
	}	
}

const char* Game::message(short n)
{
	switch(n)
	{
		case 0: return "1: Start Game";
		case 1: return "2: Show Highscores";
		case 2: return "3: Delete Highscores";
		case 3: return "4: Show Preferences";
		case 4: return "5: Set Preferences";
		case 5: return "6: Show Help";
		case 6: return "7: Exit";
	}
}

void Game::hlt(const char* s, int k)
{
	cout << "\r\t\t\t\t\t\t\t\t\t\r";
	SetConsoleTextAttribute(console, k);
	cout << s;
	SetConsoleTextAttribute(console, WHITE);
}

void Game::menu()
{
	char c = 'a';
	int pos = 0;
	bool exit_parameter = false;
	SetConsoleTextAttribute(console, WHITE);
	while(exit_parameter == false)
	{
		system("cls");
		pos = 0;
		c = 'a';
		for(int i = 0; i < 7; i++)
			cout << message(i) << endl;
		gotoxy(0, 0);
		hlt(message(0), 240);      				// 240 => black text with white background. simulates highlight. 
		gotoxy(0, 0);
		while(c != '\r')
		{	
			c = getch();
			if(c == -32)
			{
				c = getch();
				if(c == 'H' && pos > 0)
				{
					hlt(message(pos), WHITE);
					pos--;
				}
				else if(c == 'P' && pos < 6)
				{	
					hlt(message(pos), WHITE);
					pos++;
				}
				gotoxy(0,pos);
				hlt(message(pos), 240);
			}
		}
		system("cls");
		switch(pos)
		{
			case 0: start(); 
					break;
			case 1: show_hscore(); 
					break;
			case 2: remove_hscore();
					break;
			case 3: showprefs();
					break;
			case 4: setprefs();
					break;
			case 5: help();
					break;
			case 6: exit_parameter = true;
		}
	}
}

void Game::help()
{
	system("cls");
	cout << "To view controls open Show Preferences, to set preferences open Set Preferences\nfrom Main Menu.\n";
	cout << "Eat food to grow. Every time food is eaten the snake gets faster.\n";
	cout << "To beat the game reach" << MAX_LEN <<". You lose if the snake eats itself.\n";
	cout << "Enjoy.\n";
	cout << "Created by Mudit Kapoor XII E The Air Force School, Subroto Park, New Delhi.\n";
	cout << "Press any key to return to menu.\n";
	char wait;
	wait = getch();
}

void Game::set_cursor(bool visible, int size = 20) 
{
	if(size == 0)
	{
		size = 20;	// default cursor size Changing to numbers from 1 to 20, decreases cursor width
	}
	CONSOLE_CURSOR_INFO cursor = {visible, size};
	SetConsoleCursorInfo(console, &cursor);
}

void Game::set_screen_size()
{
   	system("mode 80,25");
    COORD NewSBSize = {80, 25};
	int Status;
    Status = SetConsoleScreenBufferSize(console, NewSBSize);
    if (Status == 0)
    {
        Status = GetLastError();
        cout << "SetConsoleScreenBufferSize() failed! Reason : " << Status << endl;
        exit(Status);
    }
}

void Game::start()
{
	system("cls");
	set_cursor(0);
	int length = snake.run();
	set_cursor(1);
	insert_hscore(length);
}


int main()
{
	Game game;
	return 0;
}
