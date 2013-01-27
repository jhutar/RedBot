#include <stdio.h>
#include <stdlib.h>

const char A_VOJACI = 'v';		/* "voják" */
const char A_ROLNICI = 'r';		/* "obchodník" */
const char A_ZBROJENI = 'z';		/* "zbraně" */
const char A_FARMARENI = 'f';		/* "bankovnictví" */
const char A_SKLIZEJ = 's';		/* "produkuj peníze" */
const char A_UTOC = 'u';		/* "utok" */

int main(int argc, char *argv[])
{
	int vojaci,rolnici;
	int zbrojeni,farmareni;
	int zasoby;
	int pocet_kol;
	char ret;

	pocet_kol = atoi(argv[1]);
	vojaci = atoi(argv[3]);
	rolnici = atoi(argv[4]);
	zbrojeni = atoi(argv[5]);
	farmareni = atoi(argv[6]);
	zasoby = atoi(argv[7]);

	/* mas hlad - najez se */
	if ( zasoby < rolnici+vojaci+1 ) {
		ret = A_SKLIZEJ;
		goto end;
	}

	/* farmareni minimalne 9 */
	if (farmareni < 9) {
		ret = A_FARMARENI;
		goto end;
	}

	/* vytvor si 5 rolniku */
	if (rolnici < 5) {
		ret = A_ROLNICI;
		goto end;
	}

	/* vytvor si 5 vojaku */
	if (vojaci < 5) {
		ret = A_VOJACI;
		goto end;
	}

	/* zbrojeni minimalne 9 */
	if (zbrojeni < 9) {
		ret = A_ZBROJENI;
		goto end;
	}

	/* kdyz nic nechybi utoc */
	srand(pocet_kol+zasoby);
	printf("l %d\n", rand()%3+1);
	return 0;
end:
	printf("%c\n",ret);
	return 0;
}
