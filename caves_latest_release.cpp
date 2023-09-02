/*
	General Guide

	Map:
 		~ 'T' = Trap
	 	~ 'G' = Loot
	 	~ 'H' = Health Potion
	  	~ 'M' = Mob
	  	~ 'Y' = Yendor Amulet
	  	~ 'E' = Entrance/Exit
	  	~ ' ' = Null

	Loot:
 		~ Gold = value of the game
	  	~ Health Potion = restores health of the player
	 	~ Lantern = allows the player see through the cells around him

	Mobs:
 		~ Skeletons: Health = 12hp; Damage = 2hp

	Weapons:
 		~ Hands: Damage = 3
 		~ Broken bat (b): Damage = 5
 		~ Intact bat (B): Damage = 7
 		~ Double axe (A): Damage = 9
 		~ Pistol (P): Damage = 15
	 
	Made by Michele Hotca 
  	Caves 1.4
*/

#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

const float VERSION = 1.4;

// Struct

struct player
{
	int health = 100;
	int gold = 0;
	bool hasAmulet = false;
	bool hasLantern = false;
	int potionsNumber = 0;
	string weapon;
	int x, y;
};

struct market_item
{
	string items[5];
	// list of possible items: broken bat, intact bat, double axe, pistol, health potion, lantern, yendor amulet
	int bb_percentage = 35;
	int ib_percentage = 20;
	int dx_percentage = 10;
	int p_percentage = 7;
	int hp_percentage = 21;
	int l_percentage = 5;
	int ya_percentage = 2;
};

// Header Functions

// Opens the start menu
void menu(); 
// Starts a new game
void newGame();
// Prints the history/saves
void history();
// Initialize the blank grid
void initializeGrid(char grid[8][8]);
// Prints the map
void printMap(char grid[8][8], player p, bool discovered[8][8], int startx, int starty);
// Generates the current layer
void generateMap(char grid[8][8], int seed, char chestMap);
// All the gui is managed here
void gui(player p, char grid[8][8], bool discovered[8][8], int startx, int starty, int seed);
// Sets a char x[8][8] all to false
void setFalse(bool x[8][8]);
// Reads the updates of the game
void updates();
// Returns the weapon damage
int weaponDamage(player p);
// Equips new weapon
void equip(string weapon, player &p);
// Opens the market
void market(player p, int seed);

// Main

int main() 
{
  menu();
}

// Body Functions

void menu()
{
	bool cycle = true;
	int inputValue = 1;
	while (cycle == true)
	{
		system("clear");
		cout << "\nWelcome to Caves\nInsert:\n\t1) New Game\n\t2) History\n\t3) Credits\n\t4) Updates\n\t0) Exit\n\t: "; cin >> inputValue;
		if (inputValue < 0 || inputValue > 4) 
			cout << "\n[!] Insert valid input";
		else 
			cycle = false;
	}
	switch (inputValue)
	{
		case 0:
			cout << "\nExiting...";
			break;
		case 1:
			system("clear");
			newGame();
			break;
		case 2:
			history();
			break;
		case 3:
			cout << "\nDeveloped by Michele Hotca\n\t\tCaves \n" << VERSION;
			break;
		case 4: 
			updates();
			break;
		default:
			cout << "404";
	}
}

void history()
{
	ifstream file;
	file.open("history.txt");
	string line;
	while (getline(file, line)) // Prints the history line by line
	{
		cout << line << endl;
	}
}

void newGame()
{
	player p; 
	p.weapon = "hands";
	char grid[8][8]; initializeGrid(grid);
	bool discovered[8][8] = 
	{ 
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
	}; // Map parts that are discovered already will be saved here
	
	int seed;
	cout << "\nInsert seed (0 for random one): "; cin >> seed;
	if (seed == 0)
	{
		seed = time(NULL);
		srand(seed); // Use seed for random
	}
	
	p.x = rand() % 8; p.y = rand() % 8; // Generates the player coordinates
	int startx = p.x; int starty = p.y;
	discovered[p.x][p.y] = true; // Sets the starting position to be already discovered

	char chest_map;
	
	generateMap(grid, seed, chest_map);
	grid[p.x][p.y] = 'E'; // Sets the exit
	gui(p, grid, discovered, startx, starty, seed);
}

