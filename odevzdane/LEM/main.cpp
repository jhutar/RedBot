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

#include <cstdio>
#include <vector>
#include <utility>
#include <set>
#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <algorithm>
#include <limits>

#include "Lem.h"

struct mesto
{
    int x;
    int y;
    unsigned obyvatel;
};

struct obchod
{
    int x;
    int y;
    double cena;
    double zisk;
};

struct hrac
{
    double penez;
    unsigned obchodu;
    double posledni_zisk;
};

struct game_info
{
    unsigned kolo;
    unsigned rozmer;
    struct hrac hraci[2];
    std::vector<struct mesto> mesta;
    std::vector<struct obchod> obchody;    

};

using GameInfo = struct game_info;

namespace {
    unsigned groups = 2;
    double maxGroupSize = 0.1, byLearning = 0.7, byEvolution = 0.2;
    unsigned generations = 100, progressProbe = 3, mutationProbe = 4;

    Specimen createSpecimen(unsigned dimensions) {
        std::pair<double, double> range({1.8, 5.1});
        Specimen s(dimensions);
        for (unsigned i = 0; i < dimensions; ++i) {
            s[i] = range;
        }
        return s;
    }

    //
    // GameInfo - zpracovani vstupu
    //

    GameInfo gi;
    unsigned player_id, opponent_id;
    const unsigned CENA_STAVBY = 250;
    const unsigned NAJEM_STAVBY = 30;
    std::vector<bool> obchody;
    std::vector<double> mapa;
    std::vector<unsigned> navstevyObchodu;
    std::vector<double> vahy;


    //
    // zpracovani vstupu
    //
    enum formStrNames {KOLO_FMT, ROZMER_FMT, MESTA_FMT, COORDS_FMT, HRAC_FMT, ZISK_FMT };
    std::string formstrs[] = {
        "Kolo: %u\n",
        "Rozmer: %u\n",
        "Mesta: %u\n",
        "%d %d %u\n",
        "Hrac%u: %lf euro %u obchodu\n",
        "%u %u %lf %lf\n"
    };

    void parsePlayfield() {
        unsigned cnt, p;
        int x,y;
        double zisk, cena;
        FILE *fin = NULL;
        fin = fopen("playfield.txt", "r");

        if (fin != NULL &&
            fscanf(fin, formstrs[KOLO_FMT].c_str(), &gi.kolo) &&
            fscanf(fin, formstrs[ROZMER_FMT].c_str(), &gi.rozmer) &&
            fscanf(fin, formstrs[MESTA_FMT].c_str(), &cnt)) {
            while (cnt-- && (fscanf(fin, formstrs[COORDS_FMT].c_str(), &x, &y, &p ))) {
                gi.mesta.emplace_back(mesto{x,y,p});
            }       
            // hraci
            for (unsigned hrac_id = 0; hrac_id < 2; ++hrac_id) {
                gi.hraci[hrac_id].posledni_zisk = 0;
                if (fscanf(fin, formstrs[HRAC_FMT].c_str(), &p, &gi.hraci[hrac_id].penez, &gi.hraci[hrac_id].obchodu)) {
                    cnt = gi.hraci[hrac_id].obchodu;
                    while (cnt-- && (fscanf(fin, formstrs[ZISK_FMT].c_str(), &x, &y, &cena, &zisk ))) {
                        gi.obchody.emplace_back(obchod{x, y, cena, zisk});
                        gi.hraci[hrac_id].posledni_zisk += zisk;
                    }
                }
            }
        }
        fclose(fin);
    }

    //
    // RANDOM GENERATOR
    //
    static std::random_device rd;
    static std::default_random_engine r(rd());
    unsigned rand_num(unsigned max) {
        std::uniform_int_distribution<unsigned> d(0, max);
        return d(r);
    }   
    double rand_double(double max) {
        std::uniform_real_distribution<double> d(0.0, max);
        return d(r);
    }   

    //
    // 2D -> 1D
    //  
    template <typename TI>
    TI coordConv(unsigned rozmer, TI x, TI y) {
        return y * rozmer + x;
    }

    template <typename TPOS>
    unsigned coordConv(unsigned rozmer, TPOS &o) {
        return coordConv(rozmer, o.x, o.y);
    }   

    //
    // 1D -> 2D
    //  
    std::pair<int, int> backCoordConv(unsigned rozmer, int x) {
        return std::pair<int, int>{x % rozmer, x / rozmer};
    }


