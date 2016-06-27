#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <math.h>

#define QUE_COUNT 5
#define NORMALIZE	// uncommenting this will normalise all element attributes
#define STDDEV		// uncommenting this will divite all distances by their standard deviations
#define DIST_FUNC 1	// 1 = euclidean, 2 = czebyszew, 3 = taxi

using namespace std;

typedef vector<double> TestData;

struct DistType
{
	double dist;
	string type;

	DistType() {}
	DistType(double d, string t) : dist(d), type(t) {}
	bool operator < (const DistType &b) const { return dist < b.dist; }
};


double DistanceSqEuc(TestData &l, TestData &r, vector<double> &stdDev)
{
	static double result;
	result = 0;
	for (int i = 0; i < l.size(); ++i)
#ifdef STDDEV
		result += (l[i] - r[i]) * (l[i] - r[i]) * stdDev[i];
#else
		result += (l[i] - r[i]) * (l[i] - r[i]);
#endif // STDDEV

	return result;
}


double DistanceCze(TestData &l, TestData &r, vector<double> &stdDev)
{
	static double result;
	result = 0;
	for (int i = 0; i < l.size(); ++i)
#ifdef STDDEV
		result = abs(l[i] - r[i]) * stdDev[i] > result ? abs(l[i] - r[i]) * stdDev[i] : result;
#else
		result = abs(l[i] - r[i]) > result ? abs(l[i] - r[i]) : result;
#endif // STDDEV

	return result;
}


double DistanceTax(TestData &l, TestData &r, vector<double> &stdDev)
{
	static double result;
	result = 0;
	for (int i = 0; i < l.size(); ++i)
#ifdef STDDEV
		result += abs(l[i] - r[i]) * stdDev[i];
#else
		result += abs(l[i] - r[i]);
#endif // STDDEV

	return result;
}


string GetMostCommonElement(priority_queue<DistType> &que)
{
	map<string, pair<int, double>> typeCounts;
	vector<string> keys;
	DistType td;
	string best;
	int bestCount = 0;
	int bestDist = ~0;

	while (que.empty() == false)
	{
		td = que.top();
		que.pop();
		if (find(keys.begin(), keys.end(), td.type) != keys.end())
		{
			typeCounts[td.type].first += 1;
			if (td.dist < typeCounts[td.type].second)
				typeCounts[td.type].second = td.dist;
		}
		else
		{
			keys.push_back(td.type);
			typeCounts[td.type].first = 1;
			typeCounts[td.type].second = td.dist;
		}
	}

	for each (string key in keys)
	{
		if (typeCounts[key].first > bestCount)
		{
			bestCount = typeCounts[key].first;
			best = key;
		}
		else if (typeCounts[key].first == bestCount && typeCounts[key].second < bestDist) // if counts are equall, choose the closer one
		{
			bestDist = typeCounts[key].second;
			best = key;
		}
	}
	return best;
}


bool TestNext(TestData &tested, double(*Distance)(TestData&, TestData&, vector<double>&), map<string, vector<TestData>> &info, map<string, vector<double>> &stdDev, vector<string> &keys, int maxCount, string correct)
{
	static priority_queue<DistType> que;
	que = priority_queue<DistType>();

	for each (string key in keys)
	{
		for each (TestData td in info[key])
		{
			que.push(DistType(Distance(tested, td, stdDev[key]), key));
			if (que.size() > maxCount) que.pop();
		}
	}
	cin >> correct;
	return correct == GetMostCommonElement(que);
}

static int correctGuesses = 0, guesses = 0;
void TestSet(map<string, vector<TestData>>& learnData, map<string, vector<TestData>>& testData, vector<string>& keys)
{
	map<string, vector<double>> avgs;		// this contains the average values of elements by category
	map<string, vector<double>> stdDev;		// this contains all standard deviations of values of elements by category
	double(*Distance)(TestData&, TestData&, vector<double>&);  // function pointer: points at one of the distance functions
	int k;

	#if DIST_FUNC == 1
	Distance = &DistanceSqEuc;
	#elif DIST_FUNC == 2
	Distance = &DistanceCze;
	#elif DIST_FUNC == 3
	Distance = &DistanceTax;
	#endif

	k = learnData[keys[0]][0].size();

	// averages
	for each(string key in keys)
	{
		if (avgs.find(key) == avgs.end())
			avgs[key] = vector<double>(k, 0);
		for each (TestData td in learnData[key])
			for (int i = 0; i < td.size(); ++i)
				avgs[key][i] += td[i] / learnData[key].size();
	}

	#ifdef NORMALIZE
	for each(string key in keys)
		for each (TestData td in learnData[key])
			for (int i = 0; i < td.size(); ++i)
				td[i] -= avgs[key][i];
	#endif // NORMALIZE

	#ifdef STDDEV
	for each (string key in keys)
	{
		stdDev[key] = vector<double>(k, 0);

		for each(TestData td in learnData[key])
			for (int i = 0; i < k; ++i)
				stdDev[key][i] += (avgs[key][i] - td[i]) * (avgs[key][i] - td[i]);

		for (int i = 0; i < k; ++i)
		{
			stdDev[key][i] /= learnData[key].size();
			stdDev[key][i] = 1 / stdDev[key][i]; // calculating inverse of std to avoid the division operation in the distance calculation
		}
	}
	#endif // STDDEV

	// guessing
	for each (string key in keys)
	{
		for each (TestData td in testData[key])
		{
			correctGuesses += TestNext(td, Distance, learnData, stdDev, keys, QUE_COUNT, key);
			guesses++;
		}
	}

	//cout << "\nlearnData:";
	//for each (string key in keys)
	//{
	//	cout << "\n" << key << ": " << learnData[key].size();
	//}

	//cout << "\ntestData:";
	//for each (string key in keys)
	//{
	//	cout << "\n" << key << ": " << testData[key].size();
	//}

}


int main()
{
	int k; // this is the ammount of attributes that describe each element
	int n; // this is the ammount of elements for learning
	int m;

	map<string, vector<TestData>> allData;
	map<string, vector<TestData>> learnData;// this contains all elements used for learning
	map<string, vector<TestData>> testData; // this contains all elements for guessing
	vector<string> keys;					// this contains all names of categories

	// temp variables
	TestData td;
	string key;
	double val;
	int firstPart, secondPart, thirdPart;
	bool ok;


	cin >> k >> n;

	// reading data for learning
	for (int i = 0; i < n; ++i)
	{
		td.clear();
		for (int j = 0; j < k; ++j)
		{
			cin >> val;
			td.push_back(val);
		}

		cin >> key;
		allData[key].push_back(td);		// adding read data into the collection

		// if a new type of key appeared, add new category
		ok = false;
		for each (string k in keys)
			if (k == key) ok = true;
		if (ok == false)
			keys.push_back(key);
	}

	//Testing by cross-validation
	for (int i = 0; i < 3; ++i)
	{
		learnData.clear();
		testData.clear();

		for each (string key in keys)
		{
			m = allData[key].size();
			firstPart = (m + 2) / 3;
			secondPart = 2 * firstPart;
			thirdPart = m;

			for (int j = 0; j < m; ++j)
			{
				if ((i == 0 && j < firstPart) || (i == 1 && j >= firstPart && j < secondPart) || (i == 2 && j >= secondPart))
					testData[key].push_back(allData[key][j]);
				else
					learnData[key].push_back(allData[key][j]);
			}
		}

		TestSet(learnData, testData, keys);
	}

	cout << "\nCorrect guesses: " << correctGuesses << " out of " << guesses << ".";
}
