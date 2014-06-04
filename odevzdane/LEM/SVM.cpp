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
#include "SVM.h"
#include <numeric>
#include <cmath>
#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <set>

namespace
{
    // unsigned iteration = 0;
    const double epsilon = 0.001;
    bool epsEq(double a, double b=0, double eps=epsilon) {
        return fabs(a - b) < eps;
    }

    bool inOpenInterval(double val, double lb, double ub) {
        return (val > lb) && (val < ub);
    }

    double sqVectorDifferrence(const std::vector<double> & v1, const std::vector<double> & v2) {
        return std::inner_product(v1.begin(), v1.end(), v2.begin(), 0,
                std::plus<double>(), 
                [](double v1e, double v2e){return (v1e - v2e)*(v1e - v2e);});
    }

    double norma(const std::vector<double> & v1) {
        return std::sqrt(std::inner_product(v1.begin(), v1.end(), v1.begin(), 0));
    }

    double norma(const std::vector<double> & v1, const std::vector<double> & v2) {
        return std::sqrt(sqVectorDifferrence(v1,v2));
    }

    double rbf(double gama, const std::vector<double> & v1) {
        const double prod = std::inner_product(v1.begin(), v1.end(), v1.begin(), 0);
        return std::exp(gama * prod);
    }

    double rbf(const std::vector<double> & v1) {
        return rbf(v1.size(), v1);
    }

    double rbf(double gama, const std::vector<double> & v1, const std::vector<double> & v2) {
        return std::exp(gama * sqVectorDifferrence(v1, v2));
    }

    double rbf(const std::vector<double> & v1, const std::vector<double> & v2) {
        return rbf(v1.size(), v1, v2);
    }
}

void SVM::computeErrors() {
    for (unsigned i = 0; i < errors.size(); ++i)
    {
        errors[i] = SMO_Error(i);
    }
}

double SVM::SMO_Error(const std::vector<double> & x1, const int y1) {
    return calculateOutput(x1) - y1;
}

double SVM::SMO_Error(const unsigned int index) {
    if (index < positives.size()) {
        return SMO_Error(positives[index] , 1);
    }
    return SMO_Error(negatives[index - positives.size()], -1);
}

bool SVM::SMO_ViolatesKKT(const double alpha, const int y, const double E) {
    return ((E*y) < -epsilon && alpha < C) || ((E*y) > epsilon && alpha > 0);
}

/**
 * Najde druheho k zadanemu ke vzajemne optimalizaci
 * @return byla provedena optimalizace
 */
bool SVM::SMO_OptimizeOne(double & alpha1, const std::vector<double> & x1, const int y1, double E1) {
    if (SMO_ViolatesKKT(alpha1, y1, E1)) {
        for (unsigned i = 0; i < positives.size(); ++i) {
            if (epsEq(posAlphas[i]) && SMO_OptimizationStep(alpha1, x1, y1, E1, 
                                                            posAlphas[i], positives[i], 1, errors[i]))
                    return true;
        }
        for (unsigned i = 0; i < negatives.size(); ++i) {
            if (epsEq(negAlphas[i]) && SMO_OptimizationStep(alpha1, x1, y1, E1, 
                                                            negAlphas[i], negatives[i], -1, errors[i+positives.size()]))
                    return true;            
        }
    }
    return 0;
}

bool SVM::SMO_OptimizeOne(const unsigned int index) {
    if (index < positives.size()) {
        return SMO_OptimizeOne(posAlphas[index], positives[index], 1, errors[index]);
    }
    return SMO_OptimizeOne(negAlphas[index-positives.size()], negatives[index-positives.size()], -1, errors[index]);
}

// SMO dle http://research.microsoft.com/en-us/um/people/jplatt/smo-book.pdf
bool SVM::SMO_OptimizationStep(double & alpha1, const std::vector<double> & x1, const int y1, double E1, 
                               double & alpha2, const std::vector<double> & x2, const int y2, double E2) 
{
    double L1, L2, H1, H2;
    double Lobj, Hobj, f1, f2;
    double b1, b2, bOld;
    double k11, k12, k22;

    double eta;
    double a1, a2;
    const double s = y1 * y2;
    if (y1 != y2) {
        L2 = std::max(0., alpha2 - alpha1);
        H2 = std::min(C, C + alpha2 - alpha1);
    } else { // priklady ze stejne tridy
        L2 = std::max(0., alpha1 + alpha2 - C);
        H2 = std::min(C, alpha1 + alpha2);
    }
    if (epsEq(L2, H2)) return false;

    k11 = k22 = 1; // rbf kernel : norma(x1 - x1) = 0, e^0 = 1
    k12 = rbf(rbfconst, x1, x2);      
    eta = 2 * k12 - k11 - k22;

    if (eta < 0) {
        a2 = alpha2 - y2 * (E1 - E2) / eta;         
        a2 = (a2 < L2) ? L2 : (a2 > H2) ? H2 : a2; // a2 je v <L2,H2>
    } else {
        L1 = alpha1 + s * (alpha2 - L2);
        H1 = alpha1 + s * (alpha2 - H2);
        f1 = y1 * (E1 + b) - alpha1 * k11 - s * alpha2 * k12;
        f2 = y2 * (E2 + b) - alpha2 * k22 - s * alpha1 * k12;
        Lobj = -0.5*L1*L1*k11 -0.5*L2*L2*k22 - s*L1*L2*k12 - L1*f1 - L2*f2;
        Hobj = -0.5*H1*H1*k11 -0.5*H2*H2*k22 - s*H1*H2*k12 - H1*f1 - H2*f2;
        if (epsEq(Lobj, Hobj)) a2 = alpha2;
        else if (Lobj < Hobj) a2 = H2;
        else a2 = L2;
    }

    if (epsEq(a2, alpha2, epsilon*(a2 + alpha2 + epsilon))) return false;

    a1 = alpha1 + s * (alpha2 - a2);
    if (a1 < 0) a1 = 0;

    bOld = b;
    b1 = E1 + y1 * (a1 - alpha1) + y2 * (a2 - alpha2) * rbf(rbfconst, x1, x2) + bOld;
    b2 = E2 + y2 * (a2 - alpha2) + y1 * (a1 - alpha1) * rbf(rbfconst, x1, x2) + bOld;

    if (epsEq(b1, b2)) b = b1;
    else if (!inOpenInterval(a1, 0, C) && !inOpenInterval(a2, 0, C)) b = (b1+b2) / 2.;
    else if (inOpenInterval(a1, 0, C)) b = b1;
    else b = b2;

    /* uprava chyb */
    // for (unsigned i = 0; i < errors.size(); ++i) {
    //     errors[i] += y1*(a1-alpha1)*rbf(x1,getExample(i)) + 
    //                  y2*(a2-alpha2)*rbf(x2,getExample(i));
    // }

    alpha1 = a1;
    alpha2 = a2;
    computeErrors();
    return true;
}

const std::vector<double> & SVM::getExample(const unsigned i) {
    return (i < positives.size()) ? positives[i] : negatives[i-positives.size()];
}

void SVM::SMO_Learning() {
    int changes;
    for (unsigned o = 0; o < 2; o++)
    do {
        changes = 0;
        for (unsigned i = 0, e = positives.size() + negatives.size(); i < e; ++i)   {
            changes += SMO_OptimizeOne(i);            
        }
        // iteration += changes;
    } while (changes > 0);
    // std::cerr << "iterations: " << iteration << '\n';
}

double SVM::calculateOutput(const std::vector<double> & x) {
    double outputSum = -b;
    for (unsigned ni = 0, e = negatives.size(); ni < e; ++ni) 
    {
        // if (negSkip.find(ni) != negSkip.end())
        if (negAlphas[ni] != 0)
        outputSum -= negAlphas[ni] * rbf(rbfconst, x, negatives[ni]);
    }
    for (unsigned pi = 0, e = positives.size(); pi < e; ++pi) 
    {
        // if (posSkip.find(pi) != posSkip.end())
        if (posAlphas[pi] != 0)
        outputSum += posAlphas[pi] * rbf(rbfconst, x, positives[pi]);
    }
    return outputSum;
}

bool SVM::classifyOne(const std::vector<double> & x) { 
    return calculateOutput(x) > 0;
}

const std::vector<double> & SVM::getSupportVector(unsigned i, bool positiveCls) {
    unsigned c = 0;
    for (auto a : ((positiveCls)?posAlphas:negAlphas)) {
        if (a != 0) {
            if (0 == i) { if (positiveCls) return positives[c]; else return negatives[c];}
            --i;
        }
        ++c;
    }
    --c;
    if (positiveCls) return positives[c]; else return negatives[c];
}

unsigned SVM::supportVectorsCount(bool positiveCls) {
    unsigned c = 0;
    for (auto a : ((positiveCls)?posAlphas:negAlphas)) {
        if (a != 0) ++c;
    }
    return c;
}

SVM::SVM(std::vector<std::vector<double>> & pos, std::vector<std::vector<double>> & neg, double rbfc, double C) :
    positives(pos), negatives(neg), rbfconst(-0.5/(rbfc*rbfc)), b(0), C(C)
{
    // std::cerr << "SVM Begin!\n";
    if (rbfc == -123.45 ) rbfconst = -0.5/pos[0].size();
    negAlphas.resize(negatives.size());
    posAlphas.resize(positives.size());
    errors.resize(negatives.size()+positives.size());
    computeErrors();
    SMO_Learning();
    // for (auto a : posAlphas) std::cerr << a << '\t';
    // for (auto a : negAlphas) std::cerr << a << '\t';
    // std::cerr << '\n' ;
    // double c = 0;
    // for (auto e : positives) c += classifyOne(e) == true;
    // for (auto e : negatives) c += classifyOne(e) == false;
    // std::cerr << "Train error " << 100 - c*100. / (positives.size()+negatives.size()) << "% \n";
}

