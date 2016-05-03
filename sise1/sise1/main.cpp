#include <iostream>
#include <math.h>
#include <map>
#include <queue>
#include <string>
#include <sstream>

#define w 4
#define h 4
#define maxMoves 15

using namespace std;

int states = 0;
int maxDepth = 0;
int steps = 0;

struct Visit;
struct State;

uint64_t Get(int index, uint64_t code);
uint64_t Set(int index, uint64_t code, uint64_t value);
uint64_t Swap(uint64_t code, int x0, int y0, int x2, int y2);
bool WasVisited(map< uint64_t, Visit >& visited, uint64_t code);

int GetScore(uint64_t code);
char GetMove(uint64_t before, uint64_t after);


struct Visit
{
	uint64_t value;
	int moves;

	bool operator == (const uint64_t &b) const { return value == b; }
	bool operator < (const Visit &b) const { return value > b.value; }

	Visit(){}
	Visit(uint64_t v, int m) : value(v), moves(m) {}
};


struct State
{
	uint64_t value;
	uint64_t lastVal;
	int x0;
	int y0;
	int moves;
	int score;

	bool operator < (const State &b) const { return score > b.score; }

	State(uint64_t val, uint64_t lVal, int x, int y, int m = 0) : value(val), lastVal(lVal), x0(x), y0(y), moves(m)
	{
		score = GetScore(val);
	}
};


uint64_t Get(int index, uint64_t code) { return ( code & (0xf000000000000000L >> (index * 4)) ) >> ( (15 - index) * 4 ); }
uint64_t Set(int index, uint64_t code, uint64_t value) { return ( code & (~(0xf000000000000000L >> (index * 4))) ) + (value << ((15 - index) * 4)); }
uint64_t Swap(uint64_t code, int x0, int y0, int x2, int y2) { return Set(x2 + y2 * w, Set(x0 + y0 * w, code, Get(x2 + y2 * w, code)), 0); }
bool WasVisited(map< uint64_t, Visit >& visited, uint64_t code) { return visited.find(code) != visited.end(); }


int GetScore(uint64_t code)
{
	static int desX, desY, result, i;
	result = 0;

	for (i = 0; i < 16; ++i)
	{
		desX = Get(i, code) / w;
		desY = Get(i, code) % w;
		result += abs(desX - i / w) + abs(desY - i%w);
	}

	return result;
}


char GetMove(uint64_t before, uint64_t after)
{
	int x0, y0;
	uint64_t tmp;

	for (int i = 0; i < 16; ++i)
	{
		if (!Get(i, before))
		{
			x0 = i % 4;
			y0 = i / 4;
			break;
		}
	}

	if (x0 > 0 && Swap(before, x0, y0, x0 - 1, y0) == after)
		return 'P'; //Prawo
	if (x0 < w - 1 && Swap(before, x0, y0, x0 + 1, y0) == after)
		return 'L'; //Lewo
	if (y0 > 0 && Swap(before, x0, y0, x0, y0 - 1) == after)
		return 'D'; //Dó³
	if (y0 < h - 1 && Swap(before, x0, y0, x0, y0 + 1) == after)
		return 'G'; //Góra

	return 0;		//B³¹d
}


void Display(uint64_t code)
{
	cout << "\n";
	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
			cout << hex << Get(i * w + j, code) << " ";
		cout << "\n";
	}
}


int DisplayHistory(map< uint64_t, Visit >& visited, uint64_t code, int length = 0)
{
	uint64_t tmp = visited[code].value;
	if (tmp)
		length = DisplayHistory(visited, tmp, length + 1);
	Display(code);
	return length;
}


void DisplayMoves(map< uint64_t, Visit >& visited, uint64_t code)
{
	uint64_t next = visited[code].value;
	if (next)
	{
		DisplayMoves(visited, next);
		cout << GetMove(code, next);
	}
}


bool DFS(map<uint64_t, Visit>& visited, uint64_t value, uint64_t lastVal, int x0, int y0, int moves = 0)
{
	if (moves > maxMoves || (WasVisited(visited, value) && visited[value].moves <= moves)) return false;

	++states;
	maxDepth = (moves > maxDepth) ? moves : maxDepth;

	visited[value] = Visit( lastVal, moves );

	if (GetScore(value) == 0) return true;

	bool result = false;

	if (x0 > 0)
		result = DFS(visited, Swap(value, x0, y0, x0 - 1, y0), value, x0 - 1, y0, moves + 1);
	if (!result && x0 < 3)
		result = DFS(visited, Swap(value, x0, y0, x0 + 1, y0), value, x0 + 1, y0, moves + 1);
	if (!result && y0 > 0)
		result = DFS(visited, Swap(value, x0, y0, x0, y0 - 1), value, x0, y0 - 1, moves + 1);
	if (!result && y0 < 3)
		result = DFS(visited, Swap(value, x0, y0, x0, y0 + 1), value, x0, y0 + 1, moves + 1);

	return result;
}