void initializeGrid(char grid[8][8])
{
	for (int i = 0; i < 8; i++) 
	{
		for (int j = 0; j < 8; j++) // Initialize the blank grid
		{
			grid[i][j] = ' ';
		}
	}
}

void printMap(char grid[8][8], player p, bool discovered[8][8], int startx, int starty) 
{
	bool chunks_loading[8][8] =
	{ 
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
		false, false, false, false, false, false, false, false,
	}; // Chunks loaded for the object "Lantern"

	if (p.hasLantern == true)
	{
		setFalse(chunks_loading);

		if (p.x < 7 && p.x > 0 && p.y < 7 && p.y > 0)
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x + 1][p.y] = true; chunks_loading[p.x + 1][p.y - 1] = true; chunks_loading[p.x + 1][p.y + 1] = true; chunks_loading[p.x][p.y + 1] = true; chunks_loading[p.x - 1][p.y + 1] = true; chunks_loading[p.x - 1][p.y] = true; chunks_loading[p.x][p.y - 1] = true; chunks_loading[p.x - 1][p.y - 1] = true;
		}
		else if (p.x == 7 && p.y == 7 )
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x - 1][p.y] = true; chunks_loading[p.x][p.y - 1] = true; chunks_loading[p.x - 1][p.y - 1] = true;
		}
		else if (p.x == 0 && p.y == 7 )
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x + 1][p.y] = true; chunks_loading[p.x + 1][p.y - 1] = true; chunks_loading[p.x][p.y - 1] = true;
		}
		else if (p.x == 7 && p.y == 0)
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x][p.y + 1] = true; chunks_loading[p.x - 1][p.y + 1] = true; chunks_loading[p.x - 1][p.y] = true;
		}
		else if (p.x == 0 && p.y == 0)
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x + 1][p.y] = true; chunks_loading[p.x + 1][p.y + 1] = true; chunks_loading[p.x][p.y + 1] = true;
		}
		else if (p.x < 7 && p.x > 0 && p.y == 7 && p.y > 0)
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x + 1][p.y] = true; chunks_loading[p.x + 1][p.y - 1] = true; chunks_loading[p.x - 1][p.y] = true; chunks_loading[p.x][p.y - 1] = true; chunks_loading[p.x - 1][p.y - 1] = true;
		}
		else if (p.x < 7 && p.x > 0 && p.y < 7 && p.y == 0)
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x + 1][p.y] = true; chunks_loading[p.x + 1][p.y + 1] = true; chunks_loading[p.x][p.y + 1] = true; chunks_loading[p.x - 1][p.y + 1] = true; chunks_loading[p.x - 1][p.y] = true;
		}
		else if (p.x == 7 && p.x > 0 && p.y < 7 && p.y > 0)
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x][p.y + 1] = true; chunks_loading[p.x - 1][p.y + 1] = true; chunks_loading[p.x - 1][p.y] = true; chunks_loading[p.x][p.y - 1] = true; chunks_loading[p.x - 1][p.y - 1] = true;
		}
		else if (p.x < 7 && p.x == 0 && p.y < 7 && p.y > 0)
		{
			chunks_loading[p.x][p.y] = true; chunks_loading[p.x][p.y + 1] = true; chunks_loading[p.x + 1][p.y + 1] = true; chunks_loading[p.x + 1][p.y] = true; chunks_loading[p.x][p.y - 1] = true; chunks_loading[p.x + 1][p.y - 1] = true;
		}
	}
	
	cout << endl << " - - - - - - - - " << endl;
	for (int i = 0; i < 8; i++) 
	{
		cout << "|";
		for (int j = 0; j < 8; j++) // Prints the map line by line
		{
			if (j == p.x && i == p.y)	cout << "P" << "|"; // If player position is the same as print position prints 'P'
			else if (discovered[j][i] == true) cout << grid[j][i] << "|"; // If the player hasn't visited the cell at least once it can't be shown
			else if (chunks_loading[j][i] == true) cout << grid[j][i] << "|"; // Shows the loaded chunks
			else cout << "?" << "|";
		}
		cout << endl << " - - - - - - - - " << endl;
	}
}

