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
#include "Lem.h"

#ifdef SVMCLASSIFIER
#include "SVM.h"
#else
#include "Soma.h"
#include "AdaBoost.h"
#endif

#define MMIN(x, y) (((x) < (y)) ? (x) : (y))
#define MMAX(x, y) (((x) > (y)) ? (x) : (y))

#ifdef DISCRETIZE
#include "Anchor.h"

#define GENE_TYPE unsigned
#define CONVERT_REP(x) convertRep(x)
#else 

#define GENE_TYPE double
#define CONVERT_REP(x) (x)
#endif

#include <numeric>
#include <random>
#include <algorithm>

#include <memory>

#include <vector>
#include <utility>
#include <set>
#include <map>
#include <iostream>

#ifdef BIRTHCONTROL
#include <unordered_set>
#include <string>
#endif


namespace {
    using CHROMOSOME_REP = std::vector<GENE_TYPE>;
    using ChromosomePtr = Chromosome<CHROMOSOME_REP>;
    using Population = std::vector<ChromosomePtr>;
    using Group = std::vector<CHROMOSOME_REP>;
    using ChromosomeComparsion = bool (*) (const ChromosomePtr &, const ChromosomePtr &);
    using FitnessWheelType = unsigned (*) (const double & , Population & );
    using VoidFuncType = void (*) ();
    std::vector<CHROMOSOME_REP> atrain;
    std::vector<CHROMOSOME_REP> postrain;
    std::vector<CHROMOSOME_REP> negtrain;
    std::vector<bool> alabel;

    std::vector<unsigned> indexes;
    std::vector<Group> TrainGroups;

    Population population, children;

    Statistic stats;
    std::pair<double, std::vector<std::vector<double>>> absBest;

    bool progress = false;
    int progressCounter = 10, initProgressProbe = 10;
    int mutationCounter = 10, initMutationProbe = 10;

    ObjectiveFuncType objFunc;
    ObjectiveFuncType fitnessFunc;
    Specimen specimen;
    unsigned popSize;
    unsigned byLearning, byEvolution, byRandom;
    // unsigned lowerBound, upperBound;

    unsigned groupCombinations, childrenByGroup, groupSize;

    // porovnavani chromozomu maximalizace/minimalizace
    ChromosomeComparsion compareChromosomes;
    // fitnessWheel
    FitnessWheelType fitnessWheel;
    VoidFuncType rank_survival;
    VoidFuncType updateStatistic;

#ifdef BIRTHCONTROL
    std::unordered_set<std::string> uniques;

    std::string chromosomeToSting(ChromosomePtr & ch) {
        std::string s;
        for (auto & v : ch.representation) {
            s += std::to_string(v) + ",";
        }
        return s;
    }


    inline bool uniqueSetAndCheck(ChromosomePtr & ch) {
        std::string s = chromosomeToSting(ch);
        if (uniques.find(s) != uniques.end()) {
            return false;
        } 
        uniques.insert(s);
        return true;
    }

#endif

    std::random_device rd;
    std::default_random_engine r(rd());

#ifdef DISCRETIZE   
    std::vector<double> CONVERTED_REP;
    std::vector<std::pair<GENE_TYPE, GENE_TYPE>> DIMENSIONS_RANGES;
    std::vector<Anchor> Anchors;

    // template <typename TD>
    double toDouble(GENE_TYPE num) {
        return static_cast<double> (num);
    }
    template <typename TD>
    GENE_TYPE toGenetype(TD num) {
        return static_cast<GENE_TYPE> (num);
    }

    std::vector<double> & convertRep(CHROMOSOME_REP & rep) {
        for (unsigned i = 0; i < Anchors.size(); ++i) {
            CONVERTED_REP[i] = Anchors[i][rep[i]];
            // std::cerr << rep[i] << "repi\n";
        }
        // std::transform(rep.begin(), rep.end(), CONVERTED_REP.begin(), toDouble);
        return CONVERTED_REP;
    }

    void rangesOfSpecimen(Specimen & specimen) {
        Anchors.reserve(specimen.size());
        DIMENSIONS_RANGES.reserve(specimen.size());
        for (unsigned i = 0; i < specimen.size(); ++i){
            Anchors.emplace_back(specimen[i].first, specimen[i].second);
            DIMENSIONS_RANGES.emplace_back(0, Anchors[i].max());
        }       
            std::cerr << Anchors.size() << '\t'<< Anchors[i].max() << '\t'<< Anchors.back().max() << '\t' << specimen[i].second << '\n';
    }

#endif

