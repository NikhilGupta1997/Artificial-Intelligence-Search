#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h> 
#include <queue>

#define max 10000
using namespace std;

struct node{ // bid node structure
	int bid_id;
	int cid; //company
	double price;  
	int norc;  // no. of regions in one bid
	int *region;
	double score;
	bool used;
};

struct region{
	int noOfBids;
	vector<int> bid_nos;
	bool used;
};

struct CompareReg : public std::binary_function<region, region, bool>{
   bool operator()(const region lhs, const region rhs) const
   {
      return lhs.noOfBids < rhs.noOfBids;
   }
};

int maxSteps = 50; //may vary for different inputs
int noOfSteps;

/*Probabilities multiplied by 100*/
int rp; //restart probability: 1/b
int wp = 15; //random walk probability: 0.15
int np; //novelty probability(choosing highest when its age is not larger: 0.5-(age(b2)/age(b1) - 1)/(score(b1)/score(b2))

time_t t,start,check;
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

// Used to generate the Start State
priority_queue<region,vector<region>, CompareReg > region_size;

// Debugging perposes to get Bid info
void return_bid(int bidno){
	cout<<"BID "<<bidno<< " ";
	cout<<tob[bidno].bid_id<<" "<<tob[bidno].cid<<" "<<endl;
	cout<<"Regions ";
	for(int i=0; i<tob[bidno].norc; i++){
		cout<<tob[bidno].region[i]<<" ";
	}
	cout<<endl;
}

// Debugging perposes to remember the best set of bids
std::vector<int> vec;
void remember(){
	vec.clear();
	for(int i=0; i<nob; i++){
		if(tob[i].used){
			vec.push_back(i);
		}
	}
}

//function to take input - read from console by redirection
void readFile(char* inputfile){
	// open the file to be read from
	FILE *fid;   
	fid = fopen(inputfile,"r");

	fscanf(fid,"%f\n\n",&tim);
	fscanf(fid,"%d\n\n",&nor);
	fscanf(fid,"%d\n\n",&nob);
	fscanf(fid,"%d\n\n",&noc);

	rp = 1/nob;
	if(rp<1)
		rp=0;
	//tim in seconds with a margin of 3 seconds, can change later depending on how often it is being checked (how often random restart is being called)
	tim *= 60;
	tim -= 3;

	tob = new node[nob];
	age = new int[nob];
	com = new bool[noc];
	reg = new region[nor];

	for(int i=0;i<nob;i++){
		tob[i].bid_id = i;

		char ch[max];
		fscanf(fid,"%[^\n] \n", ch);

		int t=0;int j=0;
		char ch1[max];
		while(ch[t]!=' '){
			ch1[j]=ch[t];
			j++;t++;
		}
		
		ch1[j]='\0';
		tob[i].cid=atoi(ch1);
	
		ch1[0]='\0';j=0;t++;
		while(ch[t]!=' '){
			ch1[j]=ch[t];
			j++;t++;
		}
		ch1[j]='\0';			
		tob[i].price=strtod (ch1, NULL);
		t++;
		
		int x=0;
		int w=t;		
		while(ch[t]!='#'){
			if(ch[t]==' ')
			{	x++;}
			t++;
		}
		tob[i].norc=x;
		tob[i].score = tob[i].price/tob[i].norc;
		t=w;
		tob[i].region = new int [x];
		for(int qq=0;qq<x;qq++){
			ch1[0]='\0';j=0;
			while(ch[t]!=' '){
				ch1[j]=ch[t];
				j++;t++;
			}
			t++;
			ch1[j]='\0';
			tob[i].region[qq]=atoi(ch1);
		}
		tob[i].used = false;
	}
	fclose (fid);
}

