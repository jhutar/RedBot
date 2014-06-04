#include <stdio.h>
#include <stdlib.h>

#define RENT 30.0
#define SHOPPRICE 250.0
#define MAXPRICE 10.0

#define SEARCHRANGE 5

//minimum price when income from sell is 0 (5-sqrt(15))
#define MINEFFICIENTPRICE 1.12701665379
//function of income from sell is symmetric
#define MAXEFFICIENTPRICE 5.0

#define DISTANCE(x1,y1,x2,y2) abs(x1-x2)+abs(y1-y2)
#define SHOP_WEIGHT(distance, price) (1.0/((1.0+distance)*(1.0+price*price*price)))
#define POSSIBLE_INCOME(price) (price-1.0-(price*price)/10.0)

FILE *ifp;
  char line[255];
  int value;
	int roundnum,size,cities,shopnum;
  	//cities[x][y][pop]
	int *citieslist[3];
	float **weightsmap;
  float konto[2];
  float **shops[2];
  int shopnums[2];
  float income[2];
  float incomeparcial;
  int player;

  
	float lowestIncome=1000;
	int lowestIncomeX=-1;
	int lowestIncomeY=-1;


//return number of shops of player, inside the boundaries
int numshops(minx,miny,maxx,maxy){
	int i,j;
	int res=0;

	for(i=minx;i<maxx;i++){
		for(j=miny;j<maxy;j++){
			if(shops[player][i][j]>0.00)res++;
		}
	}
	return res;
}


//return the place with the highest weight inside the range, considering only shops inside the range
int* findBestPlace(minx,miny,maxx,maxy){
	int i,j,k;
	float **weights;

	float maxweight;
	int maxwx,maxwy;
	static int result[3];
	
	weights = malloc((maxx-minx) * sizeof(float *));
	for (i = 0; i < maxx-minx; i++){
		weights[i] = malloc((maxy-miny) * sizeof(float));
	}

	if(numshops(minx,miny,maxx,maxy)==0){
		for(i=minx;i<maxx;i++){
			for(j=miny;j<maxy;j++){
				for(k=0;k<cities;k++){
					if(citieslist[0][k]>=minx && citieslist[0][k]<maxx 
					&& citieslist[1][k]>=miny && citieslist[1][k]<maxy){
						weights[i-minx][j-miny]+=citieslist[2][k]*1/(1+DISTANCE(i,j,citieslist[0][k],citieslist[1][k]));				
					}
				}
				if(weights[i-minx][j-miny]>maxweight && shops[0][i][j]==0.0 && shops[1][i][j]==0.0){
					maxweight=weights[i-minx][j-miny];
					maxwx=i;
					maxwy=j;
				}
			}
		}
		result[0]=maxwx;
		result[1]=maxwy;
		result[2]=maxweight;
	}else{
		result[0]=-1;
		result[1]=-1;
		result[2]=-1;
	}
	return result;
	
}


