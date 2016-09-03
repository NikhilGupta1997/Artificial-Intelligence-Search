#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <math.h>

#define max 10000
using namespace std;

int maxSteps = 50; //may vary for different inputs
int noOfSteps;

/*Probabilities multiplied by 100*/
int rp; //restart probability: 1/b
int wp = 15; //random walk probability: 0.15
int np; //novelty probability(choosing highest when its age is not larger: 0.5-(age(b2)/age(b1) - 1)/(score(b1)/score(b2))


struct node // bid node structure
{
	int bid_id;
	int cid; //company
	double price;  
	int norc;  // no. of regions in one bid
	int *region;
	double score;
	bool used;
};

struct region {
	int noOfBids;
	vector<int> bid_nos;
	bool used;
};

time_t t,start;
float tim; // time
int nor; // number of regions
int nob; // number of bids
int noc; // number of companies
struct node *tob; // total no. of bids
struct region *reg;
int *age; //age of all bids

bool *com; //keeps record of which companies and regions can be selected in the remaining unprocessed bids
//(bid and reg not needed as bool value is incorporated into struct)


double temp,currentVal=0,maxVal = 0;//max value obtained until now


//function to take input - read from console by redirection
void readFile(char* inputfile)
{
	// open the file to be read from
	FILE *fid;   
	fid = fopen(inputfile,"r");

	char *g;
	fscanf(fid,"%f\n\n",&tim);
	fscanf(fid,"%d\n\n",&nor);
	fscanf(fid,"%d\n\n",&nob);
	fscanf(fid,"%d\n\n",&noc);

	rp = 100/nob;
	if(rp<1)
		rp=1;
	//tim in seconds with a margin of 3 seconds, can change later depending on how often it is being checked (how often random restart is being called)
	tim *= 60;
	tim -= 3;

	tob = new node[nob];
	age = new int[nob];
	com = new bool[noc];
	reg = new region[nor];

	for(int i=0;i<nob;i++)
	{

		tob[i].bid_id = i;

		char ch[max];
		fscanf(fid,"%[^\n] \n", ch);

		int t=0;int j=0;
		char ch1[max];
		while(ch[t]!=' ')
		{
			ch1[j]=ch[t];
			j++;t++;
		}
		
		ch1[j]='\0';
		tob[i].cid=atoi(ch1);
	
		ch1[0]='\0';j=0;t++;
		while(ch[t]!=' ')
		{
			ch1[j]=ch[t];
			j++;t++;
		}
		ch1[j]='\0';			
		tob[i].price=strtod (ch1, NULL);
		t++;
		
		int x=0;
		int w=t;		
		while(ch[t]!='#')
		{
			if(ch[t]==' ')
			{	x++;}
			t++;
		}
		tob[i].norc=x;
		tob[i].score = tob[i].price/tob[i].norc;
		t=w;
		tob[i].region = new int [x];
		for(int qq=0;qq<x;qq++)
		{
			ch1[0]='\0';j=0;
			while(ch[t]!=' ')
			{
				ch1[j]=ch[t];
				j++;t++;
			}
			t++;
			ch1[j]='\0';
			tob[i].region[qq]=atoi(ch1);
		}
		//fscanf(fid,"%s", g);	
	}
	fclose (fid);
}

void fill(int);
bool checkReg(int);
void randomStart() //Whenever there is a random restart
{
	int i;
	int p,b1,b2;
	do {
		time(&t);
		if(t-start>=tim)
			return;

		for(i=0;i<nob;i++) {
			age[i] = maxSteps;
			tob[i].used = false;
		}
		for(i=0;i<nor;i++) {
			reg[i].used = false;
		}
		for(i=0;i<noc;i++) {
			com[i] = false;
		}
		noOfSteps = maxSteps;

		
		do {
			
			// srand(time(NULL));
			p = rand()%100;
			if(p<wp) {
				// srand(time(NULL));
				do{
					b1=rand()%nob;
				}while(tob[b1].used);
				fill(b1);
			} else {
				b1 = 0;
				while(tob[b1].used) {
					b1++;
				}
				b2 = b1+1;
				while(tob[b2].used) {
					b2++;
				}
				if(age[b1]>age[b2]) {
					fill(b1);
				} else {
					// srand(time(NULL));
					p = rand()%100;	
					temp = 0.5 - (age[b2]*1.0/age[b1] - 1)*tob[b2].score/tob[b1].score;
					np = temp*100;
					if(p<np) {
						fill(b1);
					} else {
						fill(b2);
					}
				}
				
			}
			

			// srand(time(NULL));
			p = rand()%100;
		} while(p>=rp);
	} while(true);
}

//helper function of randomStart function
void fill(int bidno)
{
	
	noOfSteps--;
	age[bidno] = noOfSteps;
	com[tob[bidno].cid]=true;
	int i,j;
	int reg_no,b_no;
	for(i=0;i<tob[bidno].norc;i++)
	{
		reg_no = tob[bidno].region[i];
		if(reg[reg_no].used) {
			for(j=0;j<reg[reg_no].noOfBids;j++) {
				b_no = reg[reg_no].bid_nos[j];
				if(tob[b_no].used) {
					tob[b_no].used = false;
					currentVal -= tob[b_no].price;
					break;
				}
			}
		} else {
			reg[reg_no].used = true;
		}
	}
	currentVal += tob[bidno].price;
	tob[bidno].used=true;
	if(currentVal>maxVal)
		maxVal = currentVal;

}	

//helper function of randomStart function
bool checkReg(int bidno)
{
	for(int i=0;i<tob[bidno].norc;i++)
	{
		if(reg[tob[bidno].region[i]].used)
			return true;
	}
	return false;
}

bool compareBids(const node a, const node b) {
	return a.score > b.score;
}

int main(int argc, char* argv[])
{
	time(&start);
	readFile(argv[1]);
	sort(tob,tob+nob,compareBids);
	int i,j;
	for(i=0;i<nor;i++) {
		reg[i].noOfBids = 0;
	}
	for(i=0;i<nob;i++) {
		for(j=0;j<tob[i].norc;j++) {
			reg[tob[i].region[j]].bid_nos.push_back(i);
			reg[tob[i].region[j]].noOfBids++;
		}
	}
	randomStart();
	cout<<maxVal;
	return 0;
}

// int main()
// {
// 	time(&start);
// 	char c[] = "input.txt";
// 	readFile(c);
// 	sort(tob,tob+nob,compareBids);
// 	int i,j;
// 	for(i=0;i<nor;i++) {
// 		reg[i].noOfBids = 0;
// 	}
// 	for(i=0;i<nob;i++) {
// 		for(j=0;j<tob[i].norc;j++) {
// 			reg[tob[i].region[j]].bid_nos.push_back(i);
// 			reg[tob[i].region[j]].noOfBids++;
// 		}
// 	}
// 	randomStart();
// 	cout<<log10(maxVal)<<endl;
// 	for(i=0;i<nob;i++) {
// 		cout<<i<<" "<<tob[i].price<<" "<<tob[i].used<<endl;
// 	}
// 	return 0;
// }

