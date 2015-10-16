#include "../include/ECal.hh"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <iomanip>

template <typename ForwardIt>
ForwardIt next(ForwardIt it, 
               typename std::iterator_traits<ForwardIt>::difference_type n = 1)
{
  std::advance(it, n);
  return it;
}

ECal::ECal(float x, float y){
  displayx = x;
  displayy = y;

  center = sf::Vector2f( displayx/2.0, displayy/2.0 );
  // All units unless otherwise stated are in mm
  mylar  = 1.0;
  size42 = 42; 
  size40 = 40;
  size38 = 38;
  s42 = sf::Vector2f( size42, size42 );
  s40 = sf::Vector2f( size40, size40 );
  s38 = sf::Vector2f( size38, size38 );

  // Useful counting variables
  count = 0;
  count42 = 0;
  count40 = 0;
  count38 = 0;
  minx = 0;
  maxx = 0;
  miny = 0;
  maxy = 0;
  countnodes = 0;

  // Setup Modules
  module42.setSize( s42 );
  module42.setOrigin( 0.5*size42, 0.5*size42 );
  module42.setFillColor( sf::Color(166,176,16) );
  module42.setOutlineThickness( -mylar );
  module42.setOutlineColor( sf::Color::Black );

  module40.setSize( s40 );
  module40.setOrigin( 0.5*size40, 0.5*size40 );
  module40.setFillColor( sf::Color(166,176,16) );
  module40.setOutlineThickness( -mylar );
  module40.setOutlineColor( sf::Color::Black );

  module38.setSize( s38 );
  module38.setOrigin( 0.5*size38, 0.5*size38 );
  module38.setFillColor( sf::Color(166,176,16) );
  module38.setOutlineThickness( -mylar );
  module38.setOutlineColor( sf::Color::Black ); 

  // Make Generic Node
  maxclustersize = 32;
  increment = 80; 
  incrementy = 160;
  if( maxclustersize == 32 ) {
    clustercutx = 2.1;
    clustercuty = 4.1;
  }
  if( maxclustersize == 64 ) {
    clustercutx = 4.1;
    clustercuty = 4.1;
  }
  nodeR = 5.0;
  node.setRadius( nodeR );
  sf::FloatRect origin = node.getLocalBounds();
  node.setOrigin(0.5*origin.width,0.5*origin.height);
  node.setFillColor( sf::Color(36,23,115) );

  // Initialize color vectors
  sf::Color red = sf::Color(51,0,0);
  sf::Color yellow = sf::Color(51,51,0);
  sf::Color blue = sf::Color(0,0,51);
  sf::Color green = sf::Color(0,51,0);
  sf::Color cyan = sf::Color(0,51,51);
  sf::Color magenta = sf::Color(51,0,51);
  colors.push_back( yellow );
  colors.push_back( green );
  colors.push_back( cyan );
  colors.push_back( red );
  colors.push_back( blue );
  colors.push_back( green );
 
  sf::Color red_sfml = sf::Color::Red;
  sf::Color yellow_sfml = sf::Color::Yellow;
  sf::Color blue_sfml = sf::Color::Blue;
  sf::Color green_sfml = sf::Color::Green;
  sf::Color cyan_sfml = sf::Color::Cyan;
  sf::Color magenta_sfml = sf::Color::Magenta; 
  sf::Color orange_sfml = sf::Color(255,128,0);
  sf::Color lightgreen_sfml = sf::Color(24,218,114);
  sf::Color purple_sfml = sf::Color(111,16,195);
  sf::Color pink_sfml = sf::Color(232,78,238);
  sf::Color lightblue_sfml = sf::Color(8,122,164);
  sf::Color lightpink_sfml = sf::Color(242,124,210);
  boardercolors.push_back( lightblue_sfml );
  boardercolors.push_back( yellow_sfml );
  boardercolors.push_back( pink_sfml );
  boardercolors.push_back( green_sfml );
  boardercolors.push_back( purple_sfml );
  boardercolors.push_back( orange_sfml );
  boardercolors.push_back( cyan_sfml );
  boardercolors.push_back( lightpink_sfml );
  boardercolors.push_back( lightgreen_sfml );
  boardercolors.push_back( red_sfml );
  boardercolors.push_back( blue_sfml );
  boardercolors.push_back( green_sfml );
  boardercolors.push_back( magenta_sfml );

  // Handle text indices on nodes
  if( !font.loadFromFile("fonts/arial.ttf")) {
    std::cerr << "ERROR: Font did not load properly." << std::endl;
  }
  textind.setFont(font);
  textind.setCharacterSize( 15 );
  textind.setColor( sf::Color::Black );

  // Handle boarders
  lines = sf::VertexArray(sf::LinesStrip,2);

  // Handle keyboard input
  time = 0.0;
  logboarders = false;
  logcolors = false;
  control = false;
  crescent = false;
  indexthenodes = false;
  indexthemods = false;
  count1 = 0;
  count2 = 0;
  count3 = 0;
  count4 = 0;
  count5 = 0;
}