    std::pair<GENE_TYPE, GENE_TYPE> & dimensionRange(unsigned dim) {
#ifdef DISCRETIZE
        return DIMENSIONS_RANGES[dim];
#else
        return specimen[dim];
#endif
    }

#ifdef SVMCLASSIFIER
    double getDiameter(std::vector<std::vector<double>> & examples) {
        double diameter = 0.1;
        double min, max;
        for (unsigned di = 0; di < specimen.size(); ++di) {
            min = std::numeric_limits<double>::max();
            max = std::numeric_limits<double>::lowest();
            for (auto & ex : examples) {
                min = std::min(min,ex[di]);
                max = std::max(max,ex[di]);
            }
            // std::cerr << "fffff " << max << "  " << min << "\t" << max - min << '\n' ;
            diameter = std::max(diameter, max - min);
        }
        return diameter;
    }
#endif

    template <typename TP>
    TP binom(TP n, TP k) {
        if (k == 1 || k == (n - 1)) return n;
        if (k == 0 || n == k) return 1;
        // k = (k < n - k) ? n - k : k;
        return binom(n - 1, k - 1) + binom(n - 1, k);
    }

    unsigned combinationCount(unsigned groups) {
        return binom(groups, (unsigned) 2);
    }


    bool compareMaximizeChromosomes(const ChromosomePtr &ch1, const ChromosomePtr &ch2) {
        return ch1.fitness < ch2.fitness;
    }

    bool compareMinimizeChromosomes(const ChromosomePtr &ch1, const ChromosomePtr &ch2) {
        return ch1.fitness > ch2.fitness;
    }

    double normal_distribution(double mean, double stddev) {
        std::normal_distribution<double> distribution(mean, stddev);
        return distribution(r);
    }

    double rand_range(double lowerBound, double upperBound) {
        std::uniform_real_distribution<double> d(MMIN(lowerBound, upperBound),MMAX(lowerBound, upperBound));
        double a = 0.01 * int (d(r) * 100) ;            
        return a;
    }

    template <typename TT>
    TT rand_range(std::pair<TT, TT> & bounds) {
        return rand_range(bounds.first, bounds.second);
    }

    int rand_int(int lowerBound, int upperBound) {
        std::uniform_int_distribution<int> d(MMIN(lowerBound, upperBound),MMAX(lowerBound, upperBound));
        return d(r);
    }

    int rand_int(int ub) {
        return rand_int(0, ub);
    }

    void valueTransfer(const ChromosomePtr & from, ChromosomePtr & to) {
        to.fitness = from.fitness;
        std::copy(from.representation.begin(), from.representation.end(),
            to.representation.begin());
    }

    double randomValueFromPopulation(Population & p, const int dimension) {
        int pos = rand_int(0, popSize - 1);
        return p[pos].representation[dimension];
    }

    double sumPopulationFitness(Population & p) {
        double total = 0;
        for (auto & ch : p) {
            total += ch.fitness;
        }
        return total;
    }

    template <typename InputIterator1, typename InputIterator2>
    double averagePopulationFitness(InputIterator1 start, InputIterator2 end) {
        unsigned i = 0;
        double sum = 0;
        for (;start != end; ++start, ++i) {
            sum += (*start).fitness;
        }
        return sum / i;
    }

    double averagePopulationFitness(Population & p) {
        return averagePopulationFitness(p.begin(), p.end());
    }

    std::vector<std::vector<double>> bestRepresentations(Population & p) {
        std::vector<std::vector<double>> representations;
        const double bestFitness = p.back().fitness;
        int i = popSize - 1;
        while (i >= 0 && p[i].fitness == bestFitness) {
            representations.emplace_back(CONVERT_REP(p[i].representation));
            --i;
        }
        return representations;
    }

