#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define		MAP_SIZE		14
#define		MAX_LINE_SIZE		256

GtkWidget *frame;
GtkWidget *window;
int number;

/* is on given coordinates ship segment?
 * 1 ... yes
 * 0 ... no
 */
int is_ship(int map[MAP_SIZE][MAP_SIZE], int i, int j)
{
	if (((i >= 0) && (i <= MAP_SIZE-1)) &&
		((j >= 0) && (j <= MAP_SIZE-1)) &&
		((map[i][j] == '1') ||
		(map[i][j] == '2') ||
		(map[i][j] == '*') ||
		(map[i][j] == '+'))) {
		return 1;
	} else {
		return 0;
	}
}

/* read which ship segment have to be shown based on
 * situation of neighbourng fiekds
 * return the segment code - eg. 1000
 */
char *read_ship_segment(int map[MAP_SIZE][MAP_SIZE],
	int i, int j, char label_text[10])
{
	/* left */
	if (is_ship(map, i, j-1) == 0)
		label_text[0] = '0';
	else
		label_text[0] = '1';

	/* right */
	if (is_ship(map, i, j+1) == 0)
		label_text[1] = '0';
	else
		label_text[1] = '1';

	/* left */
	if (is_ship(map, i-1, j) == 0)
		label_text[2] = '0';
	else
		label_text[2] = '1';

	/* right */
	if (is_ship(map, i+1, j) == 0)
		label_text[3] = '0';
	else
		label_text[3] = '1';

}


int bomb(int i, int j, char *name)
{
	GtkWidget *image;
	if ((i < 0) || (i > MAP_SIZE-1) ||
		(j < 0) || (j > MAP_SIZE-1)) {
		return 1;
	}
	image = gtk_image_new_from_file(name);
	gtk_fixed_put(GTK_FIXED(frame), image, 180+j*22, 20+i*22);
	return 0;
}


/* read battlefield.txt.xy and draw the battle state
 * based on it
 */