void ECal::initializeECal() {
  std::ifstream layout;
  layout.open("ecal_layout.txt");
  std::string line;
  float yoffset = 40.0;

  // Trigger Efficiency Calorimeter Shape
  // we need to exclude the perimeter modules
  std::set<int>  cellnumberTE;
  std::set<int>::iterator vit;
  std::set<int> TEcells;
  TEcells.insert( 1728 );
  TEcells.insert( 1737 );
  TEcells.insert( 1738 );
  TEcells.insert( 1739 );
  TEcells.insert( 1740 );
  TEcells.insert( 1677 ); // Might need to be removed.
  TEcells.insert( 1678 );
  TEcells.insert( 1679 );
  TEcells.insert( 1680 );
  TEcells.insert( 1681 );
  //TEcells.insert( 1601 );
  TEcells.insert( 1602 );
  TEcells.insert( 1603 );
  TEcells.insert( 1604 );
  TEcells.insert( 1605 );
  //TEcells.insert( 1511 );
  TEcells.insert( 1512 );
  TEcells.insert( 1513 );
  //TEcells.insert( 1409 );
  TEcells.insert( 1410 );
  TEcells.insert( 1411 );
  TEcells.insert( 1412 );
  TEcells.insert( 1413 );
  TEcells.insert( 1184 );
  TEcells.insert( 1185 );
  TEcells.insert( 1180 );
  TEcells.insert( 1181 );
  TEcells.insert( 1011 );
  TEcells.insert( 1068 );
  TEcells.insert( 1069 );
  TEcells.insert( 635 );
  TEcells.insert( 578 );
  TEcells.insert( 527 );
  TEcells.insert( 360 );
  TEcells.insert( 361 );
  TEcells.insert( 362 );
  TEcells.insert( 266 );
  TEcells.insert( 179 );
  TEcells.insert( 180 );
  TEcells.insert( 103 );
  TEcells.insert( 104 );
  TEcells.insert( 105 );
  TEcells.insert( 43 );
  TEcells.insert( 44 );
  TEcells.insert( 45 );
  TEcells.insert( 34 );
  TEcells.insert( 35 );


  if( layout.is_open() ) {
    while( std::getline( layout, line) && line[0]!='#') {}    
    
    int type, cell, row, col, x, y, ncol;
    while( layout >> type >> cell >> row >> col >> x >> y >> ncol ){
      y *= -1;
      y += yoffset;
      sf::Vector2f cellposition( 0.5*displayx + float(x), 0.5*displayy + float(y) );
      
      maxx = (x > maxx) ? x : maxx;
      minx = (x < minx) ? x : minx;
      maxy = (y > maxy) ? y : maxy;
      miny = (y < miny) ? y : miny;  

      // Work on defining TE ECal crescent shape (exclude perimeter modules)
      // I will take advantage of row/col & ncols. If row or col = 1, then 
      // it can be ignored. Also, if col = ncol, then it can be ignored as well
      bool found = false;
      if( row != 1 ) {
	if( col != 1 ) { 
	  if( col != ncol ) {
	    if( row != 80 ) {
	      for(vit = TEcells.begin(); vit != TEcells.end(); vit++ ) {
		if( *vit == cell ) {
		  found = true;
		}
	      }
	      if( !found ) {
		cellnumberTE.insert( cell );
	      }
	    }
	  }
	}
      }

      if( type == 42 ) {
	module42.setPosition( cellposition );
	modules.push_back( module42 );
	modmap[cell] = module42;
	count++;
	count42++;
      }
      if( type == 40 ) {
	module40.setPosition( cellposition );
	modules.push_back( module40 );
	modmap[cell] = module40;
	count++;
	count40++;
      }
      if( type == 38 ) {
	module38.setPosition( cellposition );
	modules.push_back( module38 );
	modmap[cell] = module38;
	count++;
	count38++;
      } 
    }
    layout.close();
  }
  else {
    std::cerr << "Error opening ecal_layout.txt" << std::endl;
  }
  ecalminy = miny;
  ecalmaxy = maxy;
  ecalminx = minx;
  ecalmaxx = maxx;

  // Output of Trigger Efficiency Layout
  std::ofstream output("TE_layout_oct13.txt");
  if(output.is_open() ) {
    for(vit = cellnumberTE.begin(); vit != cellnumberTE.end(); vit++ ) {
      for( mapit = modmap.begin(); mapit != modmap.end(); mapit++ ) {
  	if( *vit == mapit->first ) {
  	  //mapit->second.setFillColor( sf::Color::Blue );
  	}
      }
      output << *vit << std::endl;
    }
  }
  output.close();

  // Make transparent rectangle that boarders ECal
  float xmoduleoffset = (38 + 40) / 2.0;
  float ymoduleoffset = (38 + 42) / 2.0;

  sf::Vector2f bsize( float(maxx - minx) + xmoduleoffset, float(maxy - miny ) + ymoduleoffset );
  sf::Vector2f offset( float(maxx - abs(minx)), float(maxy - abs(miny)));
  sf::Vector2f boarderPos( center.x + offset.x*0.5 + 0.5*mylar, center.y + offset.y*0.5 + mylar );

  boarder.setSize( bsize ); 
  boarder.setFillColor( sf::Color(255,0,0,25) );
  boarder.setOrigin(0.5*bsize.x,0.5*bsize.y);
  boarder.setPosition( boarderPos );

  // Make nodes, excluding the perimeter
  sf::Vector2f boarderCenter = boarder.getPosition();
  sf::Vector2f boarderSize(0.5*boarder.getSize().x, 0.5*boarder.getSize().y);

  float totalX = 2.0*boarderSize.x - increment;
  float totalY = 2.0*boarderSize.y - incrementy;
  int numberX = int(totalX)/increment + 1; 
  int numberY = int(totalY)/incrementy + 1;

  // Start Top Left then move in by (increment,increment), then iterate
  sf::Vector2f inc(increment,increment);
  sf::Vector2f start = boarderCenter - boarderSize + inc;
  float nodeYoffset = 0.0;
  // Rows then columns
  for( int row=0; row<numberY; row++ ) {
    for( int col=0; col<numberX; col++ ) {
      sf::Vector2f tempnode(start.x + col*increment, start.y + row*incrementy + fabs(nodeYoffset) );

      for( modit = modules.begin(); modit != modules.end(); modit++ ) {
	// Peak at next row
	if( col > numberX-3 ) {
	  std::vector<sf::RectangleShape>::iterator peak = modit;
	  peak++;
	  float currentsize = modit->getSize().y;
	  float nextsize = peak->getSize().y;
	  //std::cout << currentsize << ", " << nextsize << std::endl;
	  if( nextsize != currentsize && nextsize > 0 && currentsize > 0 ) {
	    nodeYoffset = (nextsize - currentsize)+1;
	  }
	}
	sf::Vector2f tempmod = (*modit).getPosition();
	sf::Vector2f distance = tempnode - tempmod;
	sf::Vector2f size = modit->getSize();
	float D = sqrt(pow(distance.x,2)+pow(distance.y,2));
	if( fabs( distance.x ) < 0.5*size.x && fabs( distance.y ) < 0.5*size.y ){
	  countnodes++;
	  node.setPosition( tempnode );
	  //node.setPosition( (*modit).getPosition() );
	  nodes.push_back( node );	
	}
      }
      
    }
  }
}  

