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

#ifndef SVM_H
#define SVM_H

#include <vector>
#include <set>

class SVM
{
public:
    SVM(std::vector<std::vector<double>> & positives, std::vector<std::vector<double>> & negatives, double rbfconst = -123.45, double C=10);
    bool classifyOne(const std::vector<double> & x);
    const std::vector<double> & getSupportVector(unsigned i, bool positives = true);
    unsigned supportVectorsCount(bool positives = true);
private:
    double SMO_Error(const std::vector<double> & x1, const int y1);
    double SMO_Error(const unsigned int index);
    bool SMO_ViolatesKKT(const double alpha, const int y, const double E);
    bool SMO_OptimizationStep(double & alpha1, const std::vector<double> & x1, const int y1, double E1, 
                          double & alpha2, const std::vector<double> & x2, const int y2, double E2);
    void SMO_Learning();
    bool SMO_OptimizeOne(double & alpha1, const std::vector<double> & x1, const int y1, double E1);
    bool SMO_OptimizeOne(const unsigned int index);
    const std::vector<double> & getExample(const unsigned i);

    void computeErrors();
    double calculateOutput(const std::vector<double> & x1);

    std::vector<std::vector<double>> & positives;
    std::vector<std::vector<double>> & negatives;
    std::vector<double> negAlphas;
    std::vector<double> posAlphas;
    std::vector<double> errors;
    std::set<unsigned> posSkip;
    std::set<unsigned> negSkip;
    double rbfconst, b, C;
};



#endif