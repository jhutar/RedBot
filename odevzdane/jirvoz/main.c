#include <stdio.h>
#include <stdlib.h>

typedef struct pt { int x, y; } point;

int plnmb = 1;
point size;
int *field;

point base, sh1, sh2, enbase,  en1, en2;
point trg1, trg2;
int w1, w2;
char mv1, mv2;
char last1, last2, lasten1, lasten2;
char shooted = 0;

void readto(char ch, FILE *f)
{
    char c;
    do
    {
        c = getc(f);
    }
    while((c != ch) && (c != EOF));
    
    return;
}

int dist(point pt1, point pt2)
{
	return(abs(pt1.x - pt2.x) + abs(pt1.y - pt2.y));
}

void loadme(FILE* f)
{
    readto('[', f);
    fscanf(f, "%d", &base.x); readto(',', f); fscanf(f, "%d", &base.y);
    
    readto('[', f); fscanf(f, "%d", &sh1.x);     //sh1.x
    readto(',', f); fscanf(f, "%d", &sh1.y);     //sh1.y
    readto('[', f); fscanf(f, "%d", &sh2.x);     //sh2.x
    readto(',', f); fscanf(f, "%d", &sh2.y);     //sh2.y
    readto('\n', f);
}

void loadenemy(FILE* f)
{
    readto('[', f);
    fscanf(f, "%d", &enbase.x); readto(',', f); fscanf(f, "%d", &enbase.y);
    
    readto('[', f); fscanf(f, "%d", &en1.x);     //en1.x
    readto(',', f); fscanf(f, "%d", &en1.y);     //en1.y
    readto('[', f); fscanf(f, "%d", &en2.x);     //en2.x
    readto(',', f); fscanf(f, "%d", &en2.y);     //en2.y
    readto('\n', f);
}

void loadtrgs(FILE* f)
{
	typedef struct list_el {int x, y; char move; struct list_el * prev;} item;
	item * curritem, * newitem, * tmpitem;
    
    char c;
    int i, j;
    //zjistit nejblizsi
    for (j = 0; j < size.y; j++)
    {
        for (i = 0; i <= size.x; i++)
        {            
            c = getc(f);
            point p = { .x = i, .y = j };
            
            if (c =='1')
            {
	            field[i + j * size.x] = 1;
            }
            else if (c == '2')
            {
	            field[i + j * size.x] = 2;
            }
            else
            {
	            field[i + j * size.x] = 0;
            }
        }
    }
    
    //novy pathfinding
    int completed;
    int* tmpfield = malloc(size.x * size.y * sizeof(int));
    for (completed = 0; completed < size.x * size.y; completed++)
    	tmpfield[completed] = 0;
    	
    tmpfield[enbase.x + enbase.y * size.x] = 1;
    
    curritem = (item *)malloc(sizeof(item));
    curritem->x = base.x;
    curritem->y = base.y;
    curritem->prev = NULL;    
    
    do
    {
	    newitem = NULL;
	    completed = 1;
	    
	    while(curritem)
	    {
		    CHECK:
		    //je na asteroidu
		    if (field[curritem->x + curritem->y * size.x] == 1)
		    {
			    if (trg1.x < 0)
			    {
				    trg1.x = curritem->x;
				    trg1.y = curritem->y;
				    mv1 = curritem->move;
				    tmpfield[curritem->x + curritem->y * size.x] = 1;
				    w1 = 1;
			    }
			    else if (trg2.x < 0)
			    {
			    	trg2.x = curritem->x;
				    trg2.y = curritem->y;
				    mv2 = curritem->move;
				    w2 = 1;
				    curritem = NULL;
				    break;
			    }
			    
			    curritem = curritem->prev;
			    if (curritem)
			    	goto CHECK;
		    }
		    else if (field[curritem->x + curritem->y * size.x] == 2)
		    {
			    point astw2 = { .x = curritem->x, .y = curritem->y };
			    if (dist(base, astw2) <= dist(base, trg1))
			    {
				    trg1.x = curritem->x;
				    trg1.y = curritem->y;
				    trg2.x = curritem->x;
				    trg2.y = curritem->y;
				    mv1 = mv2 = curritem->move;
				    w1 = w2 = 2;
				    curritem = NULL;
				    break;
			    }
		    }
		    
		    if (!curritem)
		    	break;
		    
		    //nahoru
		    if (curritem->y > 0 && !tmpfield[curritem->x + (curritem->y - 1) * size.x])
		    {
			    tmpitem = (item *)malloc(sizeof(item));
				tmpitem->x = curritem->x;
				tmpitem->y = curritem->y - 1;
				tmpitem->move = 'd';
				tmpitem->prev = newitem;
				newitem = tmpitem;
				tmpfield[curritem->x + (curritem->y - 1) * size.x] = 1;
				completed = 0;
		    }
		    //dolu
		    if (curritem->y < size.y - 1 && !tmpfield[curritem->x + (curritem->y + 1) * size.x])
		    {
			    tmpitem = (item *)malloc(sizeof(item));
				tmpitem->x = curritem->x;
				tmpitem->y = curritem->y + 1;
				tmpitem->move = 'n';
				tmpitem->prev = newitem;
				newitem = tmpitem;
				tmpfield[curritem->x + (curritem->y + 1) * size.x] = 1;
				completed = 0;
		    }
		    //vlevo
		    if (curritem->x > 0 && !tmpfield[(curritem->x - 1) + curritem->y * size.x])
		    {
			    tmpitem = (item *)malloc(sizeof(item));
				tmpitem->x = curritem->x - 1;
				tmpitem->y = curritem->y;
				tmpitem->move = 'p';
				tmpitem->prev = newitem;
				newitem = tmpitem;
				tmpfield[(curritem->x - 1) + curritem->y * size.x] = 1;
				completed = 0;
		    }
		    //vpravo
		    if (curritem->x < size.x - 1 && !tmpfield[(curritem->x + 1) + curritem->y * size.x])
		    {
			    tmpitem = (item *)malloc(sizeof(item));
				tmpitem->x = curritem->x + 1;
				tmpitem->y = curritem->y;
				tmpitem->move = 'l';
				tmpitem->prev = newitem;
				newitem = tmpitem;
				tmpfield[(curritem->x + 1) + curritem->y * size.x] = 1;
				completed = 0;
		    }
		    
		    curritem = curritem->prev;
	    }
	    
	    curritem = newitem;
    }
    while (trg2.x < 0 && curritem);
    
    if (dist(sh1, trg1) > dist(sh1, trg2))
    {
	    point tmppt = trg1;
	    trg1 = trg2;
	    trg2 = tmppt;
	    
	    char tmpmv = mv1;
	    mv1 = mv2;
	    mv2 = tmpmv;
    }
}

