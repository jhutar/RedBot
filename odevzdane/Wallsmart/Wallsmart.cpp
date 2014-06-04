#include <iostream>
#include <fstream>
#include <array>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <chrono>
#include <random>
#include <utility>
#include "Struktura.cpp"
#include "IO.cpp"
using namespace std;

const string playfield = "playfield.txt";
const int MESTO = 10;
const int ROZESTUP = 9;
const int LIDNATOST = 30;

// inicializace nahodnych generatoru
default_random_engine rnd(chrono::system_clock::now().time_since_epoch().count());
normal_distribution<float> randn(0, 1);
uniform_real_distribution<float> randu(0, 1);
normal_distribution<float> randnCena(0.1, 0.1); // pro cenu
uniform_real_distribution<float> randuCena(Obchod::MIN_CENA, 3);

// promenne pro strategii
Kolo kolo;
map<Pozice, int> mapa;
int id;

bool chciStavet(Hrac & ja, Hrac & on){
	return (ja.pocetObchodu == 0 && ja.pocetPenez > Obchod::NOVY)
	|| (ja.pocetObchodu == 1 && ja.pocetPenez > 2*Obchod::NOVY)
	|| (ja.pocetPenez >= Obchod::NOVY + ja.pocetObchodu * ja.pocetObchodu * 600);
}

bool chciExpandovat(Hrac & ja, Hrac & on){
	return (ja.pocetObchodu == 0 && ja.pocetPenez > Obchod::NOVY)
	|| (ja.pocetObchodu == 1 && ja.pocetPenez > 2*Obchod::NOVY)
	|| (ja.zisk > Obchod::NAJEMNE && (ja.pocetPenez < on.pocetPenez+500 || on.zisk > 100) && ja.pocetPenez >= Obchod::NOVY + ja.pocetObchodu * ja.pocetObchodu * 600);
}

// udrzujeme cenu v rozmezi MIN_CENA+0.5, OPT_CENA
float cena(float c){
	if(c > Obchod::MAX_CENA) // novy obchod dostane novou "nahodnou" cenu
		c = 3+3*randnCena(rnd); // TODO jakou hodnotu nastavit?
	c = max(Obchod::MIN_CENA+0.5f, c);
	c = min(Obchod::OPT_CENA-0.0f, c);
	return c;
}

// zvolime novou cenu
float cena(float mojeCena, float jehoCena, float mujZisk, float jehoZisk){
	if(mujZisk > jehoZisk){
		if(jehoCena > Obchod::MAX_CENA) // on ma novy obchod
			return cena(mojeCena-randnCena(rnd)*mojeCena); // trochu snizime cenu, aby nas nepodstrelil
		else
			return mojeCena; // mame lepsi cenu
	}
	if(mojeCena > Obchod::MAX_CENA && jehoCena > Obchod::MAX_CENA) // oba jsme postavili obchod
		return cena(mojeCena); // nahodna cena
	if(mojeCena > Obchod::MAX_CENA && jehoCena < Obchod::MAX_CENA) // my mame novy obchod
		return cena(jehoCena-randnCena(rnd)); // podstrelime cenu
	if(mojeCena < Obchod::MAX_CENA && jehoCena < Obchod::MAX_CENA){ // oba mame fungujici obchody
		if(mojeCena >= jehoCena){ // prodavame moc draho, musime zlevnit
			return cena(mojeCena-randnCena(rnd)*mojeCena);
		}else{ // prodavame moc levno, musime zdrazit
			return cena(mojeCena+randnCena(rnd)*5/mojeCena);
		}
	}
	return mojeCena;
}

// otestuje, zda je to platna pozice
bool uvnitr(Pozice & p){
	return p.x >= 0 && p.x < kolo.rozmer && p.y >= 0 && p.y < kolo.rozmer;
}

// najde volne misto v nejblizsim okoli
Pozice nejblizsiVolna(Pozice & kde){
	// hledame do Manhattanske vzdalenosti 2
	static array<Pozice, 11> posun = {Pozice(0,0),Pozice(0,-1),Pozice(0,1),Pozice(-1,0),Pozice(1,0),Pozice(-1,-1),Pozice(1,1),Pozice(0,-2),Pozice(0,2),Pozice(-2,0),Pozice(2,0)};
	shuffle(posun.begin(), posun.end(), rnd);
	Pozice nejlepsi = Pozice(-1,-1);
	for(Pozice p : posun){
		p.x += kde.x;
		p.y += kde.y;
		if(uvnitr(p) && (mapa.find(p) == mapa.end() || mapa[p] == MESTO)){
			if(!uvnitr(nejlepsi) || kde.vzdalenost(p) < kde.vzdalenost(nejlepsi))
				nejlepsi = p;
		}
	}
	return nejlepsi;
}

