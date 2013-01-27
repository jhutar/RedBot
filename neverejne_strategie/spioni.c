/* strategie hromadi rolniky a skill farmarstvi, neutoci */

#include <stdio.h>
#include <stdlib.h>

const char A_VOJACI = 'v';		// "voják"
const char A_ROLNICI = 'r';		// "obchodník"
const char A_ZBROJENI = 'z';		// "zbraně"
const char A_FARMARENI = 'f';		// "bankovnictví"
const char A_SKLIZEJ = 's';		// "produkuj peníze"
const char A_UTOC = 'u';		// "utok"
const char A_SPIONAZ = 't';		// spionazni skill
const char A_PATREJ = 'i';

#define TEST_FILE "test.txt"

void create_file()
{
	FILE *f;
	f = fopen(TEST_FILE, "w");
	fprintf(f, "1");
	fclose(f);
}

int file_exists()
{
	FILE *f;
	char c;
	f = fopen(TEST_FILE, "r");
	if (f == NULL)
		return 0;
	c = fgetc(f);
	fclose(f);
	if (c == '1')
		return 1;
	else
		return 0;
}

void remove_file()
{
	FILE *f;
	f = fopen(TEST_FILE, "w");
	fprintf(f, "0");
	fclose(f);
}

int main(int argc, char *argv[])
{
	int vojaci,rolnici;
	int zbrojeni,farmareni;
	int zasoby, spionaz;
	char ret;

	if (argc<8) {
		printf("malo parametru - nutno 7: "\
		"kola do konce, území, jídlo, vojáci, rolníci, úroveň zbrojení, úroveň farmaření\n");
		return 1;
	}

	zasoby = atoi(argv[3]);
	vojaci = atoi(argv[4]);
	rolnici = atoi(argv[5]);
	zbrojeni = atoi(argv[6]);
	farmareni = atoi(argv[7]);
	spionaz = atoi(argv[8]);
//fprintf(stderr, "zasoby %d vojaci %d rolnici %d zbrojeni %d farmareni %d\n", zasoby, vojaci, rolnici, zbrojeni, farmareni);

	// mas hlad - najez se
	if ( zasoby < rolnici+vojaci+1 ) {
		ret = A_SKLIZEJ;
		remove_file();
		goto end;
	}

	if (file_exists() == 1) {
		ret = A_PATREJ;
		remove_file();
	}

	// mas malo roliku/ farmareni, delej s tim neco
	if (rolnici*farmareni < 9*(rolnici+vojaci)) {
		if (farmareni >= 4*rolnici)
			ret = A_ROLNICI;
		else
			ret = A_FARMARENI;
		goto end;
	}

	if (vojaci*zbrojeni < 30) {
		if (zbrojeni*2 >= vojaci) {
			ret = A_VOJACI;
			goto end;
		} else {
			ret = A_ZBROJENI;
			goto end;
		}
	} else {
//		if (spionaz < 3)
//			ret = A_SPIONAZ; 
//		else {
			ret = A_PATREJ;
			create_file();
//		}
	}
end:
	printf("%c\n",ret);
	return 0;
}
