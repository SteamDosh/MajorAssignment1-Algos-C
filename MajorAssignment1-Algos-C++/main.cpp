#include <iostream>									//i/o stream
#include <string>									//string lib
#include <fstream>									//i/o stream
#include <cctype>									//char handling
#include <iomanip>									//text formatting

using namespace std;

//**********************************************************
//*****           Classes and Structs            ***********
//**********************************************************
class node
{
public:
													//We use a node for the BST (pointers to left & right), this also makes it easier to handle the data
	int value;
	string word;

	node* ptrRight;
	node* ptrLeft;

	node(int aVal, string aWord) { value = aVal; word = aWord; }
	void clear() { ptrRight = NULL; ptrLeft = NULL; }
	void setPtr(node* aLeft, node* aRight) { ptrRight = aRight; ptrLeft = aLeft; }
};

struct foundResult 									//A simple struct to evaluate arrWords and arrFreq to return two values;
{
	int index;										//if true use the current index, false will return the highest index
	bool aVal;										//true means it exists, false does nott
};
//**********************************************************
//*****              Global Variables            ***********
//**********************************************************

const int MAXWORDS = 50000;		
int maxIndex = 0;									//Used so we dont have to loop 50,000 times
int arrFreq[MAXWORDS] = {};
int printProg = 0;
string arrWords[MAXWORDS] = {};
node* rootNode = NULL;							//Root of BST

//**********************************************************
//*****            BST Functionality             ***********
//**********************************************************

string fixWord(string aWord)						//Function remove Punctuation and checks if each character is alphabetic
{
	string newWord;
	for (int i = 0; i < aWord.size(); i++)
	{
		if (isalpha(aWord[i])) newWord += tolower(aWord[i]);
	}
	return newWord;
}

foundResult findWord(string aWord)					//Function to find the frequency of words before forming a BST for sorting.
//Because of the banshee compiler I had to make this extra ugly... (Why?)
//ie I have to do 
//foundResult result = { index, false };
//return result;
//Instead of return foundResult {index, false};
{
	for (int index = 0; index < MAXWORDS; index++)
	{
		if (arrWords[index] == "") 
		{ 
			foundResult result = { index, false };
			return result;
		};

		if (arrWords[index] == aWord) 
		{ 
			foundResult result = { index, true };
			return result;
		};
	}
	foundResult result = { 0, false };
	return result;
}

void insertNode(node* aNode)						//Function to organize and sort the BST by frequency and Alphabetic 
//(Probably should've split this into numerous functions however I'd need to keep track of prevNode, and by call itself it makes it difficult
{
	if (rootNode == NULL)						//If we have no root, create a rootnode
	{
		rootNode = aNode;
		return;
	}

	node* prevNode = NULL;
	node* node = rootNode;

	for (;;)										//Continous loop until the node finds a home
	{
		if (aNode->value == node->value)			//Alphabetic sort once frequency is same
		{
			bool side = prevNode->ptrLeft == node;	//true == ptrLeft, false == ptrRight
			if (aNode->word < node->word)			//if node should be pushed above current node
			{
				if (side)							//if previousNode was pointing to current node on left side
				{
					prevNode->ptrLeft = aNode;
					aNode->setPtr(node->ptrLeft, node->ptrRight);
					node->clear();
					insertNode(node);				//Put the current node back into sort (easier than manually shifting up/down x places)
					break;
				}
				else								//if previousNode was pointing to current node on right side
				{

					prevNode->ptrRight = aNode;
					aNode->setPtr(node->ptrLeft, node->ptrRight);
					node->clear();
					insertNode(node);				//Put the current node back into sort (easier than manually shifting up/down x places)
					break;
				}
			}
			else									//if node word should be pushed back
			{
				if (node->ptrLeft)					//Keep looking downwards until we hit a node we can fit into
				{
					prevNode = node;
					node = node->ptrLeft;
				}
				else
				{
					node->ptrLeft = aNode;			//We hit the end, I guess we go here
					break;
				}
			}
		}
		else if (aNode->value < node->value)		//if less than
		{
			if (node->ptrLeft == NULL)			//We hit the end, I guess we go here
			{
				node->ptrLeft = aNode;
				break;
			}
			else									//Keep looking downwards until we hit a node we can fit into
			{
				prevNode = node;
				node = node->ptrLeft;
			}
		}
		else if (aNode->value > node->value)		//if greater than
		{
			if (node->ptrRight == NULL)			//We hit the end, I guess we go here
			{
				node->ptrRight = aNode;
				break;
			}
			else									//Keep looking downwards until we hit a node we can fit into
			{
				prevNode = node;
				node = node->ptrRight;
			}
		}
	}
}