// vrati seznam akci, kde se nam nepovedlo postavit obchod
vector<Akce> neprovedeneStavby(Hrac & hrac){
	vector<Akce> stavby;
	// projdeme akce NOVY
	for(Akce & a : hrac.akce){
		if(a.typ != TypAkce::NOVY)
			continue;
		// podivame se, jestli takovy obchod opravdu mame
		bool nalezeno = false;
		for(Obchod & o : hrac.obchody){
			if(o.pozice == a.pozice){
				nalezeno = true;
				break;
			}
		}
		// pokud jsme nenasli, tak stavba nebyla provedena
		if(!nalezeno){
			stavby.push_back(a);
		}
	}
	return stavby;
}

void printVyhodnost(map<Pozice, double> & v){
	for(int y=0; y<kolo.rozmer; y++){
		for(int x=0; x<kolo.rozmer; x++){
			Pozice p(x, y);
			if(v.find(p) == v.end())
				cerr << 0 << " ";
			else
				cerr << v[p] << " ";
		}
		cerr << endl;
	}
}

// spocita vyhodnost policek na stavbu obchodu
vector<Kandidat> vyhodnost(vector<Mesto> & mesta){
	int delta = 10;
	map<Pozice, double> v;
	for(Mesto & m : mesta){
		for(int i=-delta; i<=delta; i++){
			for(int j=-delta; j<=delta; j++){
				Pozice p(m.pozice.x+i, m.pozice.y+j);
				if(uvnitr(p) && (mapa.find(p) == mapa.end() || mapa[p] == MESTO)){ // je tam volno nebo je tam mesto
					if(v.find(p) == v.end())
						v[p] = 0;
					v[p] += m.populace / (1.0+p.vzdalenost(m.pozice));
				}
			}
		}
	}
	//printVyhodnost(v);
	vector<Kandidat> kandidati;
	transform(v.begin(), v.end(), back_inserter(kandidati), [](map<Pozice, double>::value_type & val){return Kandidat(val.first, val.second);} );
	sort(kandidati.begin(), kandidati.end());
	return kandidati;
}

// vrati seznam objektu v dane blizkosti pozice
template <typename T>
vector<T> blizkeObjekty(Pozice & pozice, vector<T> & objekty, int vzdalenost){
	vector<T> blizke;
	for(T & o : objekty){
		if(pozice.vzdalenost(o.pozice) <= vzdalenost){
			blizke.push_back(o);
		}
	}
	return blizke;
}


// vypise na vystup vsechny akce ve spravnem poradi
void vystup(vector<Akce> akce, Hrac & ja){
	// zapiseme akce do mapy
	map<Pozice, Akce> zmena;
	// chceme, aby drivejsi akce mely prednosti pred pozdejsimi
	// budeme tedy vkladat zmeny odzadu, nakonec zustane ta drivejsi na dane pozici
	reverse(akce.begin(), akce.end());
	for(Akce & a : akce){
		zmena[a.pozice] = a;
	}
	akce.clear();
	// vypiseme zmeny cen
	for(Obchod & o : ja.obchody){
		if(zmena.find(o.pozice) != zmena.end() && zmena[o.pozice].typ == TypAkce::CENA){ // pokud doslo ke zmene ceny v akcich
			cout << zmena[o.pozice] << ";"; // vypiseme danou zmenu
		}else{ // jinak nechame cenu obchodu nezmenenou
			cout << Akce(TypAkce::CENA, cena(o.cena)) << ";"; // toto osetruje, pokud cena obchodu jeste nebyla nastavena
		}
	}
	// vypiseme ruseni obchodu a nove obchody
	transform(zmena.begin(), zmena.end(), back_inserter(akce), [](map<Pozice, Akce>::value_type & val){return val.second;} );
	sort(akce.begin(), akce.end());
	for(Akce & a : akce){
		if(a.typ == TypAkce::CENA)
			continue;
		cout << a << ";";
	}
	if(ja.obchody.size() == 0){
		cout << "n"; // novy radek
	}
	cout << endl;
}