    void computeGroupsStatistics(std::vector<GroupStatistic> & gs, Population & p) {
        // gs.resize(groupCombinations);
        double groupAvrg;
        for (unsigned i = 0; i < groupCombinations; ++i) {
            groupAvrg = averagePopulationFitness(p.begin() + i * childrenByGroup,
                                                 p.begin() + (i+1) * childrenByGroup);
            gs[i] = {groupAvrg, {(*std::min_element(p.begin() + i * childrenByGroup,
                                                    p.begin() + (i+1) * childrenByGroup, 
                                                    compareChromosomes)).fitness,
                                 (*std::max_element(p.begin() + i * childrenByGroup,
                                                    p.begin() + (i+1) * childrenByGroup, 
                                                    compareChromosomes)).fitness}};
        }
        // by Evolution
        if (byEvolution > 0) {
            groupAvrg = averagePopulationFitness(p.begin() + byLearning,
                                                 p.begin() + byLearning + byEvolution);
            gs[groupCombinations] = 
                    {groupAvrg, {(*std::min_element(p.begin() + byLearning,
                                                    p.begin() + byLearning + byEvolution, 
                                                    compareChromosomes)).fitness,
                                 (*std::max_element(p.begin() + byLearning,
                                                    p.begin() + byLearning + byEvolution, 
                                                    compareChromosomes)).fitness}}; 
        }
        // by Random
        if (byRandom > 0) {         
            groupAvrg = averagePopulationFitness(p.begin() + byLearning + byEvolution,
                                                 p.end());
            gs[groupCombinations+1] = 
                    {groupAvrg, {(*std::min_element(p.begin() + byLearning + byEvolution,
                                                    p.end(), 
                                                    compareChromosomes)).fitness,
                                 (*std::max_element(p.begin() + byLearning + byEvolution,
                                                    p.end(), 
                                                    compareChromosomes)).fitness}};     
        }
    }

    void childrenStatistic(Population & p) {
        std::vector<GroupStatistic> gs(groupCombinations+2);
        computeGroupsStatistics(gs, p);
        stats.groupsStatistics.emplace_back(gs);
    }


    void updateStatisticMaximize() {
        double lastBest = stats.bests.back();
        stats.averages.emplace_back(averagePopulationFitness(population));
        stats.bests.emplace_back(population.back().fitness);

        progress = population.back().fitness > lastBest;
        if (progress && absBest.first < population.back().fitness) {
            absBest.first = population.back().fitness;
            absBest.second = bestRepresentations(population);
        }
    }

    void updateStatisticMinimize() {
        double lastBest = stats.bests.back();
        stats.averages.emplace_back(averagePopulationFitness(population));
        stats.bests.emplace_back(population.back().fitness);

        progress = population.back().fitness < lastBest;
        if (progress && absBest.first > population.back().fitness) {
            absBest.first = population.back().fitness;
            absBest.second = bestRepresentations(population);
        }
    }


    void indexesForGroups(unsigned groups) {
        indexes.resize(groups);
        unsigned up, down;

        down = 0;
        up = popSize - groupSize;

        for (unsigned i = 0; i < groups; ++i) {
            if (i % 2 == 0) {
                indexes[i]=(up);
                up -= groupSize;
            } else {
                indexes[i]=(down);
                down += groupSize;
            }
        }

        std::sort(indexes.begin(), indexes.end());
    }

    void extremaSelection(Population & p) {
        for (unsigned i = 0; i < indexes.size(); ++i) {     
            // std::cerr << "Group " << TrainGroups.size() - i - 1 << ":\t";
            for (unsigned ai = 0, j = indexes[i], e = indexes[i] + groupSize; j < e; ++j, ++ai) {
                std::copy(p[j].representation.begin(), p[j].representation.end(),
                    TrainGroups[TrainGroups.size() - i - 1][ai].begin());
                // std::cerr << p[j].fitness << '\t';
            }           
            // std::cerr << '\n';
        }
    }

    // Single point crossover
    void crossover(const ChromosomePtr &ch1, const ChromosomePtr &ch2, ChromosomePtr & ch) {
        int point = rand_int(0, ch1.representation.size());
        for (int i = 0; i < point; ++i)
        {
            ch.representation[i] = ch1.representation[i];
        }
        for (unsigned i = point; i < ch2.representation.size(); ++i)
        {
            ch.representation[i] = ch2.representation[i];
        }
    }
    // Real value crossover
    void valueCrossover(const ChromosomePtr &ch1, const ChromosomePtr &ch2, ChromosomePtr & ch) {
        for (unsigned i = 0; i < ch1.representation.size(); ++i)
        {
            ch.representation[i] = 0.5 * (ch1.representation[i] + ch2.representation[i]);
        }
    }