void ECal::triggerlogic() {
  // LOGIC GROUPS WITH LESS THAN 32 MODULES
  /////////////////////////////////////////
  std::ifstream bad_logic;
  bad_logic.open("nodes_less_32.txt");
  std::string line;
  std::vector<int> badnodes;
  std::vector<int>::iterator vit;
  if( bad_logic.is_open() ) {
    while( getline(bad_logic,line) ) {
      if( line[0] != '#' ) {
	int node;
	std::stringstream first(line);
	first >> node;
	badnodes.push_back( node-1 );
      }
    }
  }
  else std::cerr << "Error opening text" << std::endl;
  bad_logic.close();
  ///////////////////////////////////////////

  for( int i=0; i<nodes.size(); i++ ) {
    // for( int i=150; i<151; i++ ) {
    if( i==20 || i==31  || i==43  || i==57  || i==71  || i==85  ||
    	i==98 || i==111 || i==124 || i==137 || i==151 || i==165 ||
    	i==179 || i ==191 || i==202 || i==211 ) {
    //if( i==1000 ) {
      sf::Vector2f nodetemp = nodes[i].getPosition();
      sf::Vector2f centerlogic(0,0);
      sf::Vector2f neighbors(0,0);
      int n = 0;
      int m = 0;
      std::map<int,int> cellsafe;
      std::map<int,int>::iterator cells;
      std::set<int> cells_taken;
      std::set<int>::iterator cellset;
      std::vector<float> size_in_x;
      std::set<float>::iterator vit;
      std::set<float> size_in_y;
    
      bool taken = true;

      cluster.clear();
      cellsafe.clear();
      cells_taken.clear();
      size_in_x.clear();
      size_in_y.clear();
      final.clear();

      // Locate the center of a logic pattern
      float mindistance;
      int closest_cell = -1;
      float maximumy;
      for( mapit = modmap.begin(); mapit != modmap.end(); mapit++ ){
	sf::Vector2f temp = (*mapit).second.getPosition();
	sf::Vector2f D = nodetemp - temp;
	float distance = sqrt( pow(D.x,2) + pow(D.y,2) );
	if( closest_cell == -1 || distance < mindistance ){
	  mindistance = distance;
	  closest_cell = mapit->first;
	  maximumy = temp.y;
	}
      }
      cluster[n++] = modmap[closest_cell];
      cellsafe[m++] = closest_cell;
      cells_taken.insert( closest_cell );
      size_in_x.push_back( maximumy );
      size_in_y.insert( maximumy );
    
      // Nearest neighbors routine
      clusterit = cluster.begin();
      while( clusterit != cluster.end() && cluster.size() < maxclustersize ){
	centerlogic = clusterit->second.getPosition();
	int clusterindex = clusterit->first; //position in cluster array
	int clustercell = cellsafe[clusterindex];

	// Get the closest modules and add to Logic Cluster
	for( clustit = modmap.begin(); clustit != modmap.end(); clustit++ ) {
	  int clustcell = clustit->first;
	  if( clustcell != clustercell ){
	    taken = true;
	    neighbors = clustit->second.getPosition();
	    sf::Vector2f Dclust = neighbors - centerlogic;
	    sf::Vector2f Dnode = neighbors - nodetemp;
	    float distance = sqrt( pow(Dclust.x,2) + pow(Dclust.y,2) );

	    if( fabs(Dnode.x) < clustercutx*size42 && fabs(Dnode.y) < clustercuty*size42 && cluster.size() < maxclustersize && distance < 1.2*size42 ) {
	    
	      taken =  cells_taken.find( clustit->first ) != cells_taken.end(); 
	      int mycount_in_x = 0;
	      int mycount_in_y = 0;
	      if( !taken ) {
		size_in_x.push_back( neighbors.y );
		size_in_y.insert( neighbors.y );
		mycount_in_x = std::count( size_in_x.begin(), size_in_x.end(), neighbors.y );
		mycount_in_y = size_in_y.size(); 

		if( cells_taken.size() < maxclustersize ) {	      
		  if( mycount_in_x <= 4 && mycount_in_y <= 8 ) {
		    cellsafe[ m++ ] = clustit->first;
		    cluster[ n++ ] = clustit->second;
		    cells_taken.insert( clustit->first );
		  }
		}
	      
	      }
	    }	    	  
	  }   
	} 
	++clusterit;
      }
      // for( vit = size_in_y.begin(); vit != size_in_y.end(); vit++ ) {
      //   std::cout << *vit << std::endl;
      // }
      // std::cout << size_in_y.size() << std::endl;
    
      // Change color of clusters - overlaps handled in colorthelogic() 
      // ***this routine is necessary to make a map of cell number and shape
      for( clustit = cluster.begin(); clustit != cluster.end(); clustit++ ) {
	int temp = i % colors.size();
	(*clustit).second.setFillColor( colors[temp] );
	for( cells = cellsafe.begin(); cells != cellsafe.end(); cells++ ) {
	  // counting variable must be equal
	  if( clustit->first == cells->first ) {
	    // make the final map 
	    final[cells->second] = clustit->second;
	  }
	}
      }
      // Add to global logic vector used throughout the rest of the code
      global_logic.push_back( final );
      
    }
  }
}

