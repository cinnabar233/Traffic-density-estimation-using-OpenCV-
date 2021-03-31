#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<math.h>
using namespace std;
vector<double>read_csv(string filename)
{
    vector<double>v ;
    fstream fin;
    fin.open(filename, ios::in);
    
    
    string line,word,temp,val ;
    double x ;
    
    getline(fin,line);
  //   cout << line <<"\n";
    while(!fin.eof())
    {
        getline(fin,line);
        std::stringstream s(line);
       // cout<<line<<"\n";
        while (getline(s, word, ',')) val = word;
        x = stod(val);
        v.push_back(x);
    }
    return v ; 
}
int main(int argc ,  char ** argv)
{
    vector<double> v1 = read_csv("out_bench(3x).txt");
    vector<double> v2 = read_csv(argv[1]);
    int x = atoi(argv[2]);
   //  vector<dou
    double mse = 0 ;
    // for(double x : v1) cout << x << " " ; cout << "\n";
   //  for(double x : v2) cout << x << " " ; cout << "\n";
    for(int i = 0 ; i < v1.size() ; i++)
    {
        mse+=(v2[i/(x/3)]-v1[i])*(v2[i/(x/3)]-v1[i]);
        
    }
    cout << sqrt(mse/v1.size());
}