// strategie, ktera konkuruje souperi
vector<Akce> konkuruj(Hrac & ja, Hrac & on){
	// vystupni akce
	vector<Akce> akce;
	// jaky musi mit obchod minimalni zisk, abychom mu chteli konkurovat
	constexpr float minimalniZisk = 0;
	// v jake vzdalenosti chceme nas obchod
	const int maxVzdalenost = 9;

	vector<Obchod> mojeObchody(ja.obchody);
	vector<Obchod> jehoObchody(on.obchody);

	// seradime souperovy obchody podle zisku
	// na zacatku jsou nejvynosnejsi obchody
	sort(jehoObchody.rbegin(), jehoObchody.rend());
	sort(mojeObchody.rbegin(), mojeObchody.rend());

	for(Obchod & so : jehoObchody){
		// chceme konkurovat jenom vynosnym obchodum
		if(so.zisk < 0)
			break;
		vector<Obchod> blizkeObchody = blizkeObjekty(so.pozice, mojeObchody, ja.zisk >= 100 ? 3 : maxVzdalenost);
		if(blizkeObchody.size() == 0){
			if(!chciStavet(ja, on) || so.zisk < minimalniZisk || ja.zisk > on.zisk) // nemame dost penez, nebo ten obchod je na prd, nebo vydelavame vic
				continue;
			// nemame nic blizko, postavime novy
			// najdeme mesto v blizkosti
			vector<Mesto> blizkaMesta = blizkeObjekty(so.pozice, kolo.mesta, maxVzdalenost);
			// seradime je podle poctu obyvatel, nejdriv lidnatejsi
			sort(blizkaMesta.begin(), blizkaMesta.end());
			for(Mesto & m : blizkaMesta){
				Pozice p = nejblizsiVolna(m.pozice);
				if(uvnitr(p)){
					ja.pocetObchodu += 1;
					ja.pocetPenez -= Obchod::NOVY;
					mojeObchody.push_back(Obchod(p, Obchod::MAX_CENA, 0));
					akce.push_back(Akce(TypAkce::NOVY, p)); // novy obchod
					break;
				}
			}
		}else{
			// nas nejvynosnejsi obchod
			//Obchod & mo = blizkeObchody[0];
			for(Obchod & mo : blizkeObchody){
				if(mo.zisk > so.zisk){
					// vynasi to, nedelame nic
				}else{
					// zmenime cenu
					akce.push_back(Akce(TypAkce::CENA, mo.pozice, cena(mo.cena, so.cena, mo.zisk, so.zisk)));
				}
			}
		}
	}
	return akce;
}

// strategie, ktera exploruje do mest
vector<Akce> expanduj(Hrac & ja, Hrac & on){
	// vystupni akce
	vector<Akce> akce;
	// stavime obchod ve meste
	vector<Obchod> mojeObchody(ja.obchody);
	sort(kolo.mesta.begin(), kolo.mesta.end());
	for(Mesto & mesto : kolo.mesta){
		if(!chciStavet(ja, on)) // nemame dost penez
			break;
		if(mesto.populace <= LIDNATOST && ja.pocetObchodu > 0) // pak uz jsou jen samy maly mesta
			break;
		if(mapa.find(mesto.pozice) == mapa.end() || mapa[mesto.pozice] == MESTO){ // pokud je mesto volne
			vector<Obchod> blizkeObchody = blizkeObjekty(mesto.pozice, mojeObchody, ja.zisk >= 100 ? 3 : 9);
			if(blizkeObchody.size() == 0){ // a navic nemame v okoli zadny nas obchod
				ja.pocetObchodu += 1;
				ja.pocetPenez -= Obchod::NOVY;
				mojeObchody.push_back(Obchod(mesto.pozice, Obchod::MAX_CENA, 0));
				akce.push_back(Akce(TypAkce::NOVY, mesto.pozice));
				//break;
			}
		}
	}
	return akce;
}