void ECal::colorthelogic() {
  // Use cell number to compare if cluster cells overlap
  for( glit = global_logic.begin(); glit != global_logic.end(); glit++ ) {
    for( clustit = glit->begin(); clustit != glit->end(); clustit++ ) {
      
      int currentLogicCluster = clustit->first;

      for( glit_rest = global_logic.begin(); glit_rest != global_logic.end(); glit_rest++ ) {
	if( glit != glit_rest ) {
	  for( clusterit = glit_rest->begin(); clusterit != glit_rest->end(); clusterit++ ) {
	    
	    int nextLogicCluster = clusterit->first;

	    // Check to see if cell # is the same, if so then add colors to achieve overlapping effect
	    if( currentLogicCluster == nextLogicCluster ) {
	      sf::Color first = (clustit->second).getFillColor();
	      sf::Color second = (clusterit->second).getFillColor();
	      sf::Color add = first + second;
	      clusterit->second.setFillColor(add);
	    }

	  }     
	}
      }
    }
  }
}

void ECal::logicboarder() {
  maxy = -1;
  miny = 10000;
  maxx = -1;
  minx = 10000;

  std::set<float> xvalues, maxminx;
  std::set<float>::iterator sit;

  // mm denotes a set that only includes max/min x values
  std::map<float, std::set<float> > rows_xpos, rows_xpos_mm;
  std::map<float, std::set<float> >::iterator rowit, rowit_mm;

  // carry module size
  std::map<float, sf::RectangleShape> rows_new;
  std::map<float, sf::RectangleShape>::iterator rowit_new;

  float tempy = 0;
  int ncolor = -1;
  float yoffset = 1.0;
  int offsetYorN = -1;
  float maxlogicy = -1000;

  // The following routine creates a map where the key is the y-coordinate of
  // a cell and the value is a set which contains the max/min x-coordinates of 
  // cells within a single row

  for( glit = global_logic.begin(); glit != global_logic.end(); glit++ ) {
    ncolor++;
    offsetYorN++;
    int tempC = ncolor % boardercolors.size();
    float first = true;

    for( clustit = glit->begin(); clustit != glit->end(); clustit++ ) {

      sf::Vector2f temp = clustit->second.getPosition();     
      sf::Vector2f modulesize = clustit->second.getSize();

      std::map<int,sf::RectangleShape>::iterator nextelement = next(clustit);

      float currenty = clustit->second.getPosition().y;
      float nexty = nextelement->second.getPosition().y;

      if( temp.y > maxlogicy ) {
      	maxlogicy = temp.y;
      }
      else{
      	maxlogicy = maxlogicy;
      }
      
      if( currenty != nexty ) {
	maxx = temp.x;
	xvalues.insert( temp.x );
      }

      if( currenty == nexty  ) {
      	xvalues.insert( temp.x );
      	if( temp.x < minx ) {
      	  minx = temp.x;
      	}
      	else 
      	  minx = minx;
      }
      else {	
	rows_xpos[ currenty ] = xvalues;
	maxminx.insert(minx);
	maxminx.insert(maxx);
	
	rows_xpos_mm[ currenty ] = maxminx;	
	rows_new[ currenty ] = clustit->second;
	
	minx = 10000;
	maxx = -1;
	xvalues.clear();
	maxminx.clear();
      }   
    }

    // for( rowit_mm = rows_xpos_mm.begin(); rowit_mm != rows_xpos_mm.end(); rowit_mm++ ) {
    //   std::cout << "y= " << rowit_mm->first << std::endl;
    //   for( sit = rowit_mm->second.begin(); sit != rowit_mm->second.end(); sit++ ) {
    // 	std::cout << *sit << std::endl;
    //   }
    // }


    // Use this mapping to create a boarder around a logic pattern:
    for( rowit_mm = rows_xpos_mm.begin(); rowit_mm != rows_xpos_mm.end(); rowit_mm++ ) {
      float tempy = rowit_mm->first;

      std::map<float,std::set<float> >::iterator dupe = rowit_mm;
      dupe++;
      float nexty = dupe->first;

      for( rowit_new = rows_new.begin(); rowit_new != rows_new.end(); rowit_new++ ) {
	if(  rowit_mm->first == rowit_new->first ) {
	  // Get module size of current & next row
	  // current
	  sf::Vector2f size = rowit_new->second.getSize();
	  // peak at next
	  std::map<float, sf::RectangleShape>::iterator mdupe = rowit_new;
	  mdupe++;
	  sf::Vector2f nsize = mdupe->second.getSize();

	  // Grab max and min x values 
	  std::set<float> tempx = rowit_mm->second;
	  std::set<float>::iterator it = tempx.begin();
	  float min = *it;
	  std::advance(it, 1);
	  float max = *it;

	  std::set<float> nexttempx = dupe->second;
	  std::set<float>::iterator nextit = nexttempx.begin();
	  float nextmin = *nextit;
	  std::advance(nextit, 1);
	  float nextmax = *nextit;

	  //std::cout << min << ", " << max << ", " << nextmin << ", " << nextmax << ", " << tempy <<  std::endl;

	  // Handle overlapping boarders to make it easier to visualize
	  switch( offsetYorN % 6 ) {
	  case 0 : yoffset = 0.0;
	    break;
	  case 1 : yoffset = 1.0;
	    break;
	  case 2 : yoffset = -1.0;
	    break;
	  case 3 : yoffset = 0.5;
	    break;
	  case 4 : yoffset = -0.5;
	    break;
	  case 5 : yoffset = +0.75;
	    break;
	  case 6: yoffset = -0.75;
	    break;
	  default : yoffset = 0.0;
	    break;
	  }
	  
	  if( max == 10000 ) {
	    max = min;
	  }
	  if( nextmax == 10000 ) {
	    nextmax = nextmin;
	  }

	  // Draw the boardering lines using Vertex Arrays
	  // top
	  if( rowit_mm == rows_xpos_mm.begin() ) {
	    lines[0].position = sf::Vector2f( max+0.5*size.x-yoffset, tempy-0.5*size.x-yoffset ); 
	    lines[0].color = boardercolors[ tempC ];
	    lines[1].position = sf::Vector2f( min-0.5*size.x-yoffset, tempy-0.5*size.x-yoffset );
	    lines[1].color = boardercolors[ tempC ];
	    boarderthelogic.push_back( lines );
	  }
	  // bottom
	  if( tempy == maxlogicy) {
	    lines[0].position = sf::Vector2f( max+0.5*size.x-yoffset, tempy+0.5*size.x-yoffset ); 
	    lines[0].color = boardercolors[ tempC ];
	    lines[1].position = sf::Vector2f( min-0.5*size.x-yoffset, tempy+0.5*size.x-yoffset );
	    lines[1].color = boardercolors[ tempC ];
	    boarderthelogic.push_back( lines );
	  }
	  // Scattered vertical lines
	  // right
	  lines[0].position = sf::Vector2f( max+0.5*size.x-yoffset, tempy-0.5*size.x-yoffset );
	  lines[0].color = boardercolors[ tempC ];
	  lines[1].position = sf::Vector2f( max+0.5*size.x-yoffset, tempy+0.5*size.x-yoffset );
	  lines[1].color = boardercolors[ tempC ];
	  boarderthelogic.push_back( lines );
	  // left
	  lines[0].position = sf::Vector2f( min-0.5*size.x-yoffset, tempy-0.5*size.x-yoffset );
	  lines[0].color = boardercolors[ tempC ];
	  lines[1].position = sf::Vector2f( min-0.5*size.x-yoffset, tempy+0.5*size.x-yoffset );
	  lines[1].color = boardercolors[ tempC ];
	  boarderthelogic.push_back( lines );
	  // Scattered horizontal lines
	  // right
	  if( dupe != rows_xpos_mm.end() && nsize.x > 0 ) {
	    lines[0].position = sf::Vector2f( max+0.5*size.x-yoffset, tempy+0.5*size.x-yoffset );
	    lines[0].color = boardercolors[ tempC ];
	    lines[1].position = sf::Vector2f( nextmax+0.5*nsize.x-yoffset, nexty-0.5*nsize.x-yoffset );
	    lines[1].color = boardercolors[ tempC ];
	    boarderthelogic.push_back( lines );
	    // left
	    lines[0].position = sf::Vector2f( min-0.5*size.x-yoffset, tempy+0.5*size.x-yoffset );
	    lines[0].color = boardercolors[ tempC ];
	    lines[1].position = sf::Vector2f( nextmin-0.5*nsize.x-yoffset, nexty-0.5*nsize.x-yoffset );
	    lines[1].color = boardercolors[ tempC ];
	    boarderthelogic.push_back( lines );
	  }
	}
      }
    }  
    manyboarders.push_back( boarderthelogic );
    boarderthelogic.clear();
    rows_xpos_mm.clear();
    rows_new.clear();
  }  
}