void buildTree()
{
	cout << "Building tree" << endl;
	for (int i = 0; i < MAXWORDS; i++)				//Place everything in our frequency array into a BST sort
	{
		if (arrFreq[i] == 0) break;
		insertNode(new node(arrFreq[i], arrWords[i]));
	}
	cout << "Finished Building tree" << endl;
}

//**********************************************************
//*****                  Print BST               ***********
//**********************************************************

node* getBSTArray(node* aNode, node* aArray[MAXWORDS])
{
	if (aNode == NULL) aNode = rootNode; 	//this is so you can call getBSTArray(nullptr,arr[]) from anywhere

	if (aNode->ptrRight != NULL) getBSTArray(aNode->ptrRight, aArray);
	aArray[printProg++] = aNode;
	if (aNode->ptrLeft != NULL) getBSTArray(aNode->ptrLeft, aArray);

	return *aArray;
}

void printBST(int aVal)
{	
	node* tempArr[MAXWORDS] = {};
	tempArr[MAXWORDS-1]= getBSTArray(NULL, tempArr);
	
	int size = 0;
	while (tempArr[size]) { size++; }

	cout << "Printing first 10 Ascending." << endl;
	for (int i = 0; i < aVal; i++)
	{
		cout << setw(10) << tempArr[i]->word << setw(20) << tempArr[i]->value << endl;
	}

	cout << endl << "Printing last 10." << endl;
	for (int i = size-aVal; i < size; i++)
	{
		cout << setw(10) << tempArr[i]->word << setw(20) << tempArr[i]->value << endl;
	}
}

//**********************************************************
//*****                Reading File              ***********
//**********************************************************
void ReadFile(string aFileName)
{
	ifstream fin(aFileName.c_str());

	if (!fin.good())
	{
		cerr << "Could not open data file!\n";
		return;
	}

	int index = 0;
	for (string word; fin >> word; index++)
	{
		word = fixWord(word);						//Returns with no punc & islower
		foundResult result = findWord(word);		//Returns position and if word exists
		arrFreq[result.index]++;

		if (!result.aVal)							//Fill data if word doesnt exist.
		{
			arrWords[result.index] = word;
			maxIndex = result.index;
		}

	}

	fin.close();

	cout << "Finished loading file" << endl;
}

void SelectFile()
{
	cout << "\nEnter the name of the file (ie words.txt)" << endl;
	string input;
	cin >> input;

	ReadFile(input);
}

//**********************************************************
//*****            Interface and Menu            ***********
//**********************************************************
char Menu()
{
	char Cmd;
	cout << endl << "*********************************" << endl;
	cout << "*     Assignment 1              *" << endl;
	cout << "*     (r)ead file               *" << endl;
	cout << "*     (b)uild tree              *" << endl;
	cout << "*     (d)isplay tree            *" << endl;
	cout << "*     (q)uit                    *" << endl;
	cout << "*********************************" << endl;
	cout << "Command: ";
	cin >> Cmd;
	cin.ignore();  // eat up '\n'
	Cmd = tolower(Cmd);
	return Cmd;
}

int main()
{
	bool Quit = false;

	do
	{
		char Cmd = Menu();
		switch (Cmd)
		{
		case 'r':
			SelectFile();
			break;
		case 'b':
			buildTree();
			break;
		case 'd':
			printBST(10);			
			break;
		case 'q':
			Quit = true;
			break;
		default:
			cerr << "Invalid command!\n";
		}
	} while (!Quit);

	return 0;
}