    void randomChromosome(ChromosomePtr & ch) {
        for (unsigned i = 0, e = specimen.size(); i < e; ++i ) {
            ch.representation[i] = rand_range(dimensionRange(i));
            if (ch.representation[i] < dimensionRange(i).first) std::cerr << "ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd ddd " << ch.representation[i] << "\n";
        }
    }

    // ocekava vzestupne serazene dle fitness population a children
    void rank_survival_Maximize() {
        for (unsigned i = 0, j = popSize - 1; i < popSize; ++i)
        {
            if (population[i].fitness < children[j].fitness) {
                valueTransfer(children[j], population[i]);
                --j;
            }
        }
    }
    void rank_survival_Minimize() {
        for (unsigned i = 0, j = popSize - 1; i < popSize; ++i)
        {
            if (population[i].fitness > children[j].fitness) {
                valueTransfer(children[j], population[i]);
                --j;
            }
        }
    }

    void evaluatePopulation(Population & p) {
        for (auto & ch : p) {
            ch.fitness = fitnessFunc(CONVERT_REP(ch.representation));
        }
    }

    void mutateChromosome(ChromosomePtr & child) {
        for (unsigned i = 0; i < specimen.size(); ++i) {
            double hunp = 1.0;
            double chval = child.representation[i];
            child.representation[i] = normal_distribution(chval, hunp);
            while (child.representation[i] < dimensionRange(i).first 
                   || child.representation[i] > dimensionRange(i).second) {
                child.representation[i] = normal_distribution(chval, hunp);
            }
        }
    }

    void mutateChromosome(const ChromosomePtr & parent, ChromosomePtr & child, double probability = .5) {
        for (unsigned i = 0; i < specimen.size(); ++i) {
            if (rand_range(0., 1.) >= probability) continue;
            double hunp = 1.0;
            child.representation[i] = normal_distribution(parent.representation[i], hunp);
            while (child.representation[i] < dimensionRange(i).first 
                   || child.representation[i] > dimensionRange(i).second) {
                child.representation[i] = normal_distribution(parent.representation[i], hunp);
            }
        }
    }

#ifdef SVMCLASSIFIER    
    void svmCrossover(Population &p, SVM & svm, ChromosomePtr & ch, double diameter, double probability = .5) {
        int cnt = 100;
        if (svm.supportVectorsCount()) {
            const std::vector<double> & svref = svm.getSupportVector(rand_int(svm.supportVectorsCount()-1));
            for (unsigned di = 0; di < svref.size(); ++di) {
                if (rand_range(0., 1.) >= probability) ch.representation[di] = svref[di];
                else ch.representation[di] = rand_range(MMAX(dimensionRange(di).first,  svref[di] - diameter), 
                                                    MMIN(dimensionRange(di).second, svref[di] + diameter));
            }
        } else {
            while(cnt-- > 0) {
                mutateChromosome(p[rand_int(p.size()/2, p.size()-1)], ch);
                if (svm.classifyOne(ch.representation)) return;
            }
            cnt = 100;
            while (cnt-- > 100) {
                randomChromosome(ch);
                if (svm.classifyOne(ch.representation)) return;
            }
        }
    }
#else
    // instantiantion algorithm 1 
    void valueDimensionsCrossover(Population &p, RangesForClass & dimensions, ChromosomePtr & ch) {
        for (int i = 0, e = specimen.size(); i < e; ++i)
        {
            if (dimensions.count(i)) {
                const unsigned rangeindex = rand_int(dimensions[i].size() - 1);
                unsigned ri = 0;
                for (auto & si : dimensions[i]) {
                    if (rangeindex == ri) {
                        ch.representation[i] = rand_range((GENE_TYPE) MMAX(dimensionRange(i).first, si.first), 
                                                          (GENE_TYPE) MMIN(dimensionRange(i).second, si.second));
                        break;
                    }
                    ++ri;
                }
            }
            else
                ch.representation[i] = randomValueFromPopulation(p, i);
        }
    }
#endif

    void mutatePopulation() {
        for (unsigned i = 0; i < popSize;) {
            mutateChromosome(population[i], children[i], 0.25);
#ifdef BIRTHCONTROL
            if (uniqueSetAndCheck(children[i]))
#endif
            ++i;
        }
    }

    void sortPopulation(Population &p) {
        std::sort(p.begin(), p.end(), compareChromosomes);
    }

    void createInitialPopulation() {
        for (unsigned i = 0; i < popSize; )
        {
            randomChromosome(population[i]);
#ifdef BIRTHCONTROL
            if (uniqueSetAndCheck(population[i]))
#endif
            ++i;
        }
    }

