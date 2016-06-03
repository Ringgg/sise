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


struct TestData
{
	vector<double> data;
};


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
	for (int i = 0; i < l.data.size(); ++i)
#ifdef STDDEV
		result += (l.data[i] - r.data[i]) * (l.data[i] - r.data[i]) * stdDev[i];
#else
		result += (l.data[i] - r.data[i]) * (l.data[i] - r.data[i]);
#endif // STDDEV

	return result;
}


double DistanceCze(TestData &l, TestData &r, vector<double> &stdDev)
{
	static double result;
	result = 0;
	for (int i = 0; i < l.data.size(); ++i)
#ifdef STDDEV
		result = abs(l.data[i] - r.data[i]) * stdDev[i] > result ? abs(l.data[i] - r.data[i]) * stdDev[i] : result;
#else
		result = abs(l.data[i] - r.data[i]) > result ? abs(l.data[i] - r.data[i]) : result;
#endif // STDDEV

	return result;
}


double DistanceTax(TestData &l, TestData &r, vector<double> &stdDev)
{
	static double result;
	result = 0;
	for (int i = 0; i < l.data.size(); ++i)
#ifdef STDDEV
		result += abs(l.data[i] - r.data[i]) * stdDev[i];
#else
		result += abs(l.data[i] - r.data[i]);
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


bool TestNext(TestData &tested, double(*Distance)(TestData&, TestData&, vector<double>&), map<string, vector<TestData>> &info, map<string, vector<double>> &stdDev, vector<string> &keys, int maxCount)
{
	static string correct;
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


int main()
{
	int k; // this is the ammount of attributes that describe each element
	int n; // this is the ammount of elements for learning
	int m; // this is the ammount of elements to recognise

	map<string, vector<TestData>> info;		// this contains all elements used for learning
	map<string, vector<double>> avgs;		// this contains the average values of elements by category
	map<string, vector<double>> stdDev;		// this contains all standard deviations of values of elements by category
	vector<string> keys;					// this contains all names of categories
	double(*Distance)(TestData&, TestData&, vector<double>&);  // function pointer: points at one of the distance functions

															   // temp variables
	TestData td;
	string key;
	double val;
	int correctGuesses = 0;

#if DIST_FUNC == 1
	Distance = &DistanceSqEuc;
#elif DIST_FUNC == 2
	Distance = &DistanceCze;
#elif DIST_FUNC == 3
	Distance = &DistanceTAX;
#endif


	cin >> k >> n >> m;

	// reading data for learning
	for (int i = 0; i < n; ++i)
	{
		td.data.clear();
		for (int j = 0; j < k; ++j)
		{
			cin >> val;
			td.data.push_back(val);
		}

		cin >> key;
		info[key].push_back(td);			// adding read data into the collection

		if (avgs.find(key) == avgs.end())	// if a new type of element appeared, add new average category
		{
			avgs[key].resize(k);
			keys.push_back(key);
		}

		for (int j = 0; j < k; ++j)			// add element info to average values by category collection
			avgs[key][j] += td.data[j];
	}

	// calculate averages
	for each (string key in keys)
		for each (double val in avgs[key])
			val /= info[key].size();

#ifdef NORMALIZE
	for each(string key in keys)
		for each (TestData td in info[key])
			for (int i = 0; i < td.data.size(); ++i)
				td.data[i] /= avgs[key][i];
#endif // NORMALIZE

#ifdef STDDEV
	// Calculate standard deviations for each category
	for each (string key in keys)
	{
		stdDev[key] = vector<double>(k, 0);

		for each(TestData td in info[key])
			for (int i = 0; i < k; ++i)
				stdDev[key][i] += (avgs[key][i] - td.data[i]) * (avgs[key][i] - td.data[i]);

		for (int i = 0; i < k; ++i)
		{
			stdDev[key][i] /= info[key].size();
			stdDev[key][i] = 1 / stdDev[key][i]; // calculating inverse of std to avoid the division operation in the distance calculation
		}
	}
#endif // STDDEV

	// reading data for guessing
	for (int i = 0; i < m; ++i)
	{
		td.data.clear();
		for (int j = 0; j < k; ++j)
		{
			cin >> val;
			td.data.push_back(val);
		}

		correctGuesses += TestNext(td, Distance, info, stdDev, keys, QUE_COUNT);
	}

	cout << "\n\nCorrect guesses: " << correctGuesses << " out of " << m << ".\n";
}

/*
maszyna wektorow podpierajacych da sie zrobic ponoc w 5 linijek,
korzystajac z np libSVN (wolno)
https://www.csie.ntu.edu.tw/~cjlin/libsvm/
*/

/*
Dla ka¿dej klasy nale¿y policzyæ b³êdy I i II rodzaju. <TODO>
Zwizualizowaæ wybrane rzuty zbiorów danych oraz granic decyzyjnych wybranych modeli (przynajmniej dla najskuteczniejszych modeli opartych odpowiednio o k-NN i SVM). <TODO>

SVM
Mo¿na wykorzystaæ gotow¹ implementacjê samego klasyfikatora, na przyk³ad libSVM - podczas oddawania projektu trzeba jednak umieæ wyjaœniæ jego zasadê dzia³ania.
Przetestowaæ ró¿ne wartoœci parametrów oraz przynajmniej dwie ró¿ne metody przekszta³cenia j¹dra (kernel trick).
Dla ka¿dej metody zbadaæ dzia³anie modelu dla ró¿nych wartoœci liczbowych parametrów i podj¹æ próbê wskazania wartoœci optymalnych.
*/