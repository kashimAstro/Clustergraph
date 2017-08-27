#ifndef BayesianNet_H
#define BayesianNet_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <queue>
#include <string>
#include <sstream>

template<class Type>
struct data
{
        double key;
        Type value1;
        Type value2;
};

struct data_compare
{
  bool operator()(const data<int> l, const data<int> r)  {  return l.key > r.key;  }
};

class BayesianNet
{
	protected:
        int traininstances;
        int testinstances;
        int attributes;

	void accuracy(int *outcome , int * result)
	{
		double correct=0;
		for( int i=0 ; i<testinstances; i++)
		{
			if (outcome[i]==result[i])
				correct++;
			//std::cout<<"predict to be "<<outcome[i]<<" is actually "<<result[i]<<std::endl;
		}
		std::cout<<"Total "<<testinstances<<" data have "<<correct<<" correct predictions"<< std::endl;
		double percentage=correct/testinstances;
		std::cout<<"Accuracy is "<<percentage*100<<"%"<<std::endl;
	}
};

class BayesianNetwork: public BayesianNet
{
	protected:
	void classifier(long double ***cpt ,int *numclass ,double *count ,int **parent,char* test_file)
	{
		std::ifstream testInputFile(test_file);
		if(!testInputFile){std::cout<<"Can't open test data file!"<<std::endl;return;}
		std::string Buf;
		int *result= new int[testinstances];
		for(int w=0; w<testinstances; w++)
		{
			result[w]=0;
		}
		int *outcome=new int[testinstances];
		for(int f=0; f<testinstances; f++)
		{
			outcome[f]=0;
		}
		double *oneLine=new double [attributes+1];
		long double *decision=new long double[numclass[attributes]];
		for( int a=0 ; a<testinstances ; a++)
		{
			getline( testInputFile , Buf );
			std::cout <<"Buffer: "<< Buf << std::endl;
			std::stringstream  lineStream(Buf);
			for ( int m=0 ; m<numclass[attributes]; m++)
			decision[m]=1;
			for (int u=0 ; u<=attributes; u++){
				getline( lineStream, Buf , ',' );
				oneLine[u]=stod(Buf);
				////std::cout <<"oneLine: "<< oneLine[u] << std::endl;
			}
			result[a]=oneLine[attributes];
			for( int x=0 ; x<numclass[attributes] ; x++)
			{
				for( int xx=0 ; xx<attributes ; xx++)
				{
					int reg_add=1;
					int reg_mul=1;
					for( int xxx=1 ; xxx<parent[xx][0] ; xxx++)
					{
						reg_add+=(   reg_mul * (static_cast<int>(oneLine[ parent[xx][xxx] ]) -1) );
						reg_mul*=numclass[ parent[xx][xxx] ];
					}
					reg_add+=(reg_mul*x);
					decision[x]*=cpt[xx][static_cast<int>(oneLine[xx])-1][reg_add];
				}	
				decision[x]*=count[x];
				std::cout <<"Decision: "<< decision[x] << std::endl;
			}
			int big=0;                                         
			long double hug=decision[0];
			for ( int v=1 ; v<numclass[attributes] ; v++)
			{
				if ( decision[v]>hug)
				{
					big=v;
					hug=decision[v];
					std::cout << "Hug: "<<hug<< std::endl;
				}
			}
			outcome[a]=(big+1);
		}
		accuracy ( outcome , result );
		delete [] result;
		delete [] decision;
		delete [] oneLine;
		delete [] outcome;
	}