void getStartState();
void getStartState1();
void fill(int);
bool checkReg(int);
void randomStart() //Whenever there is a random restart
{
	int i;
	int p,b1,b2;
	do {
		cout<<"Random Restart"<<endl;
		time(&t);
		if(t-start>=tim)
			return;

		for(i=0;i<nob;i++) {
			age[i] = maxSteps;
			tob[i].used = false;
		}
		for(i=0;i<nor;i++) {
			reg[i].used = false;
			region_size.push(reg[i]);
		}
		for(i=0;i<noc;i++) {
			com[i] = false;
		}
		noOfSteps = maxSteps;
		currentVal = 0;
		getStartState();

		// To see value of random start state
		double total_price = 0.0;
		for(int i=0; i<nob; i++){
			if(tob[i].used==true){
				total_price += tob[i].price;
			}
		}
		cout<<total_price<<endl;
		
		// Hill Climbing
		do{
			time(&check);
			if(check-start>=tim)
			return;
			srand(time(0));
			p = rand()%100;
			if(p<wp){
				srand(time(0));
				do{
					b1=rand()%nob;
				}while(tob[b1].used);
				fill(b1);
			} 
			else {
				b1 = 0;
				while(tob[b1].used){
					b1++;
				}
				b2 = b1+1;
				while(tob[b2].used){
					b2++;
				}
				if(age[b1]>age[b2]){
					fill(b1);
				} 
				else{
					srand(time(0));
					p = rand()%100;	
					temp = 0.5 - (age[b2]*1.0/age[b1] - 1)*tob[b2].score/tob[b1].score;
					np = temp*100;
					if(p<np){
						fill(b1);
					} 
					else{
						fill(b2);
					}
				}
			}
			srand(time(0));
			p = rand()%100;
		} while(check-t<30);
	} while(true);
}

// Helper function to get new start state
void getStartState(){
	int rand1, rand2, rand3, rand_no, num1, num2, num3;
	while ( !region_size.empty() ){
		region temp = region_size.top();
		region_size.pop();
		if(!temp.used){
			srand(std::time(0)); 
			rand1 = rand()%temp.bid_nos.size();
			num1 = temp.bid_nos[rand1];
			srand(std::time(0)); 
			rand2 = rand()%temp.bid_nos.size();
			num2 = temp.bid_nos[rand2];
			srand(std::time(0)); 
			rand3 = rand()%temp.bid_nos.size();
			num3 = temp.bid_nos[rand3];
			srand(std::time(0)); 
			rand_no = rand()%3;
			if(rand_no == 0){
				if(!checkReg(num1) && !com[temp.bid_nos[rand1]])
					fill(num1);
			}
			else if(rand_no == 1){
				if(!checkReg(num2) && !com[temp.bid_nos[rand2]])
					fill(num2);
			}
			else if(rand_no == 2){
				if(!checkReg(num3) && !com[temp.bid_nos[rand3]])
					fill(num3);
			}
		}
	}
}

// Helper function to get new start state
void getStartState1(){
	int num1,i;
	srand(time(0));
	num1=rand()%nob;
	fill(num1);
	for(i=(num1+1)%nob;i!=num1;i=(i+1)%nob){
		if(com[tob[i].cid] || checkReg(i))
			continue;
		fill(i);
	}
}

//helper function of randomStart function
void fill(int bidno){	
	noOfSteps--;
	age[bidno] = noOfSteps;
	com[tob[bidno].cid]=true;
	int i,j;
	int reg_no,b_no;
	for(i=0;i<tob[bidno].norc;i++){
		reg_no = tob[bidno].region[i];
		if(reg[reg_no].used){
			for(j=0;j<reg[reg_no].noOfBids;j++){
				b_no = reg[reg_no].bid_nos[j];
				if(tob[b_no].used){
					tob[b_no].used = false;
					com[tob[b_no].cid] = false;
					for(int k=0; k<tob[b_no].norc; k++){
						reg[tob[b_no].region[k]].used = false;
					}
					reg[reg_no].used = true;
					currentVal -= tob[b_no].price;
					break;
				}
			}
		} 
		else {
			reg[reg_no].used = true;
		}
	}
	currentVal += tob[bidno].price;
	tob[bidno].used=true;
	if(currentVal>maxVal){
		maxVal = currentVal;
		remember();
	}
}

//helper function of randomStart function
bool checkReg(int bidno){
	for(int i=0;i<tob[bidno].norc;i++){
		if(reg[tob[bidno].region[i]].used)
			return true;
	}
	return false;
}

bool compareBids(const node a, const node b){
	return a.score > b.score;
}

int main(int argc, char* argv[]){
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
			reg[tob[i].region[j]].used = false;
		}
	}
	randomStart();
	cout<<maxVal<<endl;
	for(i=0; i<vec.size(); i++){
		cout<<vec[i]<<" ";
	}
	cout<<endl;
	return 0;
}