int main(int argc, char *argv[]){

  int i,j,k,l,m;

	if(argc<2){
		fprintf(stderr, "Player number missing!\n");
		return EXIT_FAILURE;
	}

	
  ifp = fopen("playfield.txt", "r");

  player=atoi(argv[1])-1;

  if (ifp == NULL) {
    fprintf(stderr, "Can't open input file!\n");
    return EXIT_FAILURE;
  }
  
  if(fscanf(ifp, "%s %d", line, &roundnum) != EOF) {
    //printf("%s %d\n", line, roundnum);
  }
  
  if(fscanf(ifp, "%s %d", line, &size) != EOF) {
    //printf("%s %d\n", line, size);
  }
  
  if(fscanf(ifp, "%s %d", line, &cities) != EOF) {
    //printf("%s %d\n", line, cities);
  }

	for (i = 0; i < 3; i++){
		citieslist[i] = malloc(cities*sizeof(int));
	}

	weightsmap = malloc(size * sizeof(float *));
	for (i = 0; i < size; i++){
		weightsmap[i] = malloc(size * sizeof(float));
	}


  shops[0]=malloc(size*sizeof(int *));
  shops[1]=malloc(size*sizeof(int *));
  for (i = 0; i < size; i++){
    shops[0][i]=malloc(size * sizeof(int));
    shops[1][i]=malloc(size * sizeof(int));
  }
  
  int x,y;

	//fill the weights
	for(i=0;i<cities;i++){
		fscanf(ifp, "%d %d %d", &citieslist[0][i], &citieslist[1][i], &citieslist[2][i]);
		for (j = 0; j < size; j++) {
			for (k = 0; k < size; k++){
				weightsmap[j][k]+=((float)citieslist[2][i])*SHOP_WEIGHT(DISTANCE(j,k,citieslist[0][i],citieslist[1][i]),MINEFFICIENTPRICE);
			}
		}
	}

	float tmp=0,tmpmax=0;

	//read each player status
  	for(i=0;i<2;i++){
	
		if(fscanf(ifp, "%s %f %s", line, &konto[i], &line[6]) != EOF) {
			//printf("%s %.2f\n", line, konto[i]);
		}

		if(fscanf(ifp, "%d %s", &shopnums[i], line) != EOF) {
			//printf("%s %d\n", line, shopnum);
		}

		for(j=0;j<shopnums[i];j++){
			fscanf(ifp, "%d %d", &x, &y);
			fscanf(ifp, "%f %f", &shops[i][x][y],&incomeparcial);

			
			float prc=MINEFFICIENTPRICE+0.5;
			float lim=(roundnum<250)?RENT-RENT/32.0:(roundnum<500)?RENT-RENT/16.0:(roundnum<750)?RENT-RENT/8.0:RENT-RENT/4.0;

			if(i==player && incomeparcial<lowestIncome && incomeparcial<-lim/2.0){
				lowestIncome=incomeparcial;
				lowestIncomeX=x;
				lowestIncomeY=y;
			}
			
			lim-=(incomeparcial>0)?0:abs(incomeparcial/2.0);

			income[i]+=incomeparcial;
			//if(shops[i][x][y]>MAXPRICE){
				while((POSSIBLE_INCOME(prc)*weightsmap[x][y])<lim && prc<MAXEFFICIENTPRICE){
					prc+=0.05;
					//printf("%.4f %.4f \n",prc,POSSIBLE_INCOME(prc)*weightsmap[x][y]);				
				}
				shops[i][x][y]=prc;
			//}
		}
	}

	//set weights and find the best place for new shop
	int tmpx,tmpy;
	for (j = 0; j < size; j++) {
		for (k = 0; k < size; k++){
			if(weightsmap[j][k]>tmpmax && shops[0][j][k]==0.0 && shops[1][j][k]==0.0 && konto[player]>250.00){
				tmpmax=weightsmap[j][k];
				tmpx=j;
				tmpy=k;
			}
		}
	}

	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++)
			if(shops[player][j][i]>0.0)
				printf("%f;", shops[player][j][i]);
	}

	if(shopnums[player]>1 && income[player]<0 && lowestIncomeX>-1 && lowestIncomeY>-1){
		printf("z %d %d;",lowestIncomeX,lowestIncomeY);
	}

	int newshop[3];
	newshop[0]=-1;
	newshop[1]=-1;
	newshop[2]=-1;	
	int *tmpshop;

	for(i=SEARCHRANGE;i<size;i++){
		for(j=SEARCHRANGE;j<size;j++){
			tmpshop=findBestPlace(i-SEARCHRANGE,j-SEARCHRANGE,i,j);
			if(newshop[2]<tmpshop[2]){
				newshop[0]=tmpshop[0];
				newshop[1]=tmpshop[1];
				newshop[2]=tmpshop[2];
			}	
		}
	}

//	newshop=findBestPlace(0,0,size/2,size/2);
	//printf("n %d %d;",newshop[0],newshop[1]);
   

	//check if player can open shop
	if(konto[player]>SHOPPRICE+RENT*shopnums[player] /*&& (roundnum==0 || income[player]>RENT*shopnums[player])*/)
		if(/*player==1 &&*/ newshop[2]>0){
		//if(player==1 && roundnum==0){
			printf("n %d %d\n",/*1 + RENT/tmpmax,*/newshop[0],newshop[1]);//tmpx,tmpy);
		}
	
}