    unsigned fitnessMaximizeWheel(const double & sumFitness, Population & p) {
        double target = rand_range(0.0, sumFitness);
        double tsum = 0;
        for (unsigned i = 0; i < popSize; i++) {
            if (target <= tsum) {
                return i;
            }
            tsum += p[i].fitness;
        }
        return popSize - 1;
    }   

    unsigned fitnessMinimizeWheel(const double & sumFitness, Population & p) {
        double target = rand_range(0.0, sumFitness);
        double tsum = 0;
        const double max = p[0].fitness;
        for (unsigned i = 0; i < popSize; i++) {
            if (target <= tsum) {
                return i;
            }
            tsum += (max - p[i].fitness);
        }
        return popSize - 1;
    }   

    void generateByLearning() {
        extremaSelection(population);
        for (unsigned i = 0, chi = 0; i < TrainGroups.size(); ++i) {
            for (unsigned j = i + 1; j < TrainGroups.size(); ++j) {
#ifdef BIRTHCONTROL
                unsigned tries = 100;
#endif

#ifdef SVMCLASSIFIER
                const double diameter = getDiameter(TrainGroups[i]);
                // std::cerr << "diameter " << diameter << '\n';
                SVM svm(TrainGroups[i], TrainGroups[j], diameter);
#else
                AdaBoost a(TrainGroups[i], TrainGroups[j], 2 * specimen.size());
                RangesForClass ranges = a.rangesForClass(true);
#endif
                for(unsigned dd = 0; dd < childrenByGroup;) {
#ifdef SVMCLASSIFIER
                    svmCrossover(population, svm, children[chi], diameter);
#else
                    valueDimensionsCrossover(population, ranges, children[chi]);
#endif
#ifdef BIRTHCONTROL
                    if (uniqueSetAndCheck(children[chi]) || --tries == 0)
#endif
                    {++chi;++dd;}
                }
            }
        }
    }
    
    void generateByEvolution() {
        double sumFitness = sumPopulationFitness(population);
#ifdef BIRTHCONTROL
        unsigned tries = 100;
#endif
        for (unsigned i = byLearning, mod = 0, e = byLearning + byEvolution; i < e; ++mod)
        {
            unsigned p1 = fitnessWheel(sumFitness, population);
            unsigned p2 = fitnessWheel(sumFitness, population);
            switch (mod % 3) {
            case 0:
                crossover(population[p1], population[p2], children[i]);
                break;
            case 1:
                valueCrossover(population[p1], population[p2], children[i]);
                break;
            default:
                mutateChromosome(population[p1], children[i]);
            }

#ifdef BIRTHCONTROL
            if (uniqueSetAndCheck(children[i]) && --tries > 0)
#endif
            ++i;
#ifdef BIRTHCONTROL
            if (tries == 0) {
                ++i;
                tries = 100;
            }
#endif
        }       
    }
    
    void generateRandomly() {
        for (unsigned i = popSize - byRandom; i < popSize; ++i) {
            randomChromosome(children[i]);
        }
    }

    void createChildren() {
        generateByLearning();
        generateByEvolution();
        generateRandomly();
    }

#ifdef BIRTHCONTROL
    void checkPopulationUniqueness(Population & p = children) {
        for (unsigned i = 0; i < popSize; ++i)  {
            while (! uniqueSetAndCheck(p[i])) {
                mutateChromosome(p[i]);
            }
        }
    }
#endif

    inline double inverseFitness(std::vector<double> & representation) {
        return 1.0 / (1 + objFunc(representation));
    }

    inline double substractFitness(std::vector<double> & representation) {
        return 10000000 - objFunc(representation);
    }

}