void ECal::specs() {
  // Spit out useful ECal information:
  std::cout << "Total modules: " << count << std::endl;
  std::cout << "Type 42: " << count42 << std::endl;
  std::cout << "Type 40: " << count40 << std::endl;
  std::cout << "Type 38: " << count38 << std::endl;

  std::cout << "From center of red box - minx, maxx, miny, maxy = " << ecalminx << ", " << ecalmaxx << ", " 
  	    << ecalminy << ", " << ecalmaxy << std::endl;

  std::cout << "Size of box surrounding ECal: " << boarder.getSize().x << ", " << 
    boarder.getSize().y << std::endl;

  std::cout << "Number of nodes with " << increment << " mm spacing: " << countnodes << std::endl;

  std::cout << "Cluster sum = " << maxclustersize << std::endl;
}

void ECal::logicinfo() {  
  // Spit out a text file with cell number + location in x and y (mm) relative
  // to the center of ECal
  std::ofstream logic_file("ecal_triggerlogic_oct15_FINAL.txt");
  if( logic_file.is_open() ) {
    logic_file << "# Units are in mm. ECal is shifted by +40 mm in y relative to previous output." << std::endl;
    logic_file << "# Coordinates are relative to ECal center, same system as G4SBS" << std::endl;
    logic_file << "# Number of logic patterns = " << global_logic.size() << std::endl;
    logic_file << "# Type 42: " << count42 << std::endl;
    logic_file << "# Type 40: " << count40 << std::endl;
    logic_file << "# Type 38: " << count38 << std::endl;
    logic_file << "# Total number of modules: " << count << std::endl;
    logic_file << std::endl;
    logic_file << std::setw(5) << "#cell" << std::setw(5) << "x" 
	       << std::setw(5) << "y"    << std::setw(7) << "size" << std::endl; 
    
    for( glit = global_logic.begin(); glit != global_logic.end(); glit++ ) {
      for( mapit = glit->begin(); mapit != glit->end(); mapit++ ) {
	sf::Vector2f temp = mapit->second.getPosition() - center;
	sf::Vector2f size = mapit->second.getSize();
	
	logic_file << std::setw(5) << mapit->first << std::setw(6) << temp.x 
		   << std::setw(6) << -1*temp.y    << std::setw(5) << size.x << std::endl; 
      }
      logic_file << "######################" << std::endl;
    }
  }
  else std::cerr << "Error opening text output." << std::endl;
  
  logic_file.close();
}

void ECal::controldrawings(sf::Time elapsed) {
  time += elapsed.asSeconds();
  if(time > 0.30 ){
    control = true;
  }
  if(control) {
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::A) ) {
      logboarders = true;
      control = false;
      time = 0;
      count1++;
      if(count1%2==0) {
	logboarders = false;
      }
    }
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::Z) ) {
      logcolors = true;
      control = false;
      time = 0;
      count2++;
      if(count2%2==0){
	logcolors = false;
      }
    } 
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::Q) ) {
      crescent = true;
      control = false;
      time = 0;
      count3++;
      if(count3%2==0){
	crescent = false;
      }
    } 
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::X) ) {
      indexthenodes = true;
      control = false;
      time = 0;
      count4++;
      if(count4%2==0){
    	indexthenodes = false;
      }
    } 
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::C) ) {
      indexthemods = true;
      control = false;
      time = 0;
      count5++;
      if(count5%2==0){
    	indexthemods = false;
      }
    } 
  }
}

void ECal::indexnodes() {
  int nodeindex = 1;
  int modindex = 1;
  sf::Vector2f offset( 2*nodeR, 0.0 );
  
  for( nodit = nodes.begin(); nodit != nodes.end(); nodit++ ) { 
    // Handle the index text - int conversion to string
    std::stringstream temp;
    temp << nodeindex;
    std::string indexstring = temp.str();
    nodeindex++;

    // Grab node position
    sf::Vector2f tempposition = nodit->getPosition();
    sf::Vector2f final = tempposition + offset;
    // Handle the text properties
    textind.setString( indexstring );
    textind.setPosition( final.x, final.y );
    textnodes.push_back( textind );
  }

  // INDEX MODULES IF NEEDED
  textind.setColor( sf::Color::White );
  for( mapit = modmap.begin(); mapit != modmap.end(); mapit++ ) { 
    // Handle the index text - int conversion to string
    std::stringstream temp;
    temp << modindex;
    std::string indexstring = temp.str();
    modindex++;

    // Handle the text properties
    textind.setString( indexstring );
    sf::FloatRect tmprect = textind.getLocalBounds();
    offset = sf::Vector2f(-tmprect.width / 2.0, -tmprect.height / 2.0 );

    // Grab node position
    sf::Vector2f tempposition = mapit->second.getPosition();
    sf::Vector2f final = tempposition+offset;
    
    textind.setPosition( final.x, final.y );
    textmods.push_back( textind );
  }
}