int show_battlefield(FILE *f)
{
	char label_text[20];
	GtkWidget *image;
	char *file_name;
	int ret = 0;
	int i, j;
	int len;
	char line[MAX_LINE_SIZE];
	char *pos, *pos2, *pos3;
	int map[MAP_SIZE][MAP_SIZE];
	char c, d;
	char *name;
	int first;
	GtkWidget *label;

	/* skip the number of rounds */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}

	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	sscanf(line, "points: %d %d", &i, &j);

	image = gtk_image_new_from_file("pic/nic.jpg");
	gtk_fixed_put(GTK_FIXED(frame), image, 10, 40);
	sprintf(label_text, "body: %d", i);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 10, 40);

	image = gtk_image_new_from_file("pic/nic.jpg");
	gtk_fixed_put(GTK_FIXED(frame), image, 10, 120);
	sprintf(label_text, "body: %d", j);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 10, 120);

	/* skip the number of charges */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	sscanf(line, "charges: %d %d", &i, &j);

	sprintf(label_text, "peníze: %d", i);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 10, 60);

	sprintf(label_text, "peníze: %d", j);
	label = gtk_label_new(label_text);
	gtk_fixed_put(GTK_FIXED(frame), label, 10, 140);


	/* skip the first line of tabular */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}

	/* read the map */
	for (i = 0; i < MAP_SIZE; i++) {
		c = fgetc(f); /* skip the first character on each row */
		for (j = 0; j < MAP_SIZE; j++)
			map[i][j] = fgetc(f);
		fgets(line, MAX_LINE_SIZE, f); /* skip the rest of the row */
	}
	/* draw the map */
	for (i = 0; i < MAP_SIZE; i++) {
		for (j = 0; j < MAP_SIZE; j++) {
			sprintf(label_text, "%c", map[i][j]);
			if ((map[i][j] == ' ') ||
				(map[i][j] == '.')) {
				sprintf(label_text, "pic/sea.jpg");
			}

			if (map[i][j] == '*') {
				sprintf(label_text, "pic/3");
				read_ship_segment(map, i, j, &label_text[5]);
				strcpy(&label_text[9], ".jpg");
			}
			if (map[i][j] == '+') {
				sprintf(label_text, "pic/4");
				read_ship_segment(map, i, j, &label_text[5]);
				strcpy(&label_text[9], ".jpg");
			}
			if (map[i][j] == '1') {
				sprintf(label_text, "pic/1");
				read_ship_segment(map, i, j, &label_text[5]);
				strcpy(&label_text[9], ".jpg");
			}
			if (map[i][j] == '2') {
				sprintf(label_text, "pic/2");
				read_ship_segment(map, i, j, &label_text[5]);
				strcpy(&label_text[9], ".jpg");
			}
			image = gtk_image_new_from_file(label_text);
			gtk_fixed_put(GTK_FIXED(frame), image,
				180+j*22, 20+i*22);
		}
	}
	/* last row of table */
	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}
	/* first player action  draw bomb/torpedo/firework*/
	len = strlen(line);
	pos = strchr(line, ')');
	pos[0] = '\0';
	label = gtk_label_new(&line[9]);
	pos[0] = ')';
	gtk_fixed_put(GTK_FIXED(frame), label, 70, 20);
	pos3 = strstr(line, "last command: ");
	if (pos3 != NULL)
		pos3 = pos3+15;

	if ((len > 30) && (pos3 != NULL)) {
		if  (pos3[0] == 'm') {
			len = sscanf(&pos3[0], "m %d %d", &j, &i);
			bomb(i, j, "pic/1bomb.png");
		}
		if (pos3[0] == 'b') {
			len = sscanf(&pos3[0], "b %d %d", &j, &i);
			bomb(i, j, "pic/1bombd.png");
			bomb(i+1, j,  "pic/1bombc.png");
			bomb(i, j+1, "pic/1bomba.png");
			bomb(i+1, j+1, "pic/1bombb.png");
		}
		if (pos3[0] == 't') {
			len = sscanf(&pos3[0], "t %*d %*d %c", &c);
			pos = strchr(&pos3[0], ':');
			pos++;
			len = sscanf(pos, " %d,%d(%c)", &j, &i, &d);
			while (len == 3) {
				if ((d != '2') && (d != '1')) {
					if ((c == 'l') || (c == 'r'))
						bomb(i, j, "pic/1torpedoa.png");
					else
						bomb(i, j, "pic/1torpedob.png");
				}

				if ((d == '1') || (d == '2'))
					bomb(i, j, "pic/1bomb.png");

				pos2 = strchr(&pos[1], ')');

				if (pos2 != NULL) {
					pos = pos2+2;
					len = sscanf(pos, "%d,%d(%c)",
						&j, &i, &d);
				} else {
					len = 0;
				}
			}
		}
		if (pos3[0] == 'f') {
			pos = strchr(&pos3[0], ':');
			pos++;
			len = sscanf(pos, " %d,%d(%c)", &j, &i, &d);
			first = 1;
			while (len == 3) {
				if (first == 1) {
					first = 0;
					bomb(i, j, "pic/1fire.png");
				} else {
					bomb(i, j, "pic/1bomb.png");
				}
				pos2 = strchr(&pos[1], ')');
				if (pos2 != NULL) {
					pos = pos2+2;
					len = sscanf(pos, "%d,%d(%c)",
						&j, &i, &d);
				} else {
					len = 0;
				}
			}
		}
	}
	if (fgets(line, MAX_LINE_SIZE, f) == NULL) {
		ret = 2;
		goto end;
	}

	len = strlen(line);
	pos = strchr(line, ')');
	pos[0] = '\0';
	label = gtk_label_new(&line[9]);
	pos[0] = ')';

	gtk_fixed_put(GTK_FIXED(frame), label, 70, 100);
	pos3 = strstr(line, "last command: ");
	if (pos3 != NULL)
		pos3 = pos3+15;

	/* second player action  draw bomb/torpedo/firework*/
	if ((len > 30)  && (pos3 != NULL)) {
		if  (pos3[0] == 'm') {
			len = sscanf(&pos3[0], "m %d %d", &j, &i);
			bomb(i, j, "pic/2bomb.png");
		}
		if (pos3[0] == 'b') {
			len = sscanf(&pos3[0], "b %d %d", &j, &i);
			bomb(i, j, "pic/2bombd.png");
			bomb(i+1 , j, "pic/2bombc.png");
			bomb(i, j+1, "pic/2bomba.png");
			bomb(i+1, j+1, "pic/2bombb.png");
		}
		if (pos3[0] == 't') {
			len = sscanf(&pos3[0], "t %*d %*d %c", &c);
			pos = strchr(&pos3[0], ':');
			pos++;
			len = sscanf(pos, " %d,%d(%c)", &j, &i, &d);
			while (len == 3) {
				if ((d != '2') && (d != '1')) {
					if ((c == 'l') || (c == 'r'))
						bomb(i, j, "pic/2torpedoa.png");
					else
						bomb(i, j, "pic/2torpedob.png");
				}

				if ((d == '1') || (d == '2'))
					bomb(i, j, "pic/2bomb.png");

				pos2 = strchr(&pos[1], ')');
				if (pos2 != NULL) {
					pos = pos2+2;
					len = sscanf(pos, "%d,%d(%c)",
						&j, &i, &d);
				} else
					len = 0;

			}
		}
		if (pos3[0] == 'f') {
			pos = strchr(&pos3[0], ':');
			pos++;
			len = sscanf(pos, " %d,%d(%c)", &j, &i, &d);
			first = 1;
			while (len == 3) {
				if (first == 1) {
					first = 0;
					bomb(i, j, "pic/2fire.png");
				} else {
					bomb(i, j, "pic/2bomb.png");
				}

				pos2 = strchr(&pos[1], ')');
				if (pos2 != NULL) {
					pos = pos2+2;
					len = sscanf(pos, "%d,%d(%c)",
						&j, &i, &d);
				} else
					len = 0;

			}
		}
	}
end:
	return ret;
}

gint gtk_show_battlefield(gpointer data)
{
	char file_name[64];
	FILE *f;

	sprintf(file_name, "game/battlefield.txt.%d", number);
	f = fopen(file_name, "r");
	if (f != NULL) {
		show_battlefield(f);
		fclose(f);
		gtk_widget_show_all(frame);
		number++;
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int i, j;
	GtkWidget *label_hrac1, *label_hrac2, *image;
	int counter;
	gint tag;

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "lodě\n");
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
	gtk_widget_show(window);

	frame = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), frame);

	label_hrac1 = gtk_label_new("hrac:");
	gtk_fixed_put(GTK_FIXED(frame), label_hrac1, 30, 20);
	image = gtk_image_new_from_file("pic/1bomb.png");
	gtk_fixed_put(GTK_FIXED(frame), image, 10, 20);
	label_hrac2 = gtk_label_new("hrac:");
	gtk_fixed_put(GTK_FIXED(frame), label_hrac2, 30, 100);
	image = gtk_image_new_from_file("pic/2bomb.png");
	gtk_fixed_put(GTK_FIXED(frame), image, 10, 100);

	tag = g_timeout_add(500, gtk_show_battlefield, &number);

	g_signal_connect_swapped(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);

	gtk_main();

	return ret;
}
