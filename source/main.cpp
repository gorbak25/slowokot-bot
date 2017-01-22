#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

const int rowX[4] = {75, 180, 300, 400};

const int rowY[4] = {400, 500, 600, 730};

void StartTouch()
{
	system("adb shell sendevent /dev/input/event0 3 57 14");
}

void GoToField(int x, int y)
{
	if(x<0 || x>3 || y<0 || y>3)
		return;

	system(("adb shell sendevent /dev/input/event0 3 53 " + to_string(rowX[x])).c_str());
	system(("adb shell sendevent /dev/input/event0 3 54 " + to_string(rowY[y])).c_str());
	system("adb shell sendevent /dev/input/event0 3 58 57");
	system("adb shell sendevent /dev/input/event0 0 0 0");
}

void FinishTouch()
{
	system("adb shell sendevent /dev/input/event0 3 57 4294967295");
	system("adb shell sendevent /dev/input/event0 0 0 0");
}

class Node
{
public:
	bool isEnding;
	char chr;
	vector<Node*> next;

	Node():isEnding(false), chr(0){}
	~Node()
	{
		for(auto ptr : next)
			delete ptr;
	}
	Node* getNext(char c)
	{
		for(auto ptr : next)
			if(ptr->chr == c)
				return ptr;
		return nullptr;
	}
	void AddWord(const string& str, int pos = 0)
	{
		if(pos >= str.size()) return;

		Node* ptr = getNext(str[pos]);
		if(ptr == nullptr)
		{
			Node* newNode = new Node();
			newNode->chr = str[pos];
			if(pos == str.size()-1)
				newNode->isEnding = true;
			else
				newNode->AddWord(str, pos+1);
			next.push_back(newNode);
		}
		else
		{
			if(pos < str.size()-1)
				ptr->AddWord(str, pos+1);
			else
				ptr->isEnding = true;
		}
	}

	bool contains(const string& str, int pos = 0)
	{
		Node* ptr = getNext(str[pos]);
		if(ptr == nullptr)
			return false;
		else if(str.size()-1 == pos)
		{
			if(ptr->isEnding)
				return true;
			else return false;
		}
		else
			return ptr->contains(str, pos+1);
	}
};

Node trie_root;

void loadDictionary(const string& filename)
{
	ifstream file;
	file.open(filename, ios::binary);

	if(file.good())
	{
		while(!file.eof())
		{
			string loaded_word;
			char chr;
			for(;;)
			{
				if(file.eof())
					break;
				file.read(&chr, 1);
				if(chr == '\n')
					break;
				if(chr == '/')
				{
					//ignore the rest
					do
					{
						file.read(&chr, 1);
					}while(chr != '\n');

					break;
				}
				loaded_word.push_back(tolower(chr));
			}
			trie_root.AddWord(loaded_word);
			//cerr << loaded_word << endl;
		}
	}
	else
		cerr << "Could not load the dictionary" << endl;
}

unsigned short int board[4][4] = {{'b','n','y','u'}, 
				     			  {'a','l','ć','m'}, 
				     			  {'p','n','s','ź'},
				     			  {'e','r','o','e'}};
class SearchResult
{
public:
	string found;
	vector<pair<int,int>> cordinates;

	SearchResult(string str, vector<pair<int,int>> cords):found(str), cordinates(cords)
	{
	}
};

vector<SearchResult> results;
bool visited[4][4];
void DFS(int x, int y, string& head, vector<pair<int,int>>& cords)
{
	if(x==-1 || x>3 || y==-1 || y>3)
		return;

	if(board[y][x] > 255) //UTF-8
	{
		head.push_back(char((board[y][x]&0xff00)>>8));
		head.push_back(char((board[y][x]&0xff)));
	}
	else
		head.push_back(char(board[y][x]));
	cords.push_back(make_pair(x,y));

	for(int i = -1; i<2; i++)
	{
		for(int j = -1; j<2; j++)
		{
			if(!visited[y+j][x+i])
			{
				visited[y+j][x+i] = true;
				DFS(x+i, y+j, head, cords);
				visited[y+j][x+i] = false;
			}
		}
	}
	if(trie_root.contains(head))
	{
		if(head.size() > 2)
		{
			results.push_back(SearchResult(head, cords));
		}
	}
	if(board[y][x] > 255)
	{
		head.pop_back();
	}
	head.pop_back();
	cords.pop_back();
	
}

int main()
{
	StartTouch();
	FinishTouch();
	StartTouch();
	FinishTouch();
	cerr << "Loading dictionary" << endl;
	loadDictionary("pl_wordlist");
	cerr << "Succesfully loaded dictionary" << endl;

	string str;
	std::vector<pair<int,int>> cords;
	for(int x = 0; x<4; x++)
		for(int y = 0; y<4; y++)
		{
			visited[y][x] = true;
			DFS(x,y,str,cords);
			visited[y][x] = false;
		}

		sort(results.begin(), 
			results.end(),
			[](const SearchResult& a, const SearchResult& b)->bool
			{
				return a.found.size() > b.found.size();
			});

		for(auto res : results)
		{
			cout << res.found << '\n';
			StartTouch();

			for(auto cord : res.cordinates)
			{
				GoToField(cord.first, cord.second);
			}

			FinishTouch();
		}
		cout << flush;

	return 0;
}
