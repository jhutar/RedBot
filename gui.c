#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <getopt.h>

#define		MAX_LINE_SIZE		256
#define		NAME			"playfield.txt"

GtkWidget *frame;
GtkWidget *window;
int number = 1;
gint tag;
char directory[100];
char pictures[100];
char filename[100];
int maxx, maxy;


GdkPixbuf *get_screenshot(const char *filename)
{
    GdkPixbuf *screenshot;
    GdkWindow *root_window;
    GError *perror = NULL;

    root_window = window->window;
    screenshot = gdk_pixbuf_get_from_drawable (NULL, root_window, 0, 0, 0, 0, 0, 220+32*maxx, 50+32*maxy);
    perror=NULL;
    gdk_pixbuf_save(screenshot, filename, "bmp", &perror,"quality", "100",NULL);
    return screenshot;
}

int chybny_tah(int hrac, int x, int y)
{
	GtkWidget *image;

	sprintf(filename,"%spic/%dlodu.png",pictures, hrac+1);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
}

int let(int hrac, int raketa, int x, int y, char t1,char t2)
{
	GtkWidget *image;

	// druhy krok
	switch (t2) {
		case 'L':{
			sprintf(filename,"%spic/%dlodl.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y-2+4*raketa);
			break;
		}
		case 'P':{
			sprintf(filename,"%spic/%dlodp.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y-2+4*raketa);
			break;
		}
		case 'N': {
			sprintf(filename,"%spic/%dlodu.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x-2+4*raketa, 20+32*y);
			break;
		}
		default: { /* slo se dolu nebo byl zadan jen jeden krok */
			sprintf(filename,"%spic/%dlodd.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x-2+4*raketa, 20+32*y);
			break;
		}
	}

}

int blok(int hrac, int x, int y)
{
	GtkWidget *image;

	sprintf(filename,"%spic/%dlodu.png", pictures, hrac+1);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
	sprintf(filename,"%spic/%dlodblok.png", pictures, hrac+1);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
}

int strela(int hrac, int x, int y, char kam, char delka, char res)
{
	GtkWidget *image;
	int i = 0;
	int d;

	d =((int)delka)-48;
	
	if (res == 'o') {
		d = d-1;
	}

	image = gtk_image_new_from_file("pic/strela.png");

	switch (kam) {
		case 'L':
			sprintf(filename,"%spic/%dlodl.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
			x--;
			while (i<d) {
				sprintf(filename,"%spic/xstrela.png", pictures);
				image = gtk_image_new_from_file(filename);
				gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
				x--;
				i++;
			}
			x++;
			break;
		case 'P':
			sprintf(filename,"%spic/%dlodp.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
			x++;
			while (i<d) {
				sprintf(filename,"%spic/xstrela.png", pictures);
				image = gtk_image_new_from_file(filename);
				gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
				x++;
				i++;
			}
			x--;
			break;
		case 'N':
			sprintf(filename,"%spic/%dlodu.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
			y--;
			while (i<d) {
				sprintf(filename,"%spic/ystrela.png", pictures);
				image = gtk_image_new_from_file(filename);
				gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
				y--;
				i++;
			}
			y++;
			break;
		case 'D':
			sprintf(filename,"%spic/%dlodd.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
			y++;
			while (i<d) {
				sprintf(filename,"%spic/ystrela.png", pictures);
				image = gtk_image_new_from_file(filename);
				gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
				y++;
				i++;
			}
			y--;
			break;
	}
	if (res == 'z') {
		sprintf(filename,"%spic/vybuch.png", pictures);
		image = gtk_image_new_from_file(filename);
		gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
	}
}


int tah(int hrac, int raketa, int x, int y, char kam, char v)
{
	GtkWidget *image;
	int vaha;

	if (v == '1')
		vaha = 1;
	else
		vaha = 2;

	switch (kam) {
		case 'L':{
			sprintf(filename,"%spic/mlha.png", pictures);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*(x+1), 20+32*y);
			sprintf(filename,"%spic/%dlodltahne.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y-2+4*raketa);
			break;
		}
		case 'P':{
			sprintf(filename,"%spic/mlha.png", pictures);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*(x-1), 20+32*y);
			sprintf(filename,"%spic/%dlodptahne.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y-2+4*raketa);
			break;
		}
		case 'N': {
			sprintf(filename,"%spic/mlha.png", pictures);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*(y+1));
			sprintf(filename,"%spic/%dlodutahne.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x-2+4*raketa, 20+32*y);
			break;
		}
		case 'D': {
			sprintf(filename,"%spic/mlha.png", pictures);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*(y-1));
			sprintf(filename,"%spic/%dloddtahne.png", pictures, hrac+1);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x-2+4*raketa, 20+32*y);
			break;
		}
	}

	sprintf(filename,"%spic/asteroid%d.png", pictures, vaha);
	image = gtk_image_new_from_file(filename);

	gtk_fixed_put(GTK_FIXED(frame), image, 180+32*x, 20+32*y);
	
}


/* read input file  and draw the battle state
 * based on it
 */
int show_battlefield(FILE *f)
{
	char line[MAX_LINE_SIZE];

	GtkWidget *image;
	GtkWidget *label;
	char label_text[20];

	int i, j;

	unsigned body;
	char filename[100];
	int z[2][2], r[2][2][2];
	int hrac, raketa;
	char *odpoved1, *odpoved2;
	char t[2][2][20];
	char c;
	int map_size_x = 10;
	int map_size_y = 10;

	/* preskoc pocet kol */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL)
		return 2;

	/* preskoc rozsah pole */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL)
		return 2;

	/* informace o 1. hraci */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL)
		return 2;

	sscanf(line, "Hrac 1: Body:%d Zakladna:[%d,%d] Rakety:[%d,%d]:[%d,%d]",
		&body, &z[0][0], &z[0][1], &r[0][0][0], &r[0][0][1], &r[0][1][0], &r[0][1][1]);

	sprintf(filename,"%spic/body.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 50, 40);
	sprintf(label_text, "body: %d", body);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 50, 40);

	/* informace o 2. hraci */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL)
		return 2;

	sscanf(line, "Hrac 2: Body:%d Zakladna:[%d,%d] Rakety:[%d,%d]:[%d,%d]",
		&body, &z[1][0], &z[1][1],  &r[1][0][0], &r[1][0][1], &r[1][1][0], &r[1][1][1]);

	sprintf(filename,"%spic/body.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 50, 120);
	sprintf(label_text, "body: %d", body);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 50, 120);

	/* uloz tahy 1. hrace */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL)
		return 2;

	odpoved1 = strchr(line, ')');
	if (odpoved1 == NULL)
		return 2;
	odpoved1 = odpoved1 +2;

	odpoved2 = strchr(odpoved1, ':');
	if (odpoved2 == NULL) {
		return 2;
	}

	odpoved2[0] = '\0';
	strcpy(t[0][0], odpoved1);
	odpoved2[0] = ':';
	odpoved2 = odpoved2 +1;
	strcpy(t[0][1], odpoved2);

	/* uloz tahy 2. hrace */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL)
		return 2;

	odpoved1 = strchr(line, ')');
	if (odpoved1 == NULL)
		return 2;
	odpoved1 = odpoved1 +2;

	odpoved2 = strchr(odpoved1, ':');
	if (odpoved2 == NULL) {
		return 2;
	}
	odpoved2[0] = '\0';
	strcpy(t[1][0], odpoved1);
	odpoved2[0] = ':';
	odpoved2 = odpoved2 +1;
	strcpy(t[1][1], odpoved2);
	/* nacti mapu */
	for (i = 0; i < maxy; i++) {
		for (j = 0; j < maxx; j++) {
			c = fgetc(f);
			switch (c) {
				case ' ': {
					sprintf(label_text, "%spic/nic.png", pictures);
					break;
				}
				case '1': {
					sprintf(label_text, "%spic/nic.png", pictures);
					image = gtk_image_new_from_file(label_text);
					gtk_fixed_put(GTK_FIXED(frame), image, 180+j*32, 20+i*32);
					sprintf(label_text, "%spic/asteroid1.png", pictures);
					break;
				}
				case '2': {
					sprintf(label_text, "%spic/nic.png", pictures);
					image = gtk_image_new_from_file(label_text);
					gtk_fixed_put(GTK_FIXED(frame), image, 180+j*32, 20+i*32);
					sprintf(label_text, "%spic/asteroid2.png", pictures);
					break;
				}
			}
			image = gtk_image_new_from_file(label_text);
			gtk_fixed_put(GTK_FIXED(frame), image,
				180+j*32, 20+i*32);
		}
		fgetc(f); /* preskoc znak konce radku */
	}

	// vykresli zakladny
	sprintf(filename,"%spic/1zakladna.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 180+32*z[0][0], 20+32*z[0][1]);
	sprintf(filename,"%spic/2zakladna.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 180+32*z[1][0], 20+32*z[1][1]);

	// vykresli tahy -----------------------------------------------------

	// chybny tah
	for (i = 0; i<4; i++) {
		if ((t[i / 2][i % 2])[0] == 'E') {
			// raketa [i / 2][i % 2] udella chybny tah
			chybny_tah((i/2), (r[i/2][i%2][0]), (r[i/2][i%2][1]));
		}
	}

	// vykresli let
	for (i = 0; i<4; i++) {
		if ((t[i / 2][i % 2])[0] == 'L') {
			// raketa [i / 2][i % 2] leti
			let((i/2), (i%2), r[i/2][i%2][0], (r[i/2][i%2][1]), (t[i/2][i%2])[2], (t[i/2][i%2])[4]);
		}
	}

	// vykresli blokovani
	for (i = 0; i<4; i++) {
		if ((t[i / 2][i % 2])[0] == 'B') {
			// raketa [i / 2][i % 2] blokuje
			blok((i/2), r[i/2][i%2][0], (r[i/2][i%2][1]));
		}
	}

	// vykresli strelbu
	for (i = 0; i<4; i++) {
		if ((t[i / 2][i % 2])[0] == 'S') {
			// raketa [i / 2][i % 2] strili
			strela((i/2), r[i/2][i%2][0], (r[i/2][i%2][1]), (t[i/2][i%2])[2], (t[i/2][i%2])[5], (t[i/2][i%2])[7]);
		}
	}

	// vyresli tahnuti
	for (i = 0; i<4; i++) {
		if ((t[i / 2][i % 2])[0] == 'T') {
			// raketa [i / 2][i % 2] strili
			tah((i/2), i%2, r[i/2][i%2][0], (r[i/2][i%2][1]), (t[i/2][i%2])[2], t[i/2][i%2][5]);
		}
	}

	return 0;
}

gint gtk_show_battlefield()
{
	GtkWidget *image;
	GtkWidget *label;
	char label_text[20];

	char file_name[64];
	FILE *f;

	// nastav jmeno vstupniho souboru
	if (number < 10)
		sprintf(file_name, "%sgame/%s.000%d", directory, NAME, number);
	else
		if (number < 100)
			sprintf(file_name, "%sgame/%s.00%d", directory, NAME, number);
		else
			if (number < 1000)
				sprintf(file_name, "%sgame/%s.0%d", directory, NAME, number);
			else
				sprintf(file_name, "%sgame/%s.%d", directory, NAME, number);

	// otevri vstupni soubor
	f = fopen(file_name, "r");
	if (f == NULL) {
		number++;
		fprintf(stderr, "Soubor %s nelze zobrazit.\n", file_name);
		g_source_remove(tag);
		return;
	}

	sprintf(filename,"%spic/body.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 180, 0);
	sprintf(label_text, "kolo: %d", number);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 180, 0);

	// zobraz data ze vstupniho souboru
	show_battlefield(f);

	fclose(f);

	gtk_widget_show_all(frame);

	// vtvor sceenshot zobrazene situace
	sprintf(file_name, "%sgame/%s%d.bmp", directory, NAME, number);
	get_screenshot(file_name);

	number++;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int i, j, s;
	GtkWidget *label_hrac1, *label_hrac2, *image;
	int counter;
	char c;
	char file_name[MAX_LINE_SIZE];
	char line[MAX_LINE_SIZE];
	FILE *f;

	directory[0]='\0';

	/* nacti parametry */
	while ((c = getopt (argc, argv, "d:p:")) != -1)
		switch (c) {
			case 'd':
				strncpy(directory, optarg, 99);
				directory[99]='\0';
				s = strlen(directory);
				if (s < 99) {
					directory[s]='/';
					directory[s+1]='\0';
				}
				break;
			case 'p':
				strncpy(pictures, optarg, 99);
				pictures[99]='\0';
				s = strlen(pictures);
				if (s < 99) {
					pictures[s]='/';
					pictures[s+1]='\0';
				}
				break;
			default:
				fprintf(stderr,
					"neznamy parametr (mozne parametry: -d <jmeno adresare s zaznamem hry> -p <jmeno adresare s grafikou>)\n");
				abort();
		}
	gtk_init(&argc, &argv);

	/* nacti velikost hraciho planu */
	/* otevri playfield soubor */
	snprintf(file_name, MAX_LINE_SIZE-1,  "%sgame/%s.0000", directory, NAME);
	file_name[MAX_LINE_SIZE-1]='\0';
	f = fopen(file_name, "r");
	if (f == NULL) {
		fprintf(stderr, "Soubor %s nelze zobrazit.\n", file_name);
		return 1;
	}

	/* vynech prvni radek */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL)
		return 1;

	/* nacti hodnoty max_x a max_y*/
	if (fgets(line, MAX_LINE_SIZE, f) == NULL)
		return 1;

	ret = sscanf(line,"%d %d",&maxx,&maxy);
	if (ret != 2)
		return 1;

	fclose(f);

	/* vykresli grafiku okna */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Red Bot - podzim 2013 (asteoidy)\n");
	gtk_window_set_default_size(GTK_WINDOW(window), 220+32*maxx, 50+32*maxy);
	gtk_widget_show(window);

	frame = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), frame);

	label_hrac1 = gtk_label_new("hrac:");
	gtk_fixed_put(GTK_FIXED(frame), label_hrac1, 50, 20);
	sprintf(filename,"%spic/1zakladna.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 10, 20);
	label_hrac2 = gtk_label_new("hrac:");
	gtk_fixed_put(GTK_FIXED(frame), label_hrac2, 50, 100);
	sprintf(filename,"%spic/2zakladna.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 10, 100);

	tag = g_timeout_add(1000, gtk_show_battlefield, &number);

	g_signal_connect_swapped(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}
