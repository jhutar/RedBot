/*
    Copyright (c) 2014 Pavel Grunt, xgrunt01@stud.fit.vutbr.cz

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef LEM_H
#define LEM_H

#include <vector>
#include <utility>

using ObjectiveFuncType = double (*) (std::vector<double> &);
using Specimen = std::vector<std::pair<double, double>>;
using GroupStatistic = std::pair<double, std::pair<double, double>>;

struct Statistic
{
	std::vector<double> averages;
	std::vector<double> bests;
	std::pair<double, std::vector<std::vector<double>>> representations;
	std::vector<std::vector<GroupStatistic>> groupsStatistics;
};


template <typename T>
struct Chromosome
{
	T representation;
	double fitness;
};

class Lem
{
public:
	Lem(ObjectiveFuncType f, Specimen specimen,
	 	const unsigned popSize = 100,
	 	double byLearning = 0.70, double byEvolution = 0.20,
	 	const unsigned groups = 2, const double maxGroupSize = 0.30,
	 	unsigned minimize = 0);

	void step();
	void run(const unsigned steps, int progressProbe = 4, int mutationProbe = 10);

	Statistic getStatistic();
	// std::vector<std::vector<double>> getPopulation();
};


#endif
