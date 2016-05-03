#include <iostream>
#include <math.h>
#include <map>
#include <queue>

#define w 4
#define h 4
#define maxMoves 15

using namespace std;

int states = 0;
int maxDepth = 0;
int steps = 0;

uint64_t Get(int index, uint64_t code) { return ( code & (0xf000000000000000L >> (index * 4)) ) >> ( (15 - index) * 4 ); }
uint64_t Set(int index, uint64_t code, uint64_t value) { return ( code & (~(0xf000000000000000L >> (index * 4))) ) + (value << ((15 - index) * 4)); }
uint64_t Swap(uint64_t code, int x0, int y0, int x2, int y2) { return Set(x2 + y2 * w, Set(x0 + y0 * w, code, Get(x2 + y2 * w, code)), 0); }
bool WasVisited(map< uint64_t, uint64_t >& visited, uint64_t code) { return visited.find(code) != visited.end(); }


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
		return 'D'; //Dó³
	if (x0 < w - 1 && Swap(before, x0, y0, x0 + 1, y0) == after)
		return 'G'; //Góra
	if (y0 > 0 && Swap(before, x0, y0, x0, y0 - 1) == after)
		return 'P'; //Prawo
	if (y0 < h - 1 && Swap(before, x0, y0, x0, y0 + 1) == after)
		return 'L'; //Lewo

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


int DisplayHistory(map< uint64_t, uint64_t >& visited, uint64_t code, int length = 0)
{
	Display(code);
	code = visited[code];
	if (code)
		return DisplayHistory(visited, code, length + 1);
	return length;
}


void DisplayMoves(map< uint64_t, uint64_t >& visited, uint64_t code)
{
	uint64_t next = visited[code];
	if (next)
	{
		cout << GetMove(code, next);
		DisplayMoves(visited, next);
	}
}


int GetScore(uint64_t code)
{
	static int desX, desY, result, i;

	result = 0;

	for (i = 0; i < 16; ++i)
	{
		desX = Get(i, code) / w;
		desY = Get(i, code) % w;
		result += abs(desX - i/w) + abs(desY - i%w);
	}

	return result;
}


struct State
{
	uint64_t value;
	uint64_t lastVal;
	int x0;
	int y0;
	int moves;
	int score;

	bool operator < (const State &b) const { return score > b.score; }
	bool operator == (const uint64_t &b) const { return value == b; }

	State(uint64_t val, uint64_t lVal, int x, int y, int m = 0) : value(val), lastVal(lVal), x0(x), y0(y), moves(m)
	{
		score = GetScore(val);
	}
};


bool DFS(map<uint64_t, uint64_t>& visited, uint64_t value, uint64_t lastVal, int x0, int y0, int moves = 0)
{
	++states;
	maxDepth = (moves > maxDepth) ? moves : maxDepth;

	if (moves >= maxMoves) return false;
	visited[value] = lastVal;
	if (GetScore(value) == 0) return true;

	static uint64_t tmp;

	bool result = false;

	if (x0 > 0)
	{
		tmp = Swap(value, x0, y0, x0 - 1, y0);
		if (!WasVisited(visited, tmp))
			result = result || DFS(visited, tmp, value, x0 - 1, y0, moves + 1);
	}
	if (!result && x0 < w - 1)
	{
		tmp = Swap(value, x0, y0, x0 + 1, y0);
		if (!WasVisited(visited, tmp))
			result = result || DFS(visited, tmp, value, x0 + 1, y0, moves + 1);
	}
	if (!result && y0 > 0)
	{
		tmp = Swap(value, x0, y0, x0, y0 - 1);
		if (!WasVisited(visited, tmp))
			result = result || DFS(visited, tmp, value, x0, y0 - 1, moves + 1);
	}
	if (!result && y0 < h - 1)
	{
		tmp = Swap(value, x0, y0, x0, y0 + 1);
		if (!WasVisited(visited, tmp))
			result = result || DFS(visited, tmp, value, x0, y0 + 1, moves + 1);
	}

	return result;
}


bool BFS(map<uint64_t, uint64_t>& visited, queue<State>& que)
{
	State n = que.front();
	que.pop();

	++states;
	maxDepth = (n.moves > maxDepth) ? n.moves : maxDepth;

	if (n.moves >= maxMoves) return false;

	visited[n.value] = n.lastVal;
	if (GetScore(n.value) == 0) return true;

	static uint64_t tmp;

	if (n.x0 > 0)
	{
		tmp = Swap(n.value, n.x0, n.y0, n.x0 - 1, n.y0);
		if (!WasVisited(visited, tmp))
			que.push(State(tmp, n.value, n.x0 - 1, n.y0, n.moves + 1));
	}
	if (n.x0 < w - 1)
	{
		tmp = Swap(n.value, n.x0, n.y0, n.x0 + 1, n.y0);
		if (!WasVisited(visited, tmp))
			que.push(State(tmp, n.value, n.x0 + 1, n.y0, n.moves + 1));
	}
	if (n.y0 > 0)
	{
		tmp = Swap(n.value, n.x0, n.y0, n.x0, n.y0 - 1);
		if (!WasVisited(visited, tmp))
			que.push(State(tmp, n.value, n.x0, n.y0 - 1, n.moves + 1));
	}
	if (n.y0 < h - 1)
	{
		tmp = Swap(n.value, n.x0, n.y0, n.x0, n.y0 + 1);
		if (!WasVisited(visited, tmp))
			que.push(State(tmp, n.value, n.x0, n.y0 + 1, n.moves + 1));
	}
	return BFS(visited, que);
}


bool A(map<uint64_t, uint64_t>& visited, priority_queue<State>& que)
{
	State n = que.top();
	que.pop();

	++states;
	maxDepth = (n.moves > maxDepth) ? n.moves : maxDepth;

	if (n.moves >= maxMoves) return false;

	visited[n.value] = n.lastVal;

	if (GetScore(n.value) == 0) return true;
	static uint64_t tmp;

	if (n.x0 > 0)
	{
		tmp = Swap(n.value, n.x0, n.y0, n.x0 - 1, n.y0);
		if (!WasVisited(visited, tmp))
			que.push(State(tmp, n.value, n.x0 - 1, n.y0, n.moves + 1));
	}
	if (n.x0 < w - 1)
	{
		tmp = Swap(n.value, n.x0, n.y0, n.x0 + 1, n.y0);
		if (!WasVisited(visited, tmp))
			que.push(State(tmp, n.value, n.x0 + 1, n.y0, n.moves + 1));
	}
	if (n.y0 > 0)
	{
		tmp = Swap(n.value, n.x0, n.y0, n.x0, n.y0 - 1);
		if (!WasVisited(visited, tmp))
			que.push(State(tmp, n.value, n.x0, n.y0 - 1, n.moves + 1));
	}
	if (n.y0 < h - 1)
	{
		tmp = Swap(n.value, n.x0, n.y0, n.x0, n.y0 + 1);
		if (!WasVisited(visited, tmp))
			que.push(State(tmp, n.value, n.x0, n.y0 + 1, n.moves + 1));
	}
	return A(visited, que);
}


int main()
{
	map< uint64_t, uint64_t > visited;
	uint64_t tmp;
	uint64_t data = 0;
	int choice;
	int x0, y0;
	cout << "	PIETNASTKA \nwybierz algorytm: \n1. DFS \n2. BFS \n3. A*\n";
	cin >> choice;
	cout << "\n\nWprowadz wartosci 16 pol ukladanki: ";
	
	for (int i = 0; i < 16; ++i)
	{
		cin >> tmp;
		tmp <<= (15 - i) * 4;
		data += tmp;

		if (tmp == 0)
		{
			x0 = i / w;
			y0 = i % w;
		}
	}

	if (choice == 1)
	{
		DFS(visited, data, 0L, x0, y0);
	}
	if (choice == 2)
	{
		queue<State> que;
		que.push(State(data, 0x0L, x0, y0, 0));
		BFS(visited, que);
	}
	if (choice == 3)
	{
		priority_queue<State> pQue;
		pQue.push(State(data, 0x0L, x0, y0, 0));
		A(visited, pQue);
	}

	steps = DisplayHistory(visited, 0x123456789abcdef);

	cout << "\n";

	DisplayMoves(visited, 0x123456789abcdef);

	cout << dec << "\nilosc przetworzonych stanow: " << states;
	cout << dec << "\nmaksymalna glebokosc rekursji: " << maxDepth;
	cout << dec << "\nilosc wykonanych krokow: " << steps;

	cin >> tmp;
	return 0;
}

// example input
// 1 1 5 2 3 4 0 6 7 8 9 10 11 12 13 14 15