bool BFS(map<uint64_t, Visit>& visited, queue<State>& que)
{
	State n = que.front();
	que.pop();
	
	if (n.moves > maxMoves || WasVisited(visited, n.value)) return false;

	++states;
	maxDepth = (n.moves > maxDepth) ? n.moves : maxDepth;
	visited[n.value] = Visit( n.lastVal, n.moves );

	if (GetScore(n.value) == 0) return true;

	if (n.x0 > 0)
			que.push(State(Swap(n.value, n.x0, n.y0, n.x0 - 1, n.y0), n.value, n.x0 - 1, n.y0, n.moves + 1));
	if (n.x0 < w - 1)
			que.push(State(Swap(n.value, n.x0, n.y0, n.x0 + 1, n.y0), n.value, n.x0 + 1, n.y0, n.moves + 1));
	if (n.y0 > 0)
			que.push(State(Swap(n.value, n.x0, n.y0, n.x0, n.y0 - 1), n.value, n.x0, n.y0 - 1, n.moves + 1));
	if (n.y0 < h - 1)
			que.push(State(Swap(n.value, n.x0, n.y0, n.x0, n.y0 + 1), n.value, n.x0, n.y0 + 1, n.moves + 1));
	return false;
}


bool A(map<uint64_t, Visit>& visited, priority_queue<State>& que)
{
	State n = que.top();
	que.pop();

	if (n.moves > maxMoves || WasVisited(visited, n.value)) return false;

	++states;
	maxDepth = (n.moves > maxDepth) ? n.moves : maxDepth;
	visited[n.value] = Visit(n.lastVal, n.moves);

	if (GetScore(n.value) == 0) return true;
	static uint64_t tmp;

	if (n.x0 > 0)
			que.push(State(Swap(n.value, n.x0, n.y0, n.x0 - 1, n.y0), n.value, n.x0 - 1, n.y0, n.moves + 1));
	if (n.x0 < w - 1)
			que.push(State(Swap(n.value, n.x0, n.y0, n.x0 + 1, n.y0), n.value, n.x0 + 1, n.y0, n.moves + 1));
	if (n.y0 > 0)
			que.push(State(Swap(n.value, n.x0, n.y0, n.x0, n.y0 - 1), n.value, n.x0, n.y0 - 1, n.moves + 1));
	if (n.y0 < h - 1)
			que.push(State(Swap(n.value, n.x0, n.y0, n.x0, n.y0 + 1), n.value, n.x0, n.y0 + 1, n.moves + 1));
	return false;
}


int main(int argc, char* argv[])
{
	string choice;
	stringstream convert(argv[1]);
	if (!(convert >> choice))
	{
		cout << "\nERROR\npossible arguments: dfs / bfs / a";
		return -1;
	}
	
	map< uint64_t, Visit > visited;
	uint64_t tmp;
	uint64_t data = 0;
	int x0, y0;

	cin >> tmp >> tmp;
	
	for (int i = 0; i < 16; ++i)
	{
		cin >> tmp;
		tmp <<= (15 - i) * 4;
		data += tmp;

		if (tmp == 0)
		{
			x0 = i % w;
			y0 = i / w;
		}
	}


	if (choice == "dfs")
	{
		DFS(visited, data, 0L, x0, y0);
	}
	else if (choice == "bfs")
	{
		queue<State> que;
		que.push(State(data, 0x0L, x0, y0, 0));
		while (BFS(visited, que) == false && que.empty() == false) { ; }
	}
	else if (choice == "a")
	{
		priority_queue<State> pQue;
		pQue.push(State(data, 0x0L, x0, y0, 0));
		while (A(visited, pQue) == false && pQue.empty() == false) { ; }
	}
	else
	{
		cout << "\nERROR\npossible arguments: dfs / bfs / a";
		return -1;
	}

	steps = DisplayHistory(visited, 0x123456789abcdef);
	cout << "Ruchy: ";
	DisplayMoves(visited, 0x123456789abcdef);

	cout << dec << "\nilosc przetworzonych stanow: " << states;
	cout << dec << "\nmaksymalna glebokosc rekursji: " << maxDepth;
	cout << dec << "\nilosc wykonanych krokow: " << steps;

	//Display(data);
	//DisplayMoves(visited, 0x123456789abcdef);

	return 0;
}

// example input
// 4 4 1 5 2 3 4 0 6 7 8 9 10 11 12 13 14 15
// 4 4 1 2 3 0 4 5 6 7 8 9 10 11 12 13 14 15
// 4 4 1 2 3 7 4 5 6 11 8 9 10 15 12 13 14 0

// 4 4 
//4 1 2 3
//5 6 10 7
//12 8 14 11
//9 0 13 15

// 4 4 0 4 3 6 8 1 5 2 13 10 14 7 9 12 15 11