void generateMap(char grid[8][8], int seed, char chestMap) 
{
	// Percentages
	int c_percentage = 10;
	int t_percentage = 10;
	int M_percentage = 10;
	int m_percentage = 10;
	
	int y_percentage = 5;
	int empty_percentage = 55; // Percentage of empty cell

	// Only one cell can contain the amulet and lantern
	bool y_generated = false;
	bool l_generated = false;
	bool m_generated = false;
	
	// Starts the generator
	while (!y_generated && !m_generated) // If Yendor Amulet and the Market are not generated it generates the map again
	{
		for (int i = 0; i < 8; i++) 
		{
			for (int j = 0; j < 8; j++) 
			{
				int random_num = rand() % 100 + 1; // Generates a number from 1 to 100
				if (random_num <= t_percentage) grid[i][j] = 'T';
				else if (random_num <= t_percentage + c_percentage) grid[i][j] = 'C';
				else if (random_num <= t_percentage + c_percentage + M_percentage) grid[i][j] = 'M';
				else if (random_num <= t_percentage + c_percentage + M_percentage + m_percentage && !m_generated)
				{
					grid[i][j] = 'm';
					m_generated = true;
				}
				else if (random_num <= t_percentage + c_percentage + M_percentage + m_percentage + y_percentage && !y_generated) 
				{
					grid[i][j] = 'Y';
					y_generated = true;
				} 
				else if (random_num <= t_percentage + c_percentage +  M_percentage + m_percentage + y_percentage + empty_percentage) grid[i][j] = ' ';
				else grid[i][j] = ' ';
			}
		}
	}
}

