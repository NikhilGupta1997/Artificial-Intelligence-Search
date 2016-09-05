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
#include <fstream>

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
	vector<int> conflicting_bids;
};

struct region{
	int noOfBids;
	vector<int> bid_nos;
	bool used;
};

struct company{
	int noOfBids;
	vector<int> bid_nos;
	bool used;
};

struct CompareScore : public std::binary_function<node, node, bool>{
   bool operator()(const node lhs, const node rhs) const
   {
      return lhs.score < rhs.score;
   }
};

int maxSteps = 500; //may vary for different inputs
int noOfSteps;

/*Probabilities multiplied by 100*/
int rp; //restart probability: 1/b
int wp = 10; //random walk probability: 10%
int np; //novelty probability(choosing highest when its age is not larger: 0.5-(age(b2)/age(b1) - 1)/(score(b1)/score(b2))

time_t t,start,check;
float tim; // time
int nor; // number of regions
int nob; // number of bids
int noc; // number of companies
struct node *tob, *stob; // total no. of bids // sorted list of bids
struct region *reg;
struct company *comp;
int *age; //age of all bids

double temp,currentVal=0,maxVal = 0;//max value obtained until now

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

	rp = 1000/nob;
	if(rp<1)
		rp=1;
	//tim in seconds with a margin of 1 second, can change later depending on how often it is being checked (how often random restart is being called)
	tim *= 60;
	tim -= 1;

	tob = new node[nob];
	stob = new node[nob];
	age = new int[nob];
	comp = new company[noc];
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
		stob[i] = tob[i];
	}
	fclose (fid);
}

void getStartState();
void fill(int);
bool checkReg(int);

double loss(int bidno) {
	double loss_amount = 0.0;
	int size = tob[bidno].conflicting_bids.size();
	int i,b_temp;
	for(i=0;i<size;i++) {
		b_temp = tob[bidno].conflicting_bids[i];
		if(tob[b_temp].used)
			loss_amount += tob[b_temp].price;
	}
	return loss_amount;
}

void remove(int comid){
	int index, i;
	for(i=0; i<nob; i++){
		if(tob[i].cid == comid){
			if(tob[i].used){
				index = i;
				break;
			}
		}
	}
	if (i<nob){
		currentVal-=tob[index].price;
		tob[index].used=false;
		int reg_no;
		comp[tob[index].cid].used = false;
		for(int i=0; i<tob[index].norc; i++){
			reg_no = tob[index].region[i];
			reg[reg_no].used=false;
		}
	}
}

void randomStart() //Whenever there is a random restart
{
	int i;
	int p,b1,b2, b1_map, b2_map;
	bool srand_call = false;
	do {
		start:
		time(&t);
		if(t-start>=tim)
			return;
		if(((t - start) % 20 == 0)&&!srand_call) {
			srand_call = true;
			srand(time(0));
		} else {
			srand_call = false;
		}

		for(i=0;i<nob;i++) {
			age[i] = maxSteps;
			tob[i].used = false;
		}
		for(i=0;i<nor;i++) {
			reg[i].used = false;
		}
		for(i=0;i<noc;i++) {
			comp[i].used = false;
		}
		noOfSteps = maxSteps;
		currentVal = 0;
		getStartState();
		
		// Hill Climbing
		do{
			time(&check);
			if(check-start>=tim)
				return;

			p = rand()%100;
			if(p<wp){
				do{
					b1=rand()%nob;
				}while(tob[b1].used);
				remove(tob[b1].cid);
				fill(b1);
			} 
			else {
				b1 = 0;
				b1_map = stob[b1].bid_id;
				while(tob[b1_map].used || loss(b1_map)>tob[b1_map].price || comp[tob[b1_map].cid].used){
					b1++;
					if(b1>=nob){
						goto start;
					}
					b1_map = stob[b1].bid_id;
				}
				b2 = b1+1;
				if(b2 >= nob){
					goto only_one;
				}
				b2_map = stob[b2].bid_id;
				while(tob[b2_map].used || loss(b2_map)>tob[b2_map].price || comp[tob[b2_map].cid].used){
					b2++;
					if(b2>=nob){
						goto only_one;
					}
					b2_map = stob[b2].bid_id;
				}
				if(age[b1]>age[b2]){
					fill(b1_map);
				} 
				else{
					p = rand()%100;	
					temp = 0.5 - (age[b2]*1.0/age[b1] - 1)*tob[b2_map].score/tob[b1_map].score;
					np = temp*100;
					if(p<np){
						only_one:
						fill(b1_map);
					} 
					else{
						fill(b2_map);
					}
				}
			}
			p = rand()%1000;
		} while(p>=rp);
	} while(true);
}