    //
    // Manhattonovska metrika
    //
    template <typename TD>
    TD absdist(TD x, TD y) {
        return (x > y) ? x - y : y - x;
    }

    unsigned manhattonDistance(unsigned x1, unsigned y1, unsigned x2, unsigned y2) {
        return absdist(x1, y1) + absdist(x2, y2);
    }

    template <typename T1, typename T2>
    unsigned manhattonDistance(T1 &o1, T2 &o2) {
        return manhattonDistance(o1.x, o1.y, o2.x, o2.y);
    }

    template <typename TD> 
    TD mabs(TD x) {
        return absdist(x, 0);
    }

    std::set<std::pair<int, int>> manhattonDistanceFields(const int px, const int py, const int distance = 0, const unsigned rozmer = gi.rozmer) {
        std::set<std::pair<int, int>> fields;
        for (int dx = -distance; dx <= distance; ++dx) {
            for (int dy = -distance; dy <= distance; ++dy) {
                if (mabs(dx) + mabs(dy) != distance || 
                    px + dx < 0 || px + dx >= rozmer || 
                    py + dy < 0 || py + dy >= rozmer) {continue;}
                fields.insert(std::pair<int, int>{px + dx, py + dy});
            }
        }
        return fields;
    }

    template <typename TPOS>
    std::set<std::pair<int, int>> manhattonDistanceFields(TPOS & p, const int distance = 0, const unsigned rozmer = gi.rozmer) {
        return manhattonDistanceFields(p.x, p.y, distance, rozmer);
    }

    std::set<std::pair<int, int>> bestFields(const double tolerance = 0.9, const unsigned rozmer = gi.rozmer) {
        std::set<std::pair<int, int>> fields;
        double bval = std::numeric_limits<double>::lowest();
        for (int y = 0; y < rozmer; ++y) {
            for (int x = 0; x < rozmer; ++x) {
                if (bval < mapa[coordConv(rozmer, x, y)]) {
                    bval = mapa[coordConv(rozmer, x, y)];
                }
            }
        }

        bval *= tolerance;

        for (unsigned i = 0; i < mapa.size(); ++i) {
            if (mapa[i] < bval) continue;
            fields.insert(backCoordConv(rozmer, i));            
        }

        return fields;
    }

    //
    // rovnice zisku: f(cena) = cena - 1 - (cena^2)/10
    // nejvetsi zisk je pro cenu = 5, f(5) = 1.5
    // pro cenu > 8.8 je zisk zaporny
    template <typename TZ>
    double zisk(TZ cena) {
        return cena - 1 - cena * cena / 10.0;
    }


    //
    // Váha obchodu
    // vaha_obchodu = 1 / ( (1+manhatanska_vzdalenost) * (1+(cena_v_obchode^3)) )
    double vahaObchodu(unsigned x1, unsigned y1, unsigned x2, unsigned y2, double cena) {
        return 1.0 / ((1 + manhattonDistance(x1, y1, x2, y2)) * (1 + pow(cena, 3) ));
    }

    template <typename TPOS>
    double vahaObchodu(const TPOS &m, unsigned x, unsigned y, double cena) {
        return vahaObchodu(m.x, m.y, x, y, cena);
    }

    template <typename TPOS>
    double vahaObchodu(const TPOS &m, const struct obchod &o) {
        return vahaObchodu(m.x, m.y, o.x, o.y, o.cena);
    }

    //
    // Pravdepodobnostni kolo - dle PlayField.cpp
    //
    // vraci index obchodu
    unsigned koloStesti(const double cil, const std::vector<double> & vahyObchodu) {
        double tmpsum = 0;
        unsigned i = 0;

        // std::vector<std::pair<unsigned, unsigned>> ov(vahyObchodu.size());
        // for (i = 0; i < vahyObchodu.size(); ++i) {
        //     ov[i].first = i;
        // }
        // std::sort(ov.begin(), ov.end(), [&vahyObchodu](const std::pair<unsigned, unsigned> &lhs, const std::pair<unsigned, unsigned> & rhs) {return vahyObchodu[lhs.first] < vahyObchodu[rhs.first];});
        // for (i = 0; i < ov.size(); ++i) {
        //     ov[i].second = i + 1;
        // }

        // for (i = 0; i < ov.size() - 1; ++i) {
        //     if (cil <= tmpsum) break;
        //     tmpsum += ov[i].second;
        // }
        // return ov[i].first;
        do {
            tmpsum += vahyObchodu[i++];
        } while (tmpsum < cil);
        // std::cerr << tmpsum <<  ", " << cil << '\n';
        return i-1;
    }

