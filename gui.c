#include <stdio.h>
#include <gtk-3.0/gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define		MAX_LINE_SIZE		256
#define		NAME			"playfield.txt"

GtkWidget *frame;
GtkWidget *window;
char directory[100];
char pictures[100];
int number = 1;
guint tag;

/*
GdkPixbuf *get_screenshot(const char *filename)
{
    GdkPixbuf *screenshot;
    GError *perror = NULL;

    screenshot = gdk_pixbuf_get_from_drawable (NULL, window, 0, 0, 0, 0, 0, 600-10, 400-10);
    perror=NULL;
    gdk_pixbuf_save(screenshot, filename, "bmp", &perror,"quality", "100",NULL);
    return screenshot;
}
*/


/* read battlefield.txt.xy and draw the battle state
 * based on it
 */
int show_battlefield(FILE *f)
{
	char label_text[20];
	GtkWidget *image;
	int ret = 0;
	int round, map_size, mest;
	char line[MAX_LINE_SIZE];
	char filename[MAX_LINE_SIZE];
	int i, j;
	int x, y, size;
	GtkWidget *label;
	int money, shops;
	float c1, c2;

	/* skip the number of rounds */

	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	sscanf(line, "Kolo: %d", &round);

	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	sscanf(line, "Rozmer: %d", &map_size);

	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	sscanf(line, "Mesta: %d", &mest);

	sprintf(filename,"%spic/nic.jpg", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 30, 20);
	sprintf(label_text, "kolo: %d", round);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 30, 20);

	/* vykresli prazdnou mapu */
	sprintf(filename,"%spic/sea.jpg", pictures);
	image = gtk_image_new_from_file(filename);
	for (i = 0; i < map_size; i++) {
		for (j = 0; j < map_size; j++) {
			sprintf(filename,"%spic/sea.jpg", pictures);
			image = gtk_image_new_from_file(filename);
			gtk_fixed_put(GTK_FIXED(frame), image, 180+j*42, 20+i*42);
		}
	}

	/* nacti jednotliva mesta */
	for (i = 0; i<mest; i++) {
		if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
			ret = 2;
			goto end;
		}
		sscanf(line, "%d %d %d", &x, &y, &size);
		sprintf(label_text, "%d", size);
		label = gtk_label_new(label_text);
		gtk_fixed_put(GTK_FIXED(frame), label, 180+x*42+3, 20+y*42-2);
	}

	/* nacti prvniho hrace */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	sscanf(line, "Hrac1: %d euro %d", &money, &shops);
	sprintf(filename,"%spic/nic.jpg", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 30, 40);
	sprintf(label_text, "hrac: %d", money);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 30, 40);
	for (i = 0; i<shops; i++) {
		if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
			ret = 2;
			goto end;
		}
		sscanf(line, "%d %d %f %f", &x, &y, &c1, &c2);
		if (c2 < 0) {
			sprintf(filename,"%spic/1shop-.png", pictures);
			image = gtk_image_new_from_file(filename);
		} else {
			sprintf(filename,"%spic/1shop+.png", pictures);
			image = gtk_image_new_from_file(filename);
		}
		gtk_fixed_put(GTK_FIXED(frame), image, 180+x*42, 20+y*42);
		sprintf(label_text, "%.1f", c1);
		label = gtk_label_new(label_text);
		gtk_fixed_put(GTK_FIXED(frame), label, 180+x*42+3, 20+y*42+9);
		sprintf(label_text, "%2.0f", c2);
		label = gtk_label_new(label_text);
		gtk_fixed_put(GTK_FIXED(frame), label, 180+x*42+3, 20+y*42+20);
	}

	/* nacti druheho hrace */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	sscanf(line, "Hrac2: %d euro %d", &money, &shops);
	sprintf(filename,"%spic/nic.jpg", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 30, 60);
	sprintf(label_text, "hrac: %d", money);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 30, 60);
	for (i = 0; i<shops; i++) {
		if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
			ret = 2;
			goto end;
		}
		sscanf(line, "%d %d %f %f", &x, &y, &c1, &c2);
		if (c2 < 0) {
			sprintf(filename,"%spic/2shop-.png", pictures);
			image = gtk_image_new_from_file(filename);
		} else {
			sprintf(filename,"%spic/2shop+.png", pictures);
			image = gtk_image_new_from_file(filename);
		}
		gtk_fixed_put(GTK_FIXED(frame), image, 180+x*42, 20+y*42);
		sprintf(label_text, "%.1f", c1);
		label = gtk_label_new(label_text);
		gtk_fixed_put(GTK_FIXED(frame), label, 180+x*42+3, 20+y*42+9);
		sprintf(label_text, "%2.0f", c2);
		label = gtk_label_new(label_text);
		gtk_fixed_put(GTK_FIXED(frame), label, 180+x*42+3, 20+y*42+20);
	}
end:
	return ret;
}

gint gtk_show_battlefield()
{
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

	f = fopen(file_name, "r");
	if (f == NULL) {
		number++;
		fprintf(stderr, "Soubor %s nelze zobrazit.\n", file_name);
		g_source_remove(tag);
		return 1;
	}

	show_battlefield(f);
	fclose(f);

	gtk_widget_show_all(frame);

//	get_screenshot(file_name);
	number++;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	GtkWidget *label, *image;
	char c;
	int s;
	char filename[MAX_LINE_SIZE];

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
					"neznamy parametr (mozne parametry: -d <jmeno adresare s zaznamem hry> -p <jmeno adresare s grafikou");
				abort();
		}


	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Trziste");
	gtk_window_set_default_size(GTK_WINDOW(window), 820, 680);
	gtk_widget_show(window);

	frame = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), frame);

	label = gtk_label_new("kolo:");
	gtk_fixed_put(GTK_FIXED(frame), label, 30, 20);
	label = gtk_label_new("hrac:");
	gtk_fixed_put(GTK_FIXED(frame), label, 30, 40);
	sprintf(filename,"%spic/1bullet.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 10, 40);
	label = gtk_label_new("hrac:");
	gtk_fixed_put(GTK_FIXED(frame), label, 30, 60);
	sprintf(filename,"%spic/2bullet.png", pictures);
	image = gtk_image_new_from_file(filename);
	gtk_fixed_put(GTK_FIXED(frame), image, 10, 60);

	tag = g_timeout_add(1000, gtk_show_battlefield, &number);

	g_signal_connect_swapped(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);

	gtk_main();

	return ret;
}