void gui(player p, char grid[8][8], bool discovered[8][8], int startx, int starty, int seed)
{
	string s;
	int l_percentage = 15;
	int g_percentage = 10;
	int h_percentage = 20;	
	int w_percentage = 20;
	bool l_generated = false;

	while (p.health > 0)
	{
		printMap(grid, p, discovered, startx, starty);
	
		cout << "\nCurrent cell: ";
		switch (grid[p.x][p.y])
		{
			case 'E':
				cout << "Exit\n";
				break;
			case 'M':
				cout << "Skeleton\n";
				break;
			case 'T':
				cout << "Trap\n";
				break;
			case 'Y':
				cout << "Yendor Amulet\n";
				break;
			case 'C':
				cout << "Chest\n";
				break;
			case 'b':
				cout << "Broken Bat\n";
				break;
			case 'B':
				cout << "Intact Bat\n";
				break;
			case 'S':
				cout << "Sword\n";
				break;
			case 'A':
				cout << "Double Axe\n";
				break;
			case 'P':
				cout << "Pistol\n";
				break;
			case 'm':
				cout << "Shop\n";
				break;
			default:
				cout << "Empty\n";
				break;
		}
		cout << "\nPlayer health: " << p.health;
	
		cout << "\n\nInsert new istruction: "; cin >> s;

		if (s == "w") // Moving to north
		{
			if (p.y != 0)
			{
				system("clear");
				p.y -=1;
				if (grid[p.x][p.y] == 'T')
				{
					if (discovered[p.x][p.y] == false) 
					{
						
						cout << "\nThe trap hitted you and damaged your life by 25 health!";
						p.health-=25;
						if (p.health <= 0)
						{
							cout << "\nOh no! The trap killed you!";
							ofstream file;
							file.open("history.txt", ios::app);
							file << "\nSeed: " << seed << "\nPlayer health: " << p.health << "\nPlayer gold: " << p.gold;
							break;
						}
					}
					else 
					{
						cout << "\nTrap already sprung";
					}
				}
				discovered[p.x][p.y] = true;
			}
			else 
			{
				cout << "\nYou can't move in this direction!";
			}
		}
		else if (s == "a") // Moving to west
		{
			system("clear");
			if (p.x != 0)
			{
				p.x -=1;
				if (grid[p.x][p.y] == 'T')
				{
					if (discovered[p.x][p.y] == false) 
					{
						cout << "\nThe trap hitted you and damaged your life by 25 health!";
						p.health-=25;
						if (p.health <= 0)
						{
							cout << "\nOh no! The trap killed you!";
							ofstream file;
							file.open("history.txt", ios::app);
							file << "\nSeed: " << seed << "\nPlayer health: " << p.health << "\nPlayer gold: " << p.gold;
							break;
						}
					}
					else 
					{
						cout << "\nTrap already sprung";
					}
				}
				discovered[p.x][p.y] = true;
			}
			else 
			{
				cout << "\nYou can't move in this direction!";
			}
		}
		else if (s == "s") // Moving to south
		{
			system("clear");
			if (p.y != 7)
			{
				p.y +=1;
				if (grid[p.x][p.y] == 'T')
				{
					if (discovered[p.x][p.y] == false) 
					{
						cout << "\nThe trap hitted you and damaged your life by 25 health!";
						p.health-=25;
						if (p.health <= 0)
						{
							cout << "\nOh no! The trap killed you!";
							ofstream file;
							file.open("history.txt", ios::app);
							file << "\nSeed: " << seed << "\nPlayer health: " << p.health << "\nPlayer gold: " << p.gold;
							break;
						}
					}
					else 
					{
						cout << "\nTrap already sprung";
					}
				}
				discovered[p.x][p.y] = true;
			}
			else 
			{
				cout << "\nYou can't move in this direction!";
			}
		}
		else if (s == "d") // Moving to east
		{
			system("clear");
			if (p.x != 7)
			{
				p.x +=1;
				if (grid[p.x][p.y] == 'T')
				{
					if (discovered[p.x][p.y] == false) 
					{
						cout << "\nThe trap hitted you and damaged your life by 25 health!";
						p.health-=25;
						if (p.health <= 0)
						{
							cout << "\nOh no! The trap killed you!";
							ofstream file;
							file.open("history.txt", ios::app);
							file << "\nSeed: " << seed << "\nPlayer health: " << p.health << "\nPlayer gold: " << p.gold;
							break;
						}
					}
					else 
					{
						cout << "\nTrap already sprung";
					}
				}
				discovered[p.x][p.y] = true;
			}
			else 
			{
				cout << "\nYou can't move in this direction!";
			}
		}
		else if (s == "attack")
		{
			if (grid[p.x][p.y] == 'M')
			{
				int mLife = 12;
				int takenDamage = 0;
				for (takenDamage = 0; mLife > 0; takenDamage += 2)
				{
					mLife -= weaponDamage(p);
				}
				p.health -= takenDamage;
				if (p.health <= 0)
				{
					p.health = 0;
					system("clear");
					cout << "\nThe skeleton killed you!";
					ofstream file;
					file.open("history.txt");
					file << "\nSeed: " << seed << "\nPlayer health: " << p.health << "\nPlayer gold: " << p.gold;
				}
				else 
				{
					p.gold += 5;
					system("clear");
					cout << "\nSkeleton killed! +5 gold";
					grid[p.x][p.y] = ' ';
				}
			}
			else
			{
				system("clear");
				cout << "\nThere is no skeleton in this cell";
			}
		}
		else if (s == "drink")
		{
			if (p.potionsNumber > 0)
			{
				if (p.health != 100)
				{
					p.health += 20;
					if (p.health > 100)
						p.health -= (p.health - 100);
					p.potionsNumber -= 1;
					system("clear");
					cout << "\nHealth restored!";
				}	
				else
				{
					system("clear");
					cout << "\nYou already have full health!";
				}
			}
			else
			{
				system("clear");
				cout << "\nYou have no potions in your inventory";
			}
		}
		else if (s == "grab")
		{
			if (grid[p.x][p.y] == 'Y')
			{
				system("clear");
				cout << "\nYou found the Yendor Amulet! Go to the exit to win the game";
				p.hasAmulet = true;
				grid[p.x][p.y] = ' ';
			}
			else
			{
				system("clear");
				cout << "\nThere is nothing to grab here";
			}
		}
		else if (s == "inventory")
		{
			system("clear");
			cout << "\nPlayer gold: " << p.gold << "\nPotions number: " << p.potionsNumber << "\nAmulet: " << p.hasAmulet << "\nLantern: " << p.hasLantern << "\nCurrent weapon: " << p.weapon;
		}
		else if (s == "exit")
		{
			if (grid[p.x][p.y] == 'E')
			{
				if (p.hasAmulet)
				{
					system("clear");
					cout << "\nCongratulations you won the game!";
					ofstream file;
					file.open("history.txt", ios::app);
					file << "\nSeed: " << seed << "\nPlayer health: " << p.health << "\nPlayer gold: " << p.gold;
					p.health = 0;
				}
				else
				{
					system("clear");
					cout << "\nYou can't exit without the Yendor Amulet! Go find it!"; 
				}
			}
			else
			{
				system("clear");
				cout << "\nYou can't exit if there is no door!";
			}
		}	
		else if (s == "open")
		{
			if (grid[p.x][p.y] == 'C')
			{
				int random = rand() % 100;
				if (random < h_percentage)
				{
					p.potionsNumber += 1;
					system("clear");
					cout << "\nYou found a health potions!";
					grid[p.x][p.y] = ' ';
				}
				else if (random < h_percentage + g_percentage)
				{
					p.gold += rand() % 10 + 5;
					system("clear");
					cout << "\nYou found some gold!";
					grid[p.x][p.y] = ' ';
				}
				else if (random < h_percentage + g_percentage + w_percentage)
				{
					if (random < h_percentage + g_percentage + 3)
					{
						system("clear");
						grid[p.x][p.y] = 'b';
					}
					else if (random < h_percentage + g_percentage + 6)
					{
						system("clear");
						grid[p.x][p.y] = 'B';
					}
					else if (random < h_percentage + g_percentage + 10)
					{
						system("clear");
						grid[p.x][p.y] = 'S';
					}
					else if (random < h_percentage + g_percentage + 15)
					{
						system("clear");
						grid[p.x][p.y] = 'A';
					}
					else if (random < h_percentage + g_percentage + 20)
					{
						system("clear");
						grid[p.x][p.y] = 'P';
					}
				}
				else if (random < h_percentage + g_percentage + w_percentage + l_percentage && !l_generated)
				{
					p.hasLantern = true;
					system("clear");
					cout << "\nYou found the lantern!";
					grid[p.x][p.y] = ' ';
					l_generated = true;
				}
				else 
				{
					system("clear");
					cout << "\nTrap chest!";
					grid[p.x][p.y] = 'M';
				}
			}
		}
		else if (s == "equip")
		{
			switch (grid[p.x][p.y])
			{
				case 'b':
					equip("broken bat", p);
					grid[p.x][p.y] = ' ';
					break;
				case 'B':
					equip("intact bat", p);
					grid[p.x][p.y] = ' ';
					break;
				case 'S':
					equip("sword", p);
					grid[p.x][p.y] = ' ';
					break;
				case 'A':
					equip("double axe", p);
					grid[p.x][p.y] = ' ';
					break;
				case 'P':
					equip("pistol", p);
					grid[p.x][p.y] = ' ';
					break;
				default:
					system("clear");
					cout << "\nNo weapon to equip in this location";
					break;
			}
		}
		else if (s == "enter")
		{
			if (grid[p.x][p.y] == 'm')
			{
				market(p, seed);
			}
			else cout << "\nYou can't enter there!";
		}
		else 
		{
			system("clear");
			cout << "\nThis command doesn't exist";
		}
	}
}