    template <typename TPOS>
    void sumaAVahy(TPOS &m, double & suma, std::vector<double> & vahy, const std::vector<struct obchod> & obchody = gi.obchody) {
        vahy.clear();
        vahy.reserve(obchody.size());
        suma = 0;
        unsigned i = 1;
        for (auto & o : obchody) {
            double vaha = vahaObchodu(m, o);
            suma += vaha;
            vahy.emplace_back(vaha);
        }        
    }

    template <typename TPOS>
    unsigned koloStesti(const TPOS &m, const std::vector<struct obchod> & obchody = gi.obchody) {
        std::vector<double> vahy;
        vahy.reserve(obchody.size());
        double suma;
        sumaAVahy(m, suma, vahy, obchody);
        return koloStesti(rand_double(suma), vahy);
    }
    
    using ObchodIterator = std::vector<struct obchod>::iterator;
    using IteratoryObchodu = std::pair<ObchodIterator, ObchodIterator>;
    IteratoryObchodu iteratoryObchoduHrace(unsigned pi = player_id) {
        ObchodIterator s, e;
        s = gi.obchody.begin() + ((pi == 0) ? 0 : gi.hraci[0].obchodu);
        e = s + gi.hraci[pi].obchodu;
        return {s,e};
    }

    unsigned navstevyObchoduHrace(unsigned pi = player_id) {
        std::vector<unsigned>::iterator s, e;
        s = navstevyObchodu.begin() + ((pi == 0) ? 0 : gi.hraci[0].obchodu);
        e = s + gi.hraci[pi].obchodu;
        return std::accumulate(s,e,0);
    }

    //
    // nastaveni cen v obchodech hrace 
    //
    void nastavCeny(std::vector<double> &ceny, unsigned pi = player_id) {
        IteratoryObchodu its = iteratoryObchoduHrace(pi);
        for (unsigned i = 0; its.first != its.second; ++i, ++its.first) {
            its.first->cena = ceny[i];
        }
    }

    //
    // zkusi se odhadnout, kam pujdou lidi nakupovat
    //
    void spoctiNavstevy() {
        double suma = 0;
        std::fill(navstevyObchodu.begin(), navstevyObchodu.end(), 0);
        for (auto &m : gi.mesta) {
            sumaAVahy(m, suma, vahy);
            for (unsigned oi = 0; oi < m.obyvatel; ++oi) {
                unsigned index = koloStesti(rand_double(suma), vahy);
                ++navstevyObchodu[index];
            }
        }        
    }

    //
    // dle odhadu navstevnosti obchodu vypocte jejich zisky 
    //
    void vypoctiZisky(std::vector<unsigned> &obyvatele = navstevyObchodu) {
        for (unsigned i = 0; i < obyvatele.size(); ++i) {
            gi.obchody[i].zisk = obyvatele[i] * zisk(gi.obchody[i].cena) - NAJEM_STAVBY;
        }
    }

    //
    // zisk hrace dle odhadu
    //
    double ziskHrace(unsigned pi = player_id) {
        double suma = 0;
        IteratoryObchodu its = iteratoryObchoduHrace(pi);
        for (; its.first != its.second; ++its.first) {
            suma += its.first->zisk;
        }
        return suma;
    }

    //
    // Fitness - maximalizace rozdilu zisku hracu
    //         
    double fitness(std::vector<double> &representation) {
        double ff = 0;
        nastavCeny(representation);
        spoctiNavstevy();
        vypoctiZisky();

        ff = ((ziskHrace(player_id) > ziskHrace(opponent_id)) ? 1 : -1) * absdist(ziskHrace(0), ziskHrace(1));
        return ff;
    }

    double opponentFitness(std::vector<double> &representation) {
        double ff = 0;
        nastavCeny(representation, opponent_id);
        spoctiNavstevy();
        vypoctiZisky();

        ff = ((ziskHrace(opponent_id) > ziskHrace(player_id)) ? 1 : -1) * absdist(ziskHrace(0), ziskHrace(1));
        return ff;
    }
    
    void mapaVah() {
        for (unsigned y = 0; y < gi.rozmer; ++y) {
            for (unsigned x = 0; x < gi.rozmer; ++x)  {
                std::cerr << mapa[coordConv(gi.rozmer, x, y)] << '\t';
            }
            std::cerr << '\n';
        }
    }