void loadlast(FILE* f)
{
	readto(':', f);
  	if (getc(f) == '(')
	{
	    last1 = getc(f);
	    readto(':', f);
	    last2 = getc(f);
  	}
  	
	readto('\n', f);
}

void loadlasten(FILE* f)
{
	readto(':', f);
  	if (getc(f) == '(')
	{
	    lasten1 = getc(f);
	    readto(':', f);
	    lasten2 = getc(f);
  	}
  	
	readto('\n', f);
}

void load()
{    
    //otevreni souboru
    FILE *file;
    file = fopen("playfield.txt", "r");
    if (!file)
        exit(EXIT_FAILURE);
    
    //cteni souboru
    readto('\n', file); //preskocit kola
    fscanf(file, "%d", &size.x); fscanf(file, "%d", &size.y);  //velikost pole
    field = malloc(size.x * size.y * sizeof(int));
    
    trg1.x = -size.x; trg1.y = -size.y;
    trg2.x = -size.x; trg2.y = -size.y;
    w1 = 0; w2 = 0;
    
    //nacist lode a zakladny
    if (plnmb == 1)
    {
        loadme(file);
        loadenemy(file);
    }
    else
    {
        loadenemy(file);
        loadme(file);
    }
    
    if (plnmb == 1)
    {
	    loadlast(file);
	    loadlasten(file);
    }
    else
    {
	    loadlasten(file);
	    loadlast(file);
    }
    
    //cist hraci plan
	loadtrgs(file);
    
    fclose(file);
}

int tryshoot(point ship, point ship2)
{
    //sestreleni
    if (ship.x == en1.x || ship.x == en2.x)
    {
        if (((ship.y < en1.y && field[en1.x + en1.y * size.x]) || (ship.y < en2.y && field[en2.x + en2.y * size.x]))
                && (ship.y >= ship2.y || ship.x != ship2.x))
        {
            printf("s d");
            shooted = 1;
            return 1;
        }
        else if (((ship.y > en1.y && field[en1.x + en1.y * size.x]) || (ship.y > en2.y && field[en2.x + en2.y * size.x]))
                && (ship.y <= ship2.y || ship.x != ship2.x))
        {
            printf("s n");
            shooted = 1;
            return 1;
        }
    }
    else if (ship.y == en1.y || ship.y == en2.y)
    {
        if (((ship.x < en1.x && field[en1.x + en1.y * size.x]) || (ship.x < en2.x && field[en2.x + en2.y * size.x]))
                && (ship.x >= ship2.x || ship.y != ship2.y))
        {
            printf("s p");
            shooted = 1;
            return 1;
        }
        else if (((ship.x > en1.x && field[en1.x + en1.y * size.x]) || (ship.x > en2.x && field[en2.x + en2.y * size.x]))
                && (ship.x <= ship2.x || ship.y != ship2.y))
        {
            printf("s l");
            shooted = 1;
            return 1;
        }
    }
    
    return 0;
}