// strategie, ktera exploruje do oblasti
vector<Akce> expandujLip(Hrac & ja, Hrac & on){
	// vystupni akce
	vector<Akce> akce;
	vector<Obchod> mojeObchody(ja.obchody);
	// kandidati serazeni podle vhodnosti
	vector<Kandidat> kandidati = vyhodnost(kolo.mesta);
	for(Kandidat & kandidat : kandidati){
		if(!chciStavet(ja, on)) // nemame dost penez
			break;
		if(kandidat.hodnota <= LIDNATOST && ja.pocetObchodu > 0) // pak uz jsou jen samy maly mesta
			break;
		vector<Obchod> blizkeObchody = blizkeObjekty(kandidat.pozice, mojeObchody, ja.zisk >= 100 ? 3 : 9);
		if(blizkeObchody.size() == 0){ // a navic nemame v okoli zadny nas obchod
			ja.pocetObchodu += 1;
			ja.pocetPenez -= Obchod::NOVY;
			mojeObchody.push_back(Obchod(kandidat.pozice, Obchod::MAX_CENA, 0));
			akce.push_back(Akce(TypAkce::NOVY, kandidat.pozice));
			//break;
		}
	}
	return akce;
}

// strategie, ktera nastavi v obchodech nahodnou cenu
vector<Akce> nahodneCeny(Hrac & ja, Hrac & on){
	// vystupni akce
	vector<Akce> akce;
	uniform_int_distribution<int> rand(0, 10);

	for(Obchod & obchod : ja.obchody){
		if(obchod.cena > Obchod::MAX_CENA){ // obchod jeste nema nastavenou cenu
			vector<Obchod> blizkeObchody = blizkeObjekty(obchod.pozice, on.obchody, 3); // koukneme na pripadne souperovy obchody
			float podstrelenaCena = cena(obchod.cena); // zvolime pocatecni cenu
			for(Obchod & o : blizkeObchody){
				podstrelenaCena = min(podstrelenaCena, cena(obchod.cena, o.cena, obchod.zisk, o.zisk));
			}
			akce.push_back(Akce(TypAkce::CENA, obchod.pozice, cena(podstrelenaCena)));
		}else if(obchod.zisk > 0){ // vydelavame
			if(randu(rnd) < 1/obchod.zisk){ // vetsi vydelek => mensi sance na zmenu
				akce.push_back(Akce(TypAkce::CENA, obchod.pozice, cena(obchod.cena+randnCena(rnd)-0.1f))); // nahodna zmena ceny
			}
		}else{ // prodelavame, musime menit
			akce.push_back(Akce(TypAkce::CENA, obchod.pozice, cena(randuCena(rnd)))); // nahodne uniformne
		}
	}
	return akce;
}

// strategie, ktera dostavi obchody, ktere se nam nepodarilo postavit
vector<Akce> dostav(Hrac & ja, Hrac & on, vector<Akce> & nedostavene){
	// vystupni akce
	vector<Akce> akce;
	for(Akce & a : nedostavene){
		if(!chciStavet(ja, on)) // nemame dost penez
			break;
		// oznacime, ze na dane policko stavet uz nechceme
		// bo pravdepodobne tam souper bude chtit stavet znovu
		mapa[a.pozice] = 0;
		Pozice p = nejblizsiVolna(a.pozice);
		if(uvnitr(p)){
			ja.pocetObchodu += 1;
			ja.pocetPenez -= Obchod::NOVY;
			akce.push_back(Akce(TypAkce::NOVY, p)); // novy obchod
		}
	}
	return akce;
}

// nahodna strategie
// stara strategie, nikde ji nepouzivame
vector<Akce> nahodna(Hrac & ja, Hrac & on){
	vector<Akce> akce;
	uniform_int_distribution<int> rand(0, 500);

	// menime ceny
	for(Obchod & obchod : ja.obchody){
		if(obchod.zisk > 0){
			if(rand(rnd) % 10 == 0){
				obchod.cena += randn(rnd);
			}
		}else{
			if(rand(rnd) % 10 != 0){
				obchod.cena += randn(rnd);
			}
		}
		if(obchod.cena <= Obchod::MIN_CENA || obchod.cena >= obchod.MAX_CENA)
			obchod.cena = randuCena(rnd);
		else if(obchod.cena > obchod.OPT_CENA)
			obchod.cena = obchod.OPT_CENA;
		akce.push_back(Akce(TypAkce::CENA, obchod.pozice, obchod.cena));
	}

	int r = rand(rnd) % 500;
	if(ja.pocetObchodu == 0 || (r == 0 && ja.pocetPenez >= ja.pocetObchodu * 1000)){ // stavime obchod ve meste
		sort(kolo.mesta.begin(), kolo.mesta.end());
		for(Mesto & mesto : kolo.mesta){
			if(mapa.find(mesto.pozice) == mapa.end() || mapa[mesto.pozice] == MESTO){
				akce.push_back(Akce(TypAkce::NOVY, mesto.pozice));
				break;
			}
		}
	}else if(r == 1 && ja.pocetObchodu > 0){ // rusime obchod
		sort(ja.obchody.begin(), ja.obchody.end());
		Obchod & o = ja.obchody[0];
		if(o.zisk < 0){
			akce.push_back(Akce(TypAkce::ZRUS, o.pozice));
		}
	}
	return akce;
}