    // ma se stavet ?
    bool stavet() {
        return (gi.hraci[player_id].obchodu == 0 
                || gi.hraci[player_id].penez > 1.5*CENA_STAVBY 
                    && gi.hraci[opponent_id].posledni_zisk  > gi.hraci[player_id].posledni_zisk
                    && gi.hraci[opponent_id].obchodu  > gi.hraci[player_id].obchodu + 1);
    }

    //
    // Odpoved
    //
    void answer() {
        std::string s;
        unsigned x,y;
        int i = 100;

        if (gi.hraci[player_id].obchodu > 0) {
            if (gi.hraci[opponent_id].obchodu > 0) { // odhadne se, jak by nastavil ceny protihrac
                Specimen specimen = createSpecimen(gi.hraci[opponent_id].obchodu);
                Lem l(opponentFitness, specimen, 20 * gi.hraci[opponent_id].obchodu, byLearning, byEvolution, groups, maxGroupSize, 0);
                l.run(generations / 2, progressProbe, mutationProbe);
                Statistic stat = l.getStatistic();
                // nastaveni cen protihrace
                opponentFitness(stat.representations.second[0]);
            }
            Specimen specimen = createSpecimen(gi.hraci[player_id].obchodu);
            Lem l(fitness, specimen, 20 * gi.hraci[player_id].obchodu, byLearning, byEvolution, groups, maxGroupSize, 0);
            l.run(generations, progressProbe, mutationProbe);
            Statistic stat = l.getStatistic();
            fitness(stat.representations.second[0]);
            for (double chi : stat.representations.second[0]) {
                s += std::to_string(chi);
                s += ";";
            }
        }

        gi.hraci[player_id].penez -= gi.hraci[player_id].obchodu * NAJEM_STAVBY;
        if (stavet()) {
            {
                // mapaVah();
                std::set<std::pair<int, int>> fields = bestFields();
                if (fields.empty()) {
                    do {
                        x = rand_num(gi.rozmer-1);
                        y = rand_num(gi.rozmer-1);
                    } while ((i-- > 0) && (obchody[coordConv(gi.rozmer, x, y)]));
                } else {
                    unsigned fi = rand_num(fields.size() - 1);
                    for (auto & field : fields) {
                        if (fi == 0) {
                            x = field.first;
                            y = field.second;
                        }
                        --fi;
                    }
                }
                s += "n ";
                s += std::to_string(x);
                s += " ";
                s += std::to_string(y);
                s += ";";
                gi.hraci[player_id].penez -= CENA_STAVBY;
            }
        }
        // vypis odpovedi
        std::cout << s << std::endl;
    }
}


int main(int argc, char const *argv[])
{
    if (argc != 2) {
        return 1;       
    }
    std::stringstream(std::string(argv[1])) >> player_id;
    --player_id;
    opponent_id = player_id ^ 1;
    parsePlayfield();
    obchody.resize(gi.rozmer * gi.rozmer, false);
    mapa.resize(gi.rozmer * gi.rozmer, 0.);
    navstevyObchodu.resize(gi.obchody.size(), 0);
    vahy.resize(gi.obchody.size());

    // zjisteni nejlepsich mist pro stavbu obchodu
    const int maxmd = 4;
    for (auto & m : gi.mesta) {
        for (int md = 0; md < maxmd; ++md) {
            for (auto & field : manhattonDistanceFields(m, md)) {
                mapa[coordConv(gi.rozmer, field.first, field.second)] = m.obyvatel * (100. - md * 10) / 100.0;
            }
        }
    }

    for (unsigned hrac = 0; hrac < 2; ++hrac) {
        IteratoryObchodu oits = iteratoryObchoduHrace(hrac);
        for (auto oi = oits.first; oi != oits.second; ++oi) {
            const double cena = mapa[coordConv(gi.rozmer, *oi)];
            for (int md = 0; md < maxmd; ++md) {
                for (auto & field : manhattonDistanceFields(*oi, md)) {
                    if (hrac == player_id) {
                        mapa[coordConv(gi.rozmer, field.first, field.second)] -= cena * (100 - md*15 ) / 100.0;
                    }
                    else 
                        mapa[coordConv(gi.rozmer, field.first, field.second)] *= 4./ 5.0;
                }            
            }
        }
    }

    for (auto & obchod : gi.obchody) {
        obchody[coordConv(gi.rozmer, obchod)] = true;
        mapa[coordConv(gi.rozmer, obchod)] = std::numeric_limits<double>::lowest();
    }
  
    // vlastni odpoved
    answer();

    return 0;
}

