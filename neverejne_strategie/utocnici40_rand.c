/* strategie hromadi rolniky a skill farmarstvi, neutoci */

#include <stdio.h>
#include <stdlib.h>

const char A_VOJACI = 'v';		// "voják"
const char A_ROLNICI = 'r';		// "obchodník"
const char A_ZBROJENI = 'z';		// "zbraně"
const char A_FARMARENI = 'f';		// "bankovnictví"
const char A_SKLIZEJ = 's';		// "produkuj peníze"
const char A_UTOC = 'u';		// "utok"

int main(int argc, char *argv[])
{
	int vojaci,rolnici;
	int zbrojeni,farmareni;
	int zasoby;
	char ret;
	int pocet_kol;

	if (argc<8) {
		printf("malo parametru - nutno 7: "\
		"kola do konce, území, jídlo, vojáci, rolníci, úroveň zbrojení, úroveň farmaření\n");
		return 1;
	}

	pocet_kol = atoi(argv[2]);
	zasoby = atoi(argv[3]);
	vojaci = atoi(argv[4]);
	rolnici = atoi(argv[5]);
	zbrojeni = atoi(argv[6]);
	farmareni = atoi(argv[7]);
	srand(pocet_kol);

	// mas hlad - najez se
	if ( zasoby < rolnici+vojaci+1 ) {
		ret = A_SKLIZEJ;
		goto end;
	}

	// mas malo roliku/ farmareni, delej s tim neco
	if ( rolnici*farmareni < 9*(rolnici+vojaci) ) {
		if (farmareni >= 6*rolnici)
			ret = A_ROLNICI;
		else
			ret = A_FARMARENI;
		goto end;
	}

	if (vojaci*zbrojeni <40) {
		if (zbrojeni >= vojaci) {
			ret = A_VOJACI;
			goto end;
		} else {
			ret = A_ZBROJENI;
			goto end;
		}
	} else {
		printf("u %d %d\n", rand()%3+1,vojaci);
		return 1;
	}
end:
	printf("%c\n",ret);
	return 0;
}