Lem::Lem(ObjectiveFuncType fitFunc, Specimen spc,
        unsigned pSize, double byL, double byE,
        const unsigned groups, const double maxGroupSize,
        unsigned minimize) 
{   
    objFunc = fitFunc;
    fitnessFunc = fitFunc;
    specimen = spc; 
    popSize = pSize;
    byL = std::min(std::max(byL, 0.0), 1.0);
    byE = std::min(std::max(byE, 0.0), 1.0 - byL);

    groupSize = std::min(maxGroupSize * popSize, (double) popSize / groups);
    groupCombinations = combinationCount(groups);
    // childrenByGroup = std::min(maxGroupSize * popSize, (byL * popSize) / groupCombinations);
    childrenByGroup = (byL * popSize) / groupCombinations;

    byLearning = childrenByGroup * groupCombinations;
    byEvolution = byE * popSize;
    byRandom = popSize - byEvolution - byLearning;
    // lowerBound = std::min(lB, 1.0) * popSize;
    // upperBound = popSize - std::min(uB, 1.0) * popSize;;
    
    if (minimize == 1) {
        compareChromosomes = compareMinimizeChromosomes;
        fitnessWheel = fitnessMinimizeWheel;
        rank_survival = rank_survival_Minimize;
        updateStatistic = updateStatisticMinimize;
    }
    else  { // (minimize == 0)
        compareChromosomes = compareMaximizeChromosomes;
        fitnessWheel = fitnessMaximizeWheel;
        rank_survival = rank_survival_Maximize;
        updateStatistic = updateStatisticMaximize;
        if (minimize == 2) {
            fitnessFunc = inverseFitness;
        } else if (minimize == 3) {
            fitnessFunc = substractFitness;
        }
    }  


    population = Population(popSize);
    children = Population(popSize);
    for (unsigned i = 0; i < popSize; ++i) {
        population[i].representation = CHROMOSOME_REP(specimen.size());
        children[i].representation = CHROMOSOME_REP(specimen.size());
    }

#ifdef DISCRETIZE
    CONVERTED_REP.resize(specimen.size());
    rangesOfSpecimen(specimen);
#endif

    indexesForGroups((childrenByGroup > 0) ? groups : 0);
    createInitialPopulation();
    
    evaluatePopulation(population); 
    childrenStatistic(population);  
    
    sortPopulation(population);

    // std::cerr << "Population\n";
    // for (auto & pi: population) {
        // std::cerr << pi.fitness << ", "; 
    // }
    // std::cerr << '\n';

    TrainGroups.resize((childrenByGroup > 0) ? groups : 0);
    for (unsigned i = 0; i < TrainGroups.size(); ++i) {
        // TrainGroups[i].resize(childrenByGroup);
        TrainGroups[i].resize(groupSize);
        for (unsigned chi = 0; chi < TrainGroups[i].size(); ++chi) {
            TrainGroups[i][chi].resize(specimen.size());
        }
    }

    stats.averages.emplace_back(averagePopulationFitness(population));
    stats.bests.emplace_back(population.back().fitness);

    absBest = {population.back().fitness, bestRepresentations(population)};

}

void Lem::step() {
    if (!progress && --progressCounter <= 0) {
        // std::cerr << "NO PROGRESS\n";        
        progressCounter = initProgressProbe;
        if (--mutationCounter > 0) {
#ifdef SVMCLASSIFIER            
            mutatePopulation();
#else
            Soma::someToOne(population, children, specimen);
#endif
#ifdef BIRTHCONTROL
            checkPopulationUniqueness(children);
#endif
            evaluatePopulation(children);
            childrenStatistic(children);    
            sortPopulation(children);
            rank_survival();        
          } else 
        {
            mutationCounter = initMutationProbe;
            createInitialPopulation();
            evaluatePopulation(population);     
            childrenStatistic(population);  
        }
    } else { // increase diversity
        if (progress)
            progressCounter = initProgressProbe;
        createChildren();
        evaluatePopulation(children);
        childrenStatistic(children);        
        sortPopulation(children);
        rank_survival();
        // mainlyChildren(0.3);
    }
    sortPopulation(population);
    updateStatistic();
}

void Lem::run(const unsigned generations, int progressProbe, int mutationProbe) {
    progressCounter = initProgressProbe = progressProbe;
    mutationCounter = initMutationProbe = mutationProbe;
    const unsigned newcapacity = stats.averages.size() + generations;
    stats.averages.reserve(newcapacity);
    stats.bests.reserve(newcapacity);
    stats.groupsStatistics.reserve(newcapacity);
    for (unsigned i = 0; i < generations; ++i)
    {
        step();
    }
}

Statistic Lem::getStatistic() {
    stats.representations = absBest;
    return stats;
}

// std::vector<std::vector<double>> Lem::getPopulation() {
//  std::vector<std::vector<double>> allreps(popSize);
//  for (unsigned i = 0; i < popSize; ++i)
//      allreps[i] = population[i].representation;
//  return allreps;
// }