	public:
        BayesianNetwork(char* train_file, char* test_file, char* cfg_file)
	{
		std::ifstream configure;
		configure.open(cfg_file);
		if(!configure){std::cout<<"! Can't open configuration file!"<<std::endl;return;}
		configure>>traininstances>>testinstances>>attributes;
		int *discrete = new int[attributes]; 
		for(int z=0; z<attributes ; z++)
			configure>>discrete[z];
		int *numclass= new int[attributes+1]; 
		for(int v=0; v<=attributes; v++)
			configure>>numclass[v];
		double *count = new double[numclass[attributes]]; 
		for(int c=0; c<numclass[attributes]; c++)
			count[c]=0;
		configure.close();
		int combinations=1;
		for(int com=(attributes-1) ; com>1 ; com--)
			combinations+=com;
		std::cout<<"combinatinos "<< combinations << std::endl<<std::endl; 
	
		std::ifstream trainingDataFile;
		std::string Buf;
		trainingDataFile.open(train_file);
		if(!trainingDataFile){std::cout<<"Can't open training data file!"<<std::endl;return;}
	
		int **rank= new int *[combinations];
		for(int zz=0 ; zz< combinations ; zz++)   
			rank[zz]= new int[2];
		int index=0;
		for(int zzz=0 ; zzz< (attributes-1) ; zzz++)
		{
			for(int ppp=1 ; ppp<=(attributes-zzz-1) ; ppp++)
			{
				rank[index][0]=zzz ;
				rank[index][1]=(zzz+ppp) ;
				index++;
			}
		}
		double **ccc= new double *[ attributes*numclass[attributes] ];
		for(int f=0 ; f< attributes ; f++)
		{
			for(int f1=(f*numclass[attributes]) ; f1 < ( (f+1)*numclass[attributes] ) ; f1++)
				ccc[f1]= new double[ numclass[f] ];
		}
		for(int f2=0 ; f2< attributes ; f2++)
		{
			for(int f3=(f2*numclass[attributes]) ; f3 < ( (f2+1)*numclass[attributes] ) ; f3++)
			{
				for( int f4=0 ; f4< numclass[f2] ; f4++) 
					ccc[f3][f4]=0;
			}
		}
		double **aaa= new double *[ combinations*numclass[attributes] ];
		for(int ff=0 ; ff< combinations ; ff++) 
		{
			for(int ff1=(ff*numclass[attributes]) ; ff1 < ( (ff+1)*numclass[attributes] ) ; ff1++)
			{
				aaa[ff1]= new double[ numclass[ rank[ff][0] ]*numclass[ rank[ff][1] ] ];
	
				for(int ff2=0 ; ff2 < numclass[ rank[ff][0] ]*numclass[ rank[ff][1] ] ; ff2++)
					aaa[ff1][ff2]=0;
			}
		}
		double **bbb= new double *[combinations*numclass[attributes] ];
		for(int fff=0 ; fff< combinations ; fff++)   
		{
			for(int fff1=(fff*numclass[attributes]) ; fff1 < ( (fff+1)*numclass[attributes] ) ; fff1++)
			{
				bbb[fff1]= new double[ numclass[ rank[fff][0] ]*numclass[ rank[fff][1] ] ];
	
				for(int fff2=0 ; fff2<numclass[ rank[fff][0] ]*numclass[ rank[fff][1] ] ; fff2++)
					bbb[fff1][fff2]=0;
			}
		}
	
		int *oneLine = new int[attributes+1];
		for( int ii=1 ; ii<=traininstances; ii++)
		{
			getline( trainingDataFile, Buf );
			std::stringstream  lineStream(Buf);
	
			for (int bb=0 ; bb<=attributes ; bb++){
				getline( lineStream, Buf , ',' );
				oneLine[bb]=stod(Buf);
			}
			for (int h=0 ; h< attributes ; h++)
				ccc[h*numclass[attributes]+oneLine[attributes]-1][oneLine[h]-1 ]++;
	
			for(int hh=0 ; hh < combinations ; hh++)
			{
				aaa[ hh*numclass[attributes]+oneLine[attributes]-1 ]
			    [ (oneLine[ rank[hh][0] ]-1)*numclass[rank[hh][1]] + oneLine[ rank[hh][1] ]-1 ]++;
	
				bbb[ hh*numclass[attributes]+oneLine[attributes]-1 ]
				[ (oneLine[ rank[hh][0] ]-1)*numclass[rank[hh][1]] + oneLine[ rank[hh][1] ]-1 ]++;
			}
			
			count[oneLine[attributes] -1 ] ++;
		}
		delete [] oneLine;
		trainingDataFile.close();
		for( int t=0 ; t< attributes ; t++)
		{	
			for ( int d=0 ; d<numclass[attributes] ; d++)
			{	
				int correction=0;
				for (int o=0 ; o < numclass[t] ; o++)
				{					
					if (ccc[(t*numclass[attributes]+d)][o]==0 )
					{
						correction=numclass[t];
						for(int p=0 ; p <numclass[t] ; p++)
						{
							ccc[(t*numclass[attributes]+d)][p]++;
						}
						break;
					}
				}
				for ( int w=0 ; w<numclass[t] ; w++)
					ccc[(t*numclass[attributes]+d)][w]/=(count[d]+correction);
			}		
		}
	
		for( int tt=0 ; tt< combinations ; tt++)
		{
			int correction1=0;
			for ( int dd=0 ; dd< numclass[attributes] ; dd++)
			{	
				for (int oo=0 ; oo < numclass[ rank[tt][0] ]*numclass[ rank[tt][1] ]  ; oo++)
				{
					if (aaa[ tt*numclass[attributes]+dd ][oo]==0 )
					{
						for(int pp=0 ; pp < numclass[attributes] ; pp++)
						{
							for(int ppp=0 ; ppp < numclass[ rank[tt][0] ]*numclass[ rank[tt][1] ]  ; ppp++)
							{
								aaa[tt*numclass[attributes]+pp][ppp]++;
							}
						}
						correction1= numclass[attributes]*numclass[ rank[tt][0] ]*numclass[ rank[tt][1] ];
						break;
					}
				}
			}
			for(int w1=0 ; w1 < numclass[attributes] ; w1++)
			{
				for(int ww1=0 ; ww1 < numclass[ rank[tt][0] ]*numclass[ rank[tt][1] ]  ; ww1++)
				{
					aaa[tt*numclass[attributes]+w1][ww1]/=( traininstances+correction1 );
				}
			}
		}
		for( int ttt=0 ; ttt< combinations ; ttt++)
		{
			for ( int ddd=0 ; ddd< numclass[attributes] ; ddd++)
			{	
				int correction2=0;
				for (int ooo=0 ; ooo < numclass[ rank[ttt][0] ]*numclass[ rank[ttt][1] ]  ; ooo++)
				{
					if (bbb[ ttt*numclass[attributes]+ddd ][ooo]==0 )
					{
						for(int ppp=0 ; ppp < numclass[ rank[ttt][0] ]*numclass[ rank[ttt][1] ]  ; ppp++)
						{
							bbb[ttt*numclass[attributes]+ddd][ppp]++;
						}
	
						correction2= numclass[ rank[ttt][0] ]*numclass[ rank[ttt][1] ];
						break;
					}
				} 
				for(int w2=0 ; w2 < numclass[ rank[ttt][0] ]*numclass[ rank[ttt][1] ]  ; w2++ )
					bbb[ttt*numclass[attributes]+ddd][w2]/=( count[ddd]+correction2 );
			}
		}
		double *relation = new double[combinations];
		for( int ss=0 ; ss < combinations ; ss++)
		{
			double tempo=0;
			for( int s1=0 ; s1< numclass[attributes] ; s1++)
			{
				for( int s2=0 ; s2< numclass[ rank[ss][0] ] ; s2++)
				{
					for( int s3=0 ; s3< numclass[ rank[ss][1] ] ; s3++)
					{
						tempo+= 
						( 
							aaa[ ss*numclass[attributes]+s1 ][ s2*numclass[ rank[ss][1] ]+s3 ] * 
							log10
							( 
								bbb[ ss*numclass[attributes]+s1 ][ s2*numclass[ rank[ss][1] ]+s3 ] / 
								( ccc[ rank[ss][0]*numclass[attributes]+s1 ]
								[s2]*ccc[ rank[ss][1]*numclass[attributes]+s1 ][s3] ) 
							)   
						);
					}
				}
			}		
			relation[ss]=tempo;
		}
		std::priority_queue< data<int>, std::vector< data<int> >, data_compare  >  maxweight;
		data<int> elen;
		for( int cast=0 ; cast< combinations ; cast++)
		{
			elen.value1 = rank[cast][0];
		  	elen.value2 = rank[cast][1];
		  	elen.key = relation[cast] ;
		  	maxweight.push(elen);
		}
		int *groups= new int[attributes];
		for(int vv=0; vv < attributes; vv++)   
			groups[vv]=0;
		int **graph= new int *[attributes];
		for(int zz1=0 ; zz1 < attributes ; zz1++)   
			graph[zz1]= new int[attributes];
		for(int k1=0; k1<attributes ; k1++)
		{
			for(int kk1=0 ; kk1<attributes  ; kk1++)	
				graph[k1][kk1]=0;
		}
		data<int> mmm;
		int base = 1;
		for(int combi=0 ; combi < combinations ; combi ++ )
		{
			if( !maxweight.empty() ){
				mmm = maxweight.top();
				maxweight.pop();
			}
			if( groups[mmm.value1] != 0 && groups[mmm.value2] != 0 && groups[mmm.value1] == groups[mmm.value2] )
			{}
			else if( groups[mmm.value1] == 0 && groups[mmm.value1] == groups[mmm.value2] )
			{
				groups[mmm.value1]=base;
				groups[mmm.value2]=base;
				base++;
	
				graph[mmm.value1][mmm.value2]=1;
				graph[mmm.value2][mmm.value1]=1;
			}
			else if( groups[mmm.value1] == 0 && groups[mmm.value2] != 0 )
			{
				groups[mmm.value1] = groups[mmm.value2];
	
				graph[mmm.value1][mmm.value2]=1;
				graph[mmm.value2][mmm.value1]=1;			
			}
			else if( groups[mmm.value1] != 0 && groups[mmm.value2] == 0 )
			{
				groups[mmm.value2] = groups[mmm.value1];
	
				graph[mmm.value1][mmm.value2]=1;
				graph[mmm.value2][mmm.value1]=1;
			}
			else if( groups[mmm.value1] != 0 && groups[mmm.value2] != 0 && groups[mmm.value1] !=groups[mmm.value2] )
			{
				int boss,slave;
				if( groups[mmm.value1] <  groups[mmm.value2] )
				{
					boss=groups[mmm.value1];
					slave=groups[mmm.value2];
				}
				else
				{
					boss=groups[mmm.value2];
					slave=groups[mmm.value1];
				}
				for( int scan=0 ; scan < attributes ; scan++)
				{
					if ( groups[scan]==slave )
					{
						groups[scan]=boss;
					}
				}
				graph[mmm.value1][mmm.value2]=1;
				graph[mmm.value2][mmm.value1]=1;
			}
		}
		int *transfer= new int[attributes];
		for(int vvv=0; vvv<attributes; vvv++)   
			transfer[vvv]=attributes;
	
		transfer[0]=0; 
		for(int redo=0 ; redo<attributes ; redo++)
		{
			int min=(attributes+1);
			int point=0;
			for(int redo1=0 ; redo1<attributes ; redo1++)
			{
				if( min > transfer[redo1] )
				{
					min=transfer[redo1];
					point=redo1;
				}
			}
	
			for(int redo2=0 ; redo2<attributes ; redo2++)
			{
				if( graph[point][redo2]==1)
				{
					graph[redo2][point]=0;
					transfer[redo2]=(min+1);
				}
			}
			transfer[point]=(attributes+1);
		}
	
		int **parent= new int *[attributes];
		for(int z=0 ; z< attributes ; z++)   
			parent[z]= new int[attributes+1];
		for(int k=0; k<attributes ; k++)
		{
			for(int kk=0 ; kk<=attributes   ; kk++)	
				parent[k][kk]=0;
		}
		for(int e=0; e<attributes ; e++)
		{
			int pama=1;
			int pamaindex=1;
			for(int ee=0 ; ee<attributes ; ee++)
			{		
				if( graph[ee][e]==1 )
				{
					pama++;
					parent[e][pamaindex]=ee;
					pamaindex++;
				}
	
			}
			parent[e][0]=pama;
			parent[e][pamaindex]=attributes;
		}
		long double ***cpt = new long double**[attributes]; 
		for(int j=0; j<attributes; j++)
		{
	
			cpt[j]=new long double*[ numclass[j] ];
			int reg=1;
			for(int jjj=1 ; jjj<=parent[j][0] ;jjj++) 
				reg*=numclass[ parent[j][jjj] ];
			for(int jj=0 ; jj<numclass[j] ;jj++)
			{
				cpt[j][jj]=new long double[reg+1];
				cpt[j][jj][0]=reg;       
				for(int jjjj=1 ; jjjj<=reg ; jjjj++)
					cpt[j][jj][jjjj]=0;	
			}
	
		}
	
		trainingDataFile.open(train_file);
		if(!trainingDataFile){std::cout<<"Can't open training data file!"<<std::endl;return;}
		double *oneLine_double = new double[attributes+1];
		for( int i=1 ; i<=traininstances; i++)
		{
			getline( trainingDataFile, Buf );
			std::stringstream  lineStream(Buf);
			for (int y=0 ; y<=attributes ; y++){
				getline( lineStream, Buf , ',' );
				oneLine_double[y]=stod(Buf);
			}
			for (int yy=0 ; yy<attributes ;yy++)
			{
				int reg_add=1;
				int reg_mul=1;
				for( int yyy=1 ; yyy<=parent[yy][0] ; yyy++)
				{
					reg_add+=(   reg_mul * (static_cast<int>(oneLine_double[ parent[yy][yyy] ]) -1) );
					reg_mul*=numclass[ parent[yy][yyy] ];
				}
				cpt[yy][(static_cast<int>(oneLine_double[yy])-1)][reg_add]++;
			}
		}
		delete [] oneLine_double;
		trainingDataFile.close();
		for( int t1=0 ; t1< attributes ; t1++)
		{
			for ( int d=1 ; d<=cpt[t1][0][0] ; d++)
			{	
				for (int o=0 ; o < numclass[t1] ; o++)
				{
					if (cpt[t1][o][d]==0 )
					{
						for(int p=0 ; p <numclass[t1] ; p++)
						{
							cpt[t1][p][d]++;
						}
						break;
					}
				}
	
				int sum=0;
				for ( int w=0 ; w<numclass[t1] ; w++)
					sum+=cpt[t1][w][d];
				for ( int ww=0 ; ww<numclass[t1] ; ww++)
					cpt[t1][ww][d]/=sum;
			}
		}
	
		for ( int ppp=0 ; ppp<numclass[attributes] ; ppp++)
		{
			count[ppp]=count[ppp]/traininstances;
		}
		classifier(cpt , numclass ,  count , parent, test_file);
		for( int x=0; x<attributes ; x++)
		{
			for( int xx=0 ; xx< numclass[x] ; xx++)
				delete [] cpt[x][xx];
			delete [] cpt[x];
		}
		
		for( int pa=0 ; pa< attributes ; pa++)
			delete [] parent[pa];
		delete [] parent;
		delete [] cpt;
		delete [] numclass;
		delete [] discrete;
		delete [] count;
	}
};
#endif