void ECal::draw(sf::RenderTarget& target, sf::RenderStates) const{
  std::map<int,sf::RectangleShape>::const_iterator cit;
  if( !crescent ) {
    if( global_logic.size() != nodes.size() ) {
      for( cit = modmap.begin(); cit != modmap.end(); cit++ ){
	target.draw( cit->second );
      }
    }
    target.draw( boarder );
  }

  std::vector<std::map<int,sf::RectangleShape> >::const_iterator glit_const;
  if( !logcolors ){
    for( glit_const = global_logic.begin(); glit_const != global_logic.end(); glit_const++ ) {
      for( cit = glit_const->begin(); cit != glit_const->end(); cit++ ) {
	target.draw( (*cit).second );
      }
    }
  }

  std::vector<sf::CircleShape>::const_iterator cit1;
  for( cit1 = nodes.begin(); cit1 != nodes.end(); cit1++ ){
    target.draw(*cit1);
  }

  std::vector<std::vector<sf::VertexArray> >::const_iterator cit3;
  std::vector<sf::VertexArray>::const_iterator cit2;
  if( logboarders ) {
    for( cit3 = manyboarders.begin(); cit3 != manyboarders.end(); cit3++ ) {
      for( cit2 = cit3->begin(); cit2 != cit3->end(); cit2++ ) {
	target.draw(*cit2);
      }
    }
  }

  std::vector<sf::Text>::const_iterator textit;
  if( indexthenodes ) {
    for( textit = textnodes.begin(); textit != textnodes.end(); textit++ ) {
      target.draw( *textit );
    }
  }
  if( indexthemods ) {
    for( textit = textmods.begin(); textit != textmods.end(); textit++ ) {
      target.draw( *textit );
    }
  }
}
