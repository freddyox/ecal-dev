#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
#include <sstream>

using namespace std;

int main() {
  ifstream logic;
  logic.open("full_logic_sept25_copy.txt");
  string line_temp;
  int logic_number = 0;
  vector<int> cell_values, x_values, y_values, size_values;
  map<int,vector<int> > logic_cell, logic_x, logic_y, logic_size;
  set<int> modulecount;
  vector<int>::iterator vit;
  map<int,vector<int> >::iterator mit;

  if( logic.is_open() ) {
    while( getline(logic,line_temp) ) {
      if( line_temp[0] == '#' ) {
	logic_number++;

	logic_cell[logic_number] = cell_values;
	logic_x[logic_number] = x_values;
	logic_y[logic_number] = y_values;
	logic_size[logic_number] = size_values;
	
	// clear all the vectors so they can be inserted again
	cell_values.clear();
	x_values.clear();
	y_values.clear();
	size_values.clear();
      }
      if( line_temp[0] != '#' ) {
	int cell, x, y, size;
	stringstream first(line_temp);
	first >> cell;
	first >> x;
	first >> y;
	first >> size;
	cell_values.push_back( cell );
	x_values.push_back( x );
	y_values.push_back( -1*y+40 );
	size_values.push_back( size );
	modulecount.insert( cell );
      }
    }
    
    logic.close();
  }
  else
    cerr << "Error opening the text file" << endl;
  
  
  // for( mit = logic_y.begin(); mit != logic_y.end(); mit++ ) {
  //   cout << "log #: " << mit->first << endl;
  //   for( vit = mit->second.begin(); vit != mit->second.end(); vit++ ){
  //     cout << *vit << endl;   
  //   }
  // }

  cout << modulecount.size() << endl;

  return 0;
}