void movesh1()
{
    if (last1 != 's')
        if (tryshoot(sh1, sh2))
            return;
    
    if (trg1.x < 0 || trg1.y < 0
    	|| (w1 == 2 && (sh1.x == en1.x && sh1.y == en1.y && lasten1 == 't')
    	|| (sh1.x == en2.x && sh1.y == en2.y && lasten2 == 't')))
    {
	    printf("b");
	    return;
    }
    
    //není na asteroidu
    if (sh1.x != trg1.x || sh1.y != trg1.y)
    {
        printf("l");
        
    	int i;
        for (i = 0; i < 2; i++)
        {
            if (sh1.x < trg1.x)
            {
                printf(" p");
                sh1.x++;
            }
            else if (sh1.x > trg1.x)
            {
                printf(" l");
                sh1.x--;
            }
            else if (sh1.y < trg1.y)
            {
                printf(" d");
                sh1.y++;
            }
            else if (sh1.y > trg1.y)
            {
                printf(" n");
                sh1.y--;
            }
        }
    }
    else        //tahnout
    {
	    switch (mv1)
	    {
		    case 'n':
		    	sh1.y--;
		    	break;
		    case 'd':
		    	sh1.y++;
		    	break;
		    case 'l':
		    	sh1.x--;
		    	break;
		    case 'p':
		    	sh1.x++;
		    	break;
	    }
	    
	    printf("t %c", mv1);
	    return;
	    
        if (sh1.x < base.x && (sh1.x + 1 != enbase.x || sh1.y != enbase.y)
        	&& field[(sh1.x + 1) + sh1.y * size.x] == 0)
        {
            printf("t p");
            sh1.x++;
        }
        else if (sh1.x > base.x && (sh1.x - 1 != enbase.x || sh1.y != enbase.y)
        	&& field[(sh1.x - 1) + sh1.y * size.x] == 0)
        {
            printf("t l");
            sh1.x--;
        }
        else if (sh1.y < base.y && (sh1.x != enbase.x || sh1.y + 1 != enbase.y)
        	&& field[sh1.x + (sh1.y + 1) * size.x] == 0)
        {
            printf("t d");
            sh1.y++;
        }
        else if (sh1.y > base.y  && (sh1.x != enbase.x || sh1.y - 1 != enbase.y)
        	&& field[sh1.x + (sh1.y - 1) * size.x] == 0)
        {
            printf("t n");
            sh1.y--;
        }
        else
	        printf("b");
    }
    
}

void movesh2()
{
    if (last1 != 's' && last2 != 's')
        if (tryshoot(sh2, sh1))
            return;
    
    if ((shooted && w2 == 2) || trg2.x < 0 || trg2.y < 0
    	|| (w2 == 2 && (sh2.x == en1.x && sh2.y == en1.y && lasten1 == 't')
    	|| (sh2.x == en2.x && sh2.y == en2.y && lasten2 == 't')))
    {
	    printf("b");
	    return;
    }
    
    //není na asteroidu
    if (sh2.x != trg2.x || sh2.y != trg2.y)
    {
        printf("l");
        
	    int i;
        for (i = 0; i < 2; i++)
        {
            if (sh2.x < trg2.x)
            {
                printf(" p");
                sh2.x++;
            }
            else if (sh2.x > trg2.x)
            {
                printf(" l");
                sh2.x--;
            }
            else if (sh2.y < trg2.y)
            {
                printf(" d");
                sh2.y++;
            }
            else if (sh2.y > trg2.y)
            {
                printf(" n");
                sh2.y--;
            }
        }
    }
    else	//tahnout
    {
	    switch (mv2)
	    {
		    case 'n':
		    	sh2.y--;
		    	break;
		    case 'd':
		    	sh2.y++;
		    	break;
		    case 'l':
		    	sh2.x--;
		    	break;
		    case 'p':
		    	sh2.x++;
		    	break;
	    }
	    if (w2 == 2 || dist(sh1, sh2) > 0)
	    	printf("t %c", mv2);
	    else
	    	printf("b");
	    
	    return;
    }
        
}

int main(int argc, char** argv)
{
    if (argc > 1)
        plnmb = atoi(argv[1]);
    
    load();
    
    movesh1();
    printf(":");
    movesh2();
    
    return (EXIT_SUCCESS);
}