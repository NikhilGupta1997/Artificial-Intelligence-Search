#include <iostream>
#include <malloc.h>
#include <string>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#define max 10000
using namespace std;

struct node // bid node structure
{
	int cid; //company
	double price;  
	int norc;  // no. of regions in one bid
	int region[max];
};

float tim; 
int nor;
int nob;
int noc;
struct node tob[max]; // total no. of bids

bool com[max],reg[max]; //keeps record of which companies and regions can be selected in the remaining unprocessed bids
bool bid[max]; // final bids


//function to take input - read from console by redirection
void readFile()
{

	string g;
	scanf("%f\n\n",&tim);
	scanf("%d\n\n",&nor);
	scanf("%d\n\n",&nob);
	scanf("%d\n\n",&noc);
	for(int i=0;i<nob;i++)
	{
		cout<<flush;
		string ch;
		getline(cin,ch);
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
		t=w;
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
		getline(cin,g);	
	}
}

void fill(int);
bool checkReg(int);
void getRandom() //modify this function to produce the best output(following the conditions mentioned in the assignment)
{
	int num1,i;
	num1=rand()%nob;
	fill(num1);
	for(i=(num1+1)%nob;i!=num1;i=(i+1)%nob)
	{
		if(com[tob[i].cid] || checkReg(i))
			continue;
		fill(i);
	}
	for(i=0;i<nob;i++)
	{
		if(bid[i]) {
			cout<<i<<" ";
		}
	}
	cout<<"#"<<endl;
}

//helper function of getRandom function
void fill(int bidno)
{
	com[tob[bidno].cid]=true;
	bid[bidno]=true;
	for(int i=0;i<tob[bidno].norc;i++)
	{
		reg[tob[bidno].region[i]]=true;
	}
}	

//helper function of getRandom function
bool checkReg(int bidno)
{
	for(int i=0;i<tob[bidno].norc;i++)
	{
		if(reg[tob[bidno].region[i]]==true)
			return true;
	}
	return false;
}

int main()
{
	readFile();
	getRandom();
	return 0;
}