void setFalse(bool x[8][8])
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			x[i][j] = false;
		}
	}
}

void updates()
{
	ifstream file;
	string line;
	file.open("updates.txt");
	while (getline(file, line))
	{
		cout << line << endl;
	}
}


int weaponDamage(player p)
{
	if (p.weapon == "broken bat")
		return 5;
	else if (p.weapon == "intact bat")
		return 7;
	else if (p.weapon == "double axe")
		return 9;
	else if (p.weapon == "pistol")
		return 15;
	return 3;
}

void equip(string weapon, player &p)
{
	system("clear");
	cout << "\n" << weapon << " equiped";
	p.weapon = weapon;
} 

void market(player p, int seed)
{
	int choice = 0;
	market_item items;

	srand (seed);
	for (int i = 0; i < 7; i++)
	{
		int random_number = rand() % 100;
		if (random_number <= items.bb_percentage) items.items[i] = "Broken Bat"; 
		else if (random_number <= items.bb_percentage + items.ib_percentage) items.items[i] = "Intact Bat";
		else if (random_number <= items.bb_percentage + items.ib_percentage + items.dx_percentage) items.items[i] = "Double Axe";
		else if (random_number <= items.bb_percentage + items.ib_percentage + items.dx_percentage + items.p_percentage) items.items[i] = "Pistol";
		else if (random_number <= items.bb_percentage + items.ib_percentage + items.dx_percentage + items.p_percentage + items.hp_percentage) items.items[i] = "Health Potion";
		else if (random_number <= items.bb_percentage + items.ib_percentage + items.dx_percentage + items.p_percentage + items.hp_percentage + items.l_percentage) items.items[i] = "Lantern";
		else items.items[i] = "Yendor Amulet";
	}
	int prices[5];
	for (int i = 0; i < 5; i++)
	{
		if (items.items[i] == "Broken Bat") prices[i] = 4;
		else if (items.items[i] == "Intact Bat") prices[i] = 7;
		else if (items.items[i] == "Double Axe") prices[i] = 10;
		else if (items.items[i] == "Pistol") prices[i] = 15;
		else if (items.items[i] == "Health Potion") prices[i] = 5;
		else if (items.items[i] == "Lantern") prices[i] = 20;
		else if (items.items[i] == "Yendor Amulet") prices[i] = 40;
	}

	while (choice >= 0)
	{
		system("clear");
		cout << "\nAvaliable to shop: " << p.gold << " Gold\n";
		cout << "\n\t1) " << items.items[0] << "\n\t2) " << items.items[1] << "\n\t3) " << items.items[2] << "\n\t4) " << items.items[3] << "\n\t5) " << items.items[4];
		cout << "\nInsert number of choice (-1 to exit the shop): "; cin >> choice;
		switch (choice)
		{
			case 1: 
				if (p.gold >= prices[choice - 1])
				{
					p.gold -= prices[choice-1];
					p.weapon = "Broken Bat";
					cout << "\nBroken bat equipped";
				}
				else cout << "\nYou don't have enough gold";
				break;
			case 2: 
				if (p.gold >= prices[choice - 1])
				{
					p.gold -= prices[choice-1];
					p.weapon = "Intact Bat";
					cout << "\nIntact bat equipped";
				}
				else cout << "\nYou don't have enough gold";
				break;
			case 3: 
				if (p.gold >= prices[choice - 1])
				{
					p.gold -= prices[choice-1];
					p.weapon = "Double Axe";
					cout << "\nDouble axe equipped";
				}
				else cout << "\nYou don't have enough gold";
				break;
			case 4: 
				if (p.gold >= prices[choice - 1])
				{
					p.gold -= prices[choice-1];
					p.weapon = "Pistol";
					cout << "\nPistol equipped";
				}
				else cout << "\nYou don't have enough gold";
				break;
			case 5: 
				if (p.gold >= prices[choice - 1])
				{
					p.gold -= prices[choice-1];
					p.potionsNumber++;
					cout << "\nPotion added";
				}
				else cout << "\nYou don't have enough gold";
				break;
			case 6: 
				if (!p.hasLantern)
				{
					if (p.gold >= prices[choice - 1])
					{
						p.gold -= prices[choice-1];
						p.hasLantern = true;
					}
					else cout << "\nYou don't have enough gold";
				}
				else cout << "\nYou already have Lantern!";
				break;
			case 7: 
				if (!p.hasAmulet)
				{
					if (p.gold >= prices[choice - 1])
					{
						p.gold -= prices[choice-1];
						p.hasAmulet = true;
					}
					else cout << "\nYou don't have enough gold";
				}
				else cout << "\nYou already have the Yendor Amulet!";
				break;
		}
	}
	system("clear");
}

	