void pridej(vector<Akce> & akce, const vector<Akce> & nove){
	akce.insert(akce.end(), nove.begin(), nove.end());
}

// kombinace strategie konkurence
// + expandovani
// TODO dodelat strategii
void strategie(){
	Hrac & ja = kolo.hraci[id-1];
	Hrac & on = kolo.hraci[id % 2];
	// vektor akci
	// pokud je pro dany obchod (pozici) vice zmen cen ve vektoru, tak se pouzije ta prvni zmena ceny
	vector<Akce> akce;
	//pridej(akce, nahodna(ja, on));
	vector<Akce> nepostavene = neprovedeneStavby(ja);
	uniform_int_distribution<int> rand(0, 10);
	bool provedenaKonkurence = false;
	if(nepostavene.size() > 0 && chciStavet(ja, on)){
		//cerr << "--nepostavene" << endl;
		pridej(akce, dostav(ja, on, nepostavene)); // zkusime dostavit kousek vedle
	}else if(chciExpandovat(ja, on)){
		//cerr << "--expand" << endl;
		if(kolo.pocetMest > 1000) // kvuli rychlosti
			pridej(akce, expanduj(ja, on)); // expandujeme
		else
			pridej(akce, expandujLip(ja, on)); // expandujeme
	}else{
		//cerr << "--konkur" << endl;
		pridej(akce, konkuruj(ja, on)); // konkurujeme
		provedenaKonkurence = true;
	}
	if(akce.size() == 0 && !provedenaKonkurence){ // nic jsme neudelali
		//cerr << "--konkur2" << endl;
		pridej(akce, konkuruj(ja, on)); // konkurujeme
	}
	pridej(akce, nahodneCeny(ja, on)); // nastavime nahodne ceny
	vystup(akce, ja);
}

// test parseru
void test(){
	string odpoved = " 2.7 ; 5 ; z 2 3; n 2 3;; n ; novinka";
	vector<string> prikazy = split(odpoved, ";");
	printq(prikazy);
	for(string prikaz : prikazy){
		istringstream iss(prikaz);
		Akce akce;
		iss >> akce;
		cout << iss.fail() << " " << akce << endl;
		// 0 .. platna akce
		// 1 .. neplatna akce
	}
}

int main(int argc, char* argv[]){
	//cout << "n 1 1;n 2 2;" << endl;
	//test();
	id = (argv[argc-1][0] == '1' ? 1 : 2); // ktery hrac je na rade
	ifstream f;
	f.open(playfield.c_str());
	f >> kolo;
	//cerr << kolo;

	// ulozime mapu
	// 10 .. je tam mesto a nikdo tam nema obchod
	// 11 .. je tam mesto a ma tam obchod hrac 1
	// 12 .. je tam mesto a ma tam obchod hrac 2
	// 1 .. neni tam mesto a ma tam obchod hrac 1
	// 2 .. neni tam mesto a ma tam obchod hrac 2

	// ulozime mesta do mapy
	for(Mesto & mesto : kolo.mesta){
		mapa[mesto.pozice] = MESTO; // je tam mesto
	}
	// ulozime obchody do mapy
	for(Hrac & hrac : kolo.hraci){
		for(Obchod & obchod : hrac.obchody){
			if(mapa.find(obchod.pozice) == mapa.end()) // obchod mimo mesto
				mapa[obchod.pozice] = hrac.cislo;
			else
				mapa[obchod.pozice] += hrac.cislo; // obchod ve meste
		}
	}

	strategie();
	return 0;
}
