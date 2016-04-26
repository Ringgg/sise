#include <iostream>
#include <vector>
#include <set>
#include <map>

using namespace std;

struct State
{
	int val[3];
	static const int max[];

	State(int a, int b, int c)
	{
		val[0] = a;
		val[1] = b;
		val[2] = c;
	}

	State(const State &s)
	{
		val[0] = s.val[0];
		val[1] = s.val[1];
		val[2] = s.val[2];
	}

	bool operator <(const State x) const
	{
		if (val[0] != x.val[0]) return val[0] < x.val[0];
		if (val[1] != x.val[1]) return val[1] < x.val[1];
		return val[2] < x.val[2];
	}

	State (){}

	State& Pour(int from, int to)
	{
		State s(*this);
		int amount = val[from] + val[to];
		if (amount > max[to])
		{
			s.val[to] = max[to];
			s.val[from] = amount - max[to];
		}
		else
		{
			s.val[to] = amount;
			s.val[from] = 0;
		}
		return s;
	}
};

const int State::max[] = { 8, 5, 3 };

bool Contains(set<State>& visited, State& s)
{
	return visited.find(s) != visited.end();
}

void BFS(vector<State>& q, set<State>& visited, map<State, State>& paths)
{
	if (q.size() == 0) return;

	State s = q.back();
	State tmp;
	q.pop_back();
	
	if (s.val[0] == 6)
	{
		cout << "AAA";
	}
	else
	{
		cout << "B";
		tmp = s.Pour(0, 1);
		if (!Contains(visited, tmp))
		{
			q.push_back(tmp);
			visited.insert(tmp);
			paths.emplace(tmp, s);
		}

		tmp = s.Pour(0, 2);
		if (!Contains(visited, tmp))
		{
			q.push_back(tmp);
			visited.insert(tmp);
			paths.emplace(tmp, s);
		}
		tmp = s.Pour(1, 0);
		if (!Contains(visited, tmp))
		{
			q.push_back(tmp);
			visited.insert(tmp);
			paths.emplace(tmp, s);
		}
		tmp = s.Pour(1, 2);
		if (!Contains(visited, tmp))
		{
			q.push_back(tmp);
			visited.insert(tmp);
			paths.emplace(tmp, s);
		}
		tmp = s.Pour(2, 0);
		if (!Contains(visited, tmp))
		{
			q.push_back(tmp);
			visited.insert(tmp);
			paths.emplace(tmp, s);
		}
		tmp = s.Pour(2, 1);
		if (!Contains(visited, tmp))
		{
			q.push_back(tmp);
			visited.insert(tmp);
			paths.emplace(tmp, s);
		}
		
		BFS(q, visited, paths);
	}
}

int main()
{
	State s(8, 0, 0);

	vector<State> q;
	set<State> visited;
	map<State, State> paths;

	q.push_back(s);
	BFS(q, visited, paths);

	getchar();
	return 0;
}