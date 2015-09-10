#include "../include/ECal.hh"
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iterator>
#include <algorithm>

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
  increment = 160; 
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

  // Handle boarders
  lines = sf::VertexArray(sf::LinesStrip,2);

  // Handle keyboard input
  time = 0.0;
  logboarders = false;
  logcolors = false;
  control = false;
  crescent = false;
  count1 = 0;
  count2 = 0;
  count3 = 0;
}

void ECal::initializeECal() {
  std::ifstream layout;
  layout.open("ecal_layout.txt");
  std::string line;
  if( layout.is_open() ) {
    while( std::getline( layout, line) && line[0]!='#') {}    
    //std::cout << line << ", count=" << count << std::endl;

    int type, cell, row, col, x, y, ncol;
    while( layout >> type >> cell >> row >> col >> x >> y >> ncol ){
      sf::Vector2f cellposition(0.5*displayx + float(x), 0.5*displayy + float(y));
      //std::cout << "cell = " << cell << std::endl;
      
      maxx = (x > maxx) ? x : maxx;
      minx = (x < minx) ? x : minx;
      maxy = (y > maxy) ? y : maxy;
      miny = (y < miny) ? y : miny;

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

  // Make transparent rectangle that boarders ECal
  float xmoduleoffset = (38 + 40) / 2.0;
  float ymoduleoffset = (38 + 42) / 2.0;

  sf::Vector2f bsize( float(maxx - minx) + xmoduleoffset, float(maxy - miny ) + ymoduleoffset );
  sf::Vector2f offset( float(maxx - abs(minx)), float(maxy - abs(miny)));
  sf::Vector2f boarderPos( center.x + offset.x*0.5 + 0.5*mylar, center.y + offset.y*0.5 - mylar);

  boarder.setSize( bsize ); 
  boarder.setFillColor( sf::Color(255,0,0,25) );
  boarder.setOrigin(0.5*bsize.x,0.5*bsize.y);
  boarder.setPosition( boarderPos );

  // Make nodes, excluding the perimeter
  sf::Vector2f boarderCenter = boarder.getPosition();
  sf::Vector2f boarderSize(0.5*boarder.getSize().x, 0.5*boarder.getSize().y);

  float totalX = 2.0*boarderSize.x - increment;
  float totalY = 2.0*boarderSize.y - increment;
  int numberX = int(totalX)/increment + 1; 
  int numberY = int(totalY)/increment + 1;

  // Start Top Left then move in by (16,16), then iterate
  sf::Vector2f inc(increment,increment);
  sf::Vector2f start = boarderCenter - boarderSize + inc;
  
  // Rows then columns
  for( int row=0; row<numberY; row++ ) {
    for( int col=0; col<numberX; col++ ) {
      sf::Vector2f tempnode(start.x + col*increment, start.y + row*increment );
      bool onlyone = false;
      for( modit = modules.begin(); modit != modules.end(); modit++ ) {
	sf::Vector2f tempmod = (*modit).getPosition();
	sf::Vector2f distance = tempnode - tempmod;
	float D = sqrt(pow(distance.x,2)+pow(distance.y,2));
	if( D <= 0.6*size42 && !onlyone ) {
	  onlyone = true;
	  countnodes++;
	  //node.setPosition( tempnode );
	  node.setPosition( (*modit).getPosition() );
	  nodes.push_back( node );	
	}
      }

    }
  }
}

void ECal::triggerlogic() {

  //for( int i=0; i<nodes.size(); i++ ) {
  for( int i=13; i<14; i++ ) {
    sf::Vector2f nodetemp = nodes[i].getPosition();
    sf::Vector2f centerlogic(0,0);
    sf::Vector2f neighbors(0,0);
    int n = 0;
    int m = 0;
    std::map<int,int> cellsafe;
    std::map<int,int>::iterator cells;
    std::set<int> cells_taken;
    std::set<int>::iterator cellset;

    bool taken = true;

    cluster.clear();
    cellsafe.clear();
    cells_taken.clear();

    // Locate the center of a logic pattern
    float mindistance;
    int closest_cell = -1;
    for( mapit = modmap.begin(); mapit != modmap.end(); mapit++ ){
      sf::Vector2f temp = (*mapit).second.getPosition();
      sf::Vector2f D = nodetemp - temp;
      float distance = sqrt( pow(D.x,2) + pow(D.y,2) );
      if( closest_cell == -1 || distance < mindistance ){
	mindistance = distance;
	closest_cell = mapit->first;
      }
    }
    cluster[n++] = modmap[closest_cell];
    cellsafe[m++] = closest_cell;
    cells_taken.insert( closest_cell );
    
    // Nearest neighbors routine
    clusterit = cluster.begin();
    while( clusterit != cluster.end() && cluster.size() < 64 ){
      centerlogic = clusterit->second.getPosition();
      
      // Get the closest modules and add to Logic Cluster
      for( clustit = modmap.begin(); clustit != modmap.end(); clustit++ ) {
	if( clustit != clusterit ){
	  taken = true;
	  neighbors = clustit->second.getPosition();
	  sf::Vector2f Dclust = neighbors - centerlogic;
	  sf::Vector2f Dnode = neighbors - nodetemp;
	  float distance = sqrt( pow(Dclust.x,2) + pow(Dclust.y,2) );
	  
	  if( fabs(Dnode.x) < 4.1*size42 && fabs(Dnode.y) < 4.1*size42 && cluster.size() < 64 && distance < 1.3*size42 ) {
	    for( cellset = cells_taken.begin(); cellset != cells_taken.end(); cellset++ ) {
	      if( clustit->first == *cellset ) taken = true;
	      else taken = false; 
	    }
	    if( !taken ) {
	      if( cells_taken.size() < 64 ) {
		cellsafe[ m++ ] = clustit->first;
		cluster[ n++ ] = clustit->second;
		cells_taken.insert( clustit->first );
		taken = true;
	      }
	    }
	  }	    	  
	}   
      } 
      ++clusterit;
    }

    // Change color of clusters: 
    // This just gives a cluster color properties. Overlapping effects are
    // handled in colorthelogic() routine. 
    for( clustit = cluster.begin(); clustit != cluster.end(); clustit++ ) {
      int temp = i % colors.size();
      (*clustit).second.setFillColor( colors[temp] );
    }

    for( cells = cellsafe.begin(); cells != cellsafe.end(); cells++ ) {
      std::cout << cells->first << ", " << cells->second << std::endl;
    }
    
    // Add to global logic vector
    global_logic.push_back( cluster );
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

  float maxlogicy = -1;
  bool last = true;

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

  // The following routine creates a map where the key is the y-coordinate of
  // a cell and the value is a set which contains the max/min x-coordinates of 
  // cells within a single row

  for( glit = global_logic.begin(); glit != global_logic.end(); glit++ ) {
    ncolor++;
    offsetYorN++;

    int tempC = ncolor % boardercolors.size();
    for( clustit = glit->begin(); clustit != glit->end(); clustit++ ) {
      
      sf::Vector2f temp = clustit->second.getPosition();     
      sf::Vector2f modulesize = clustit->second.getSize();

      std::map<int,sf::RectangleShape>::iterator nextelement = next(clustit);

      float currenty = clustit->second.getPosition().y;
      float nexty = nextelement->second.getPosition().y;

      if( fabs(temp.y) > maxlogicy ) {
	maxlogicy = temp.y;
      }
      else{
	maxlogicy = maxlogicy;
      }
      
      if( currenty != nexty ) {
	last = true;
	if(last) {
	  maxx = temp.x;
	  xvalues.insert( temp.x );
	  last = false;
	}
      }
      
      if( currenty == nexty ) {
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

	xvalues.clear();
	maxminx.clear();
	maxx = -1;
	minx = 10000;
      }    
    }
  
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

	  // Handle the exception when max = min, i.e. one cell in a row
	  if( max == 10000 ) {
	    max = min;
	  }

	  // Handle overlapping boarders to make it easier to visualize
	  switch( offsetYorN % 3 ) {
	  case 0 : yoffset = 0.0;
	    break;
	  case 1 : yoffset = 1.0;
	    break;
	  case 2 : yoffset = -1.0;
	    break;
	  default : yoffset = 0.0;
	    break;
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
    maxlogicy = 0.0;
    manyboarders.push_back( boarderthelogic );
    boarderthelogic.clear();
    rows_xpos_mm.clear();
    rows_new.clear();
  }  
}

void ECal::specs(){
  // Spit out useful ECal information:
  std::cout << "Total modules: " << count << std::endl;
  std::cout << "Type 42: " << count42 << std::endl;
  std::cout << "Type 40: " << count40 << std::endl;
  std::cout << "Type 38: " << count38 << std::endl;

  std::cout << "minx,maxx,miny,maxy=" << minx << ", " << maxx << ", " 
  	    << miny << ", " << maxy << std::endl;

  std::cout << "Size of box surrounding ECal: " << boarder.getSize().x << ", " << 
    boarder.getSize().y << std::endl;

  std::cout << "Number of nodes with " << increment << " mm spacing: " << countnodes << std::endl;
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

  for( cit1 = logic.begin(); cit1 != logic.end(); cit1++ ){
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
}
