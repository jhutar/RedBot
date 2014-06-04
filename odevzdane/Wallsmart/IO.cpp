#ifndef WALLSMART_IO
#define WALLSMART_IO

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include "Struktura.cpp"
using namespace std;

static inline string &ltrim(string & s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
	return s;
}

static inline string &rtrim(string & s) {
	s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

static inline string &trim(string & s) {
	return ltrim(rtrim(s));
}

vector<string> split(const string & s, const string & delimiter){
	size_t a = 0, b;
	size_t d = delimiter.length();
	size_t l = s.length();
	vector<string> strings;
	do{
		b = s.find(delimiter, a);
		if(b == string::npos)
			b = l;
		string token = s.substr(a, b-a);
		strings.push_back(trim(token));
		a = b+d;
	}while(b < l);
	return strings;
}

ostream & operator<<(ostream & os, const Pozice & pozice){
	os << pozice.x << " " << pozice.y;
	return os;
}
istream & operator>>(istream & is, Pozice & pozice){
	is >> pozice.x >> pozice.y;
	return is;
}

ostream & operator<<(ostream & os, const Akce & akce){
	switch(akce.typ){
		case TypAkce::ZRUS:
			os << "z " << akce.pozice;
			break;
		case TypAkce::NOVY:
			os << "n " << akce.pozice;
			break;
		case TypAkce::CENA:
			os << akce.cena;
			break;
	}
	return os;
}
istream & operator>>(istream & is, Akce & akce){
	char c = is.peek();
	switch(c){
		case 'z':
			akce.typ = TypAkce::ZRUS;
			is >> c;
			is >> akce.pozice;
			break;
		case 'n':
			akce.typ = TypAkce::NOVY;
			is >> c;
			is >> akce.pozice;
			break;
		default:
			akce.typ = TypAkce::CENA;
			is >> akce.cena;
			break;
	}
	return is;
}

ostream & operator<<(ostream & os, const Mesto & mesto){
	os << mesto.pozice << " " << mesto.populace;
	return os;
}
istream & operator>>(istream & is, Mesto & mesto){
	is >> mesto.pozice >> mesto.populace;
	return is;
}

ostream & operator<<(ostream & os, const Obchod & obchod){
	os << obchod.pozice << " " << obchod.cena << " " << obchod.zisk;
	return os;
}
istream & operator>>(istream & is, Obchod & obchod){
	is >> obchod.pozice >> obchod.cena >> obchod.zisk;
	obchod.novaCena = obchod.cena;
	return is;
}

ostream & operator<<(ostream & os, const Hrac & hrac){
	os << "Hrac" << hrac.cislo << ": " << hrac.pocetPenez << " euro " << hrac.pocetObchodu << " obchodu" << endl;
	for(int i=0; i<hrac.pocetObchodu; i++){
		os << hrac.obchody[i] << endl;
	}
	for(size_t i=0; i<hrac.akce.size(); i++){
		os << hrac.akce[i] << "; ";
	}
	os << endl;
	return os;
}
istream & operator>>(istream & is, Hrac & hrac){
	string dummy;
	is >> dummy >> hrac.pocetPenez >> dummy >> hrac.pocetObchodu >> dummy;
	hrac.zisk = 0;
	hrac.obchody.resize(hrac.pocetObchodu);
	for(int i=0; i<hrac.pocetObchodu; i++){
		is >> hrac.obchody[i];
		hrac.obchody[i].cislo = i;
		hrac.zisk += hrac.obchody[i].zisk;
	}
	return is;
}

ostream & operator<<(ostream & os, const Kolo & kolo){
	os << "Kolo: " << kolo.cislo << endl;
	os << "Rozmer: " << kolo.rozmer << endl;
	os << "Mesta: " << kolo.pocetMest << endl;
	for(int i=0; i<kolo.pocetMest; i++){
		os << kolo.mesta[i] << endl;
	}
	for(int i=0; i<kolo.pocetHracu; i++){
		os << kolo.hraci[i];
	}
	return os;
}
istream & operator>>(istream & is, Kolo & kolo){
	string dummy;
	is >> dummy >> kolo.cislo >> dummy >> kolo.rozmer >> dummy >> kolo.pocetMest;
	kolo.mesta.resize(kolo.pocetMest);
	for(int i=0; i<kolo.pocetMest; i++){
		is >> kolo.mesta[i];
	}
	kolo.hraci.resize(kolo.pocetHracu);
	for(int i=0; i<kolo.pocetHracu; i++){
		is >> kolo.hraci[i];
		kolo.hraci[i].cislo = i+1;
	}
	getline(is, dummy);
	for(int i=0; i<kolo.pocetHracu; i++){
		string line;
		getline(is, line);
		size_t a = line.find("\""); // prvni "
		size_t b = line.find("\"", a+1); // druha "
		string odpoved = line.substr(a+1, b-a-1); // to co je mezi ""
		vector<string> prikazy = split(odpoved, ";"); // rozsekame podle ;
		for(size_t j=0; j<prikazy.size(); j++){
			Akce akce;
			istringstream iss(prikazy[j]);
			iss >> akce;
			if(iss.fail())
				break;
			else
				kolo.hraci[i].akce.push_back(akce);
		}
	}
	return is;
}

template <typename K, typename V>
void print(const map<K, V> & o){
	for(typename map<K, V>::const_iterator it = o.begin(); it != o.end(); ++it){
		cout << it->first << " " << it->second << endl;
	}
}

template <typename T>
void print(const vector<T> & o){
	for(typename vector<T>::const_iterator it = o.begin(); it != o.end(); ++it){
		cout << (*it) << endl;
	}
}

template <typename K, typename V>
void printq(const map<K, V> & o){
	for(typename map<K, V>::const_iterator it = o.begin(); it != o.end(); ++it){
		cout << "'" << it->first << "' '" << it->second << "'" << endl;
	}
}

template <typename T>
void printq(const vector<T> & o){
	for(typename vector<T>::const_iterator it = o.begin(); it != o.end(); ++it){
		cout << "'" << (*it) << "'" << endl;
	}
}

#endif
