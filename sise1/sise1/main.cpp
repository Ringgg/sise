#include <iostream>
#include <math.h>
#include <set>

using namespace std;

int w = -1, k = -1;

bool Contains(set<uint64_t>& s, uint64_t value)
{
	return s.find(value) != s.end();
}

short GetScore(short **tab)
{
	short desX, desY, result = 0;

	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < k; ++y)
		{
			desX = tab[x][y] / w;
			desY = tab[x][y] % w;
			result += abs(desX - x) + abs(desY - y);
			//cout << "\n" << abs(desX - x) + abs(desY - y);
		}
	}

	return result;
}

uint64_t GetCode(short **tab)
{
	uint64_t result = 0;

	for (int x = 0; x < w; ++x)
	{
		for (int y = 0; y < k; ++y)
		{
			result << 4;
			result += tab[x][y];
		}
	}

	return result;
}

bool Swap(short **tab, int x1, int y1, int x2, int y2)
{
	if (x1 >= w || x1 < 0 || x2 >= w || x2 < 0 ||
		y1 >= k || y1 < 0 || y2 >= k || y2 < 0)
		return false;

	short tmp = tab[x1][y1];
	tab[x1][y1] = tab[x2][y2];
	tab[x2][y2] = tmp;

	return true;
}

int main()
{
	short ** tab;
	set<uint64_t> visited;

	cin >> w >> k;

	tab = new short*[w];
	for (int i = 0; i < w; ++i)
		tab[i] = new short[k];

	for (int i = 0; i < w; ++i)
		for (int j = 0; j < k; ++j)
			cin >> tab[i][j];

	//some test code
	cout << "\n" << GetScore(tab);
	cout << "\n" << GetCode(tab);
	visited.insert(GetCode(tab));
	cout << "\n" << Contains(visited, GetCode(tab));

	return 0;
}