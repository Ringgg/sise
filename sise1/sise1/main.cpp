#include <iostream>
#include <math.h>
#include <map>
#include <queue>

#define w 4
#define h 4

using namespace std;


uint64_t Get(int index, uint64_t code) { return ( code & (0xf000000000000000L >> (index * 4)) ) >> ( (15 - index) * 4 ); }
uint64_t Set(int index, uint64_t code, uint64_t value) { return ( code & (~(0xf000000000000000L >> (index * 4))) ) + (value << ((15 - index) * 4)); }

bool WasVisited(map< uint64_t, uint64_t >& visited, uint64_t code) { return visited.find(code) != visited.end(); }


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


void DisplayHistory(map< uint64_t, uint64_t >& visited, uint64_t code)
{
	Display(code);
	code = visited[code];
	if (code)
		DisplayHistory(visited, code);
}


uint64_t Swap(uint64_t code, int x0, int y0, int x2, int y2)
{
	return Set(x2 + y2 * w, Set(x0 + y0 * w, code, Get(x2 + y2 * w, code)), 0);
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


bool DFS(map<uint64_t, uint64_t>& visited, uint64_t value, uint64_t lastVal, int x0, int y0, int moves = 0)
{
	if (moves >= 15) return false;
	visited[lastVal] = value;
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



int main()
{
	map< uint64_t, uint64_t > visited;
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
			x0 = i / w;
			y0 = i % w;
		}
	}

	DFS(visited, data, 0L, x0, y0);
	DisplayHistory(visited, data);

	cin >> tmp >> tmp;
	return 0;
}

// example input
// 0 0 1 5 2 3 4 0 6 7 8 9 10 11 12 13 14 15