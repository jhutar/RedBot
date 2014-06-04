#ifndef WALLSMART_STRUKTURA
#define WALLSMART_STRUKTURA

#include <vector>
#include <cmath>
using namespace std;

struct Pozice{
	int x, y;

	Pozice(){
	}
	Pozice(int x, int y)
	: x(x), y(y){
	}

	int vzdalenost(Pozice & p){
		return abs(x-p.x)+abs(y-p.y);
	}
	bool operator<(const Pozice & p) const{
		return x == p.x ? (y < p.y) : (x < p.x);
	}
	bool operator==(const Pozice & p) const{
		return (x == p.x) && (y == p.y);
	}
};

struct Kandidat{
	Pozice pozice;
	double hodnota;

	Kandidat(){
	}
	Kandidat(Pozice pozice, double hodnota)
	: pozice(pozice), hodnota(hodnota){
	}

	bool operator<(const Kandidat & k) const{
		return hodnota == k.hodnota ? (pozice < k.pozice) : (hodnota > k.hodnota);
	}
};

enum TypAkce{
	CENA,
	ZRUS,
	NOVY
};

struct Akce{
	TypAkce typ;
	Pozice pozice;
	float cena;

	Akce(){
	}
	Akce(TypAkce typ)
	: typ(typ){
	}
	Akce(TypAkce typ, Pozice pozice)
	: typ(typ), pozice(pozice){
	}
	Akce(TypAkce typ, float cena)
	: typ(typ), cena(cena){
	}
	Akce(TypAkce typ, Pozice pozice, float cena)
	: typ(typ), pozice(pozice), cena(cena){
	}
	bool operator<(const Akce & a) const{
		return typ == a.typ ? pozice < a.pozice : typ < a.typ;
	}
};


struct Mesto{
	static const int MAX_POPULACE = 100;
	Pozice pozice;
	int populace;

	Mesto(){
	}
	Mesto(Pozice & pozice, int populace)
	: pozice(pozice), populace(populace){
	}

	bool operator<(const Mesto & m) const{
		return populace == m.populace ? (pozice < m.pozice) : (populace > m.populace);
	}
};

struct Obchod{
	static constexpr float MAX_CENA = 10; // dano pravidly redbotu
	static constexpr float OPT_CENA = 5; // maximalni marze
	static constexpr float MIN_CENA = 1.12702; // nulova marze
	static constexpr float NAJEMNE = 30;
	static constexpr float NOVY = 250;
	int cislo; // poradi obchodu na vstupu
	Pozice pozice;
	float cena; // ma smysl pouze v rozmezi [MIN_CENA, OPT_CENA]
	float novaCena;
	float zisk;

	Obchod(){
	}
	Obchod(Pozice & pozice, float cena, float zisk)
	: pozice(pozice), cena(cena), novaCena(cena), zisk(zisk){
	}

	float vaha(Pozice & p){
		return 1.0 / ( (1+pozice.vzdalenost(p)) * (1+(cena*cena*cena)) );
	}
	// maximalni marze: marze(OPT_CENA) = 1.5
	// nulova marze:    marze(MIN_CENA) = 0
	float marze(){
		return cena - 1 - (cena*cena)/10;
	}
	bool operator<(const Obchod & o) const{
		return zisk == o.zisk ? (pozice < o.pozice) : (zisk < o.zisk);
	}
};

struct Hrac{
	static const int MAX_CISLO = 2;
	int cislo;
	float pocetPenez;
	float zisk;
	int pocetObchodu;
	vector<Obchod> obchody;
	vector<Akce> akce;

	Hrac()
	: cislo(0){
	}
	Hrac(int pocetPenez, int pocetObchodu, vector<Obchod> & obchody, vector<Akce> & akce)
	: pocetPenez(pocetPenez), pocetObchodu(pocetObchodu), obchody(obchody), akce(akce){
	}
};

struct Kolo{
	static const int MAX_CISLO = 1000;
	int cislo;
	int rozmer;
	int pocetMest;
	int pocetHracu;
	vector<Mesto> mesta;
	vector<Hrac> hraci;

	Kolo()
	: pocetHracu(Hrac::MAX_CISLO){
	}
	Kolo(int cislo, int rozmer, vector<Mesto> & mesta, vector<Hrac> & hraci)
	: cislo(cislo), rozmer(rozmer), mesta(mesta), hraci(hraci){
		pocetMest = mesta.size();
		pocetHracu = hraci.size();
	}
};
#endif