bool not_zero(bool arr[]){
	for(int i=0; i<noc; i++){
		if (arr[i])
			return true;
	}
	return false;
}

void getStartState(){
	priority_queue<node,vector<node>, CompareScore > company_scores[max];
	for(int j=0; j<nob; j++){
		int co = tob[j].cid;
		company_scores[co].push(tob[j]);
	}
	int num = rand()%noc;
	bool count[noc];
	for(int i=0; i<noc; i++){
		count[i] = true;
	}
	while(not_zero(count)){
		count[num]=false;
		while(!company_scores[num].empty()){
			node temp = company_scores[num].top();
			company_scores[num].pop();
			int id = temp.bid_id;
			int p = rand()%1000;
			if(p>800)
				continue;
			if(!checkReg(id)){
				fill(id);
				break;
			}
		}
		num = (num+1)%noc;
	}	
}

void fill(int bidno){	
	noOfSteps--;
	age[bidno] = noOfSteps;
	comp[tob[bidno].cid].used=true;
	int i,j;
	int reg_no,b_no;
	int size = tob[bidno].conflicting_bids.size();
	for(i=0;i<size;i++) {
		b_no = tob[bidno].conflicting_bids[i];
		if(tob[b_no].used) {
			tob[b_no].used = false;
			comp[tob[b_no].cid].used = false;
			for(int k=0; k<tob[b_no].norc; k++){
				reg[tob[b_no].region[k]].used = false;
			}
			currentVal -= tob[b_no].price;
		} 
	}
	for(i=0;i<tob[bidno].norc;i++){
			reg_no = tob[bidno].region[i];
			reg[reg_no].used = true;
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

bool anyCommonRegion(int a, int b) {
	int i,j;
	for(i=0;i<tob[a].norc;i++) {
		for(j=0;j<tob[b].norc;j++) {
			if(tob[a].region[i]==tob[b].region[j])
				return true;
		}
	}
	return false;
}

int main(int argc, char* argv[]){
	time(&start);
	
	readFile(argv[1]);
	sort(stob,stob+nob,compareBids);
	int i,j;
	for(i=0;i<nor;i++) {
		reg[i].noOfBids = 0;
	}
	for(i=0; i<noc; i++){
		comp[i].noOfBids = 0;
	}
	for(i=0;i<nob;i++) {
		for(j=0;j<tob[i].norc;j++) {
			reg[tob[i].region[j]].bid_nos.push_back(i);
			reg[tob[i].region[j]].noOfBids++;
			reg[tob[i].region[j]].used = false;
			comp[tob[i].cid].bid_nos.push_back(i);
			comp[tob[i].cid].noOfBids++;
			comp[tob[i].cid].used = false;
		}
		for(j=i+1;j<nob;j++) {
			if(tob[j].cid==tob[i].cid) {
				tob[i].conflicting_bids.push_back(j);
				tob[j].conflicting_bids.push_back(i);
				continue;
			} else if (anyCommonRegion(i,j)) {
				tob[i].conflicting_bids.push_back(j);
				tob[j].conflicting_bids.push_back(i);
			}
		}
	}

	ofstream outputFile;
	outputFile.open(argv[2]);

	randomStart();
	
	cout<<maxVal<<endl;
	for(i=0; i<vec.size(); i++){
		outputFile<<tob[vec[i]].bid_id<<" ";
	}
	outputFile<<"#";
	outputFile.close();
	
	return 0;
}
