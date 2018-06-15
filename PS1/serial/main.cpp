#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>
#include <iomanip> 

using namespace std;

int prev (int i, int N);
int next (int i, int N);

int main (int argc, char *argv[])
{
	if(argc != 7)
	{
		cout<<"Wrong Number of Inputs."<<endl;
	}
	else
	{
		int N = stod(argv[1]);
		int NT = stod(argv[2]);
		double L = stod(argv[3]);
		double T = stod(argv[4]);
		double u = stod(argv[5]);
		double v = stod(argv[6]);
		cout<<"You have all the inputs."<<endl;
		double dx = L/N;
		double dy = dx;
		double dt = T/(NT);
		cout <<"dt is "<< dt<<endl;
		cout <<"dx is "<< dx<<endl;
		cout << "dt/(2dx) "<< dt/(2*dx)<< endl;
		double x0 = L/2;
		double y0 = L/2;
		double deltax = L/4;
		double deltay = L/4;
		double val;
		double x;
		double y;
		int iprev;
		int inext;
		int jprev;
		int jnext;
		// double test;

		if (dt > dx/sqrt(2*(u*u+v*v)))
		{
			cout<<"Numerically Instable"<<endl;
			exit(1);
		}

// Open Files
		ofstream outfile;
   		outfile.open("data.txt");

//Initialization
		// double C[N][N];
		// double Cnext[N][N];

		vector< vector<double> > C(N,vector<double>(N));
		vector< vector<double> > Cnext(N,vector<double>(N));

		for (int i=0; i<N ; i++)
		{
			for (int j=0; j<N ; j++)
			{
				x = i*dx;
				y = j*dy;
				val = pow((x-x0),2)/(2*pow(deltax,2))+pow((y-y0),2)/(2*pow(deltay,2));
				C[i][j] = exp(-val);
				Cnext[i][j] = 0;
			}
		}


//Lax Scheme
		for (int n = 0; n < NT ; n++)
		{
			for(int i = 0; i< N ; i++)
			{
				for(int j = 0; j < N; j++)
				{
					iprev = prev(i,N);
					inext = next(i,N);
					jprev = prev(j,N);
					jnext = next(j,N);
					//update t_n+1 at i,j
					Cnext[i][j] = 0.25*(C[iprev][j]+C[inext][j]+C[i][jprev]+C[i][jnext])
						- dt/(2*dx)*(u*(C[inext][j]-C[iprev][j])+v*(C[i][jnext]-C[i][jprev]));
				}
			}
			if (n% 10000 == 0)
			{
				// Write to ASCII
				for (int i=0; i < N ; i++)
				{
					for (int j=0; j < N ; j++)
					{
						outfile<<fixed << setprecision(4)<<C[i][j]<<' ';
					} 
					outfile<<endl;
				}
			}
			C = Cnext;
		}

		// Write to ASCII
		for (int i=0; i < N ; i++)
		{
			for (int j=0; j < N ; j++)
			{
				outfile<<fixed << setprecision(4)<<C[i][j]<<' ';
			} 
			outfile<<endl;
		}

//Close file
		outfile.close();
	}
	return 0;
}

int prev (int i, int N)
{
	int prev;
	if (i == 0)
	{
		prev = N-1;
	}
	else
	{
		prev = i-1;
	}
	return prev;
}
int next (int i, int N)
{
	int next;
	if (i == N-1)
	{
		next = 0;
	}
	else
	{
		next = i+1;
	}
	return next;
}
