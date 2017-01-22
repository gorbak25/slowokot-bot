#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <vector>

using namespace std;

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
	Node* getNext(wchar_t c)
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
				loaded_word.push_back(chr);
			}
			trie_root.AddWord(loaded_word);
		}
	}
	else
		cerr << "Could not load the dictionary" << endl;
}

char board[4][4] = {{'L','A','J','I'}, 
					{'K','C','O','N'}, 
					{'Z','I','R','A'},
					{'J','U','S','Z'}};
bool visited[4][4];
int nums = 0;
void DFS(int x, int y, string& head)
{
	if(x==-1 || x>3 || y==-1 || y>3 || head.size()>16)
		return;

	head.push_back(board[y][x]);
	for(int i = -1; i<2; i++)
	{
		for(int j = -1; j<2; j++)
		{
			if(!visited[y+j][x+i])
			{
				visited[y+j][x+i] = true;
				DFS(x+i, y+j, head);
				visited[y+j][x+i] = false;
			}
		}
	}
	//if(dict->check(head))
	//{
		cerr << head << endl;
	//}
	nums++;
	head.pop_back();
	
}

int main()
{
	cerr << "Loading dictionary" << endl;
	loadDictionary("pl_wordlist");
	cerr << "Succesfully loaded dictionary" << endl;

	string str;
	for(int x = 0; x<4; x++)
		for(int y = 0; y<4; y++)
		{
			visited[y][x] = true;
			DFS(x,y,str);
			visited[y][x] = false;
		}
	cout << nums << endl;
	return 0;
}
