#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <queue>

#define max 10000
using namespace std;

struct node // bid node structure
{
	int bidno;
	int cid; //company
	double price;  
	int norc;  // no. of regions in one bid
	int *region;
	float weight;
};

struct CompareNode : public std::binary_function<node, node, bool>                                                                                     
{
  bool operator()(const node lhs, const node rhs) const
  {
     return lhs.weight < rhs.weight;
  }
};

float tim; // time
int nor; // number of regions
int nob; // number of bids
int noc; // number of companies
struct node *tob; // total no. of bids
priority_queue<node,vector<node>, CompareNode > weights;

bool *com,*reg; //keeps record of which companies and regions can be selected in the remaining unprocessed bids
bool *bid; // final bids

double average_price;
double sum_price=0;
float average_regions;
int sum_regions=0;

void giveWeights(){
	for(int i=0; i<nob; i++){
		sum_price+=tob[i].price;
		sum_regions+=tob[i].norc;
	}
	average_price=sum_price/nob;
	average_regions=sum_regions/nob;
	for(int i=0; i<nob; i++){
		tob[i].weight=tob[i].price/tob[i].norc*average_regions/average_price;
		weights.push(tob[i]);
	}
}

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

	tob = new node[nob];
	com = new bool[noc];
	reg = new bool[nor];
	bid = new bool[nob];

	for(int i=0;i<nob;i++)
	{
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
		tob[i].bidno=i;
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

void fill(node);
bool checkReg(node);
void getRandom() //modify this function to produce the best output(following the conditions mentioned in the assignment)
{
	//int num1,i;
	//num1=rand()%nob;
	//cout<<num1<<endl;
	//node temp=weights.top();

	// fill(num1);
	// for(i=(num1+1)%nob;i!=num1;i=(i+1)%nob)
	// {
	// 	if(com[tob[i].cid] || checkReg(i))
	// 		continue;
	// 	fill(i);
	// }
	int c=0;
	while ( !weights.empty() )
    {
        node n = weights.top();
        weights.pop();
  		if(com[n.cid] || checkReg(n))
			continue;
		fill(n);
    }

	double count=0;
	for(int i=0;i<nob;i++)
	{
		if(bid[i]) {
			cout<<i<<" ";
			count+=tob[i].price;
		}
	}
	cout<<"# "<<count<<endl;
}

//helper function of getRandom function
void fill(node mybid)
{
	com[mybid.cid]=true;
	bid[mybid.bidno]=true;
	for(int i=0;i<mybid.norc;i++)
	{
		reg[mybid.region[i]]=true;
	}
}	

//helper function of getRandom function
bool checkReg(node mybid)
{
	for(int i=0;i<mybid.norc;i++)
	{
		if(reg[mybid.region[i]]==true)
			return true;
	}
	return false;
}

int main(int argc, char* argv[])
{
	readFile(argv[1]);
	giveWeights();
	getRandom();
	return 0;
}