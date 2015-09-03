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

  // Initialize a color vector
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
  boardercolors.push_back( yellow_sfml );
  boardercolors.push_back( green_sfml );
  boardercolors.push_back( cyan_sfml );
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
  for( int i=35; i<45; i++ ) {
    sf::Vector2f nodetemp = nodes[i].getPosition();
    sf::Vector2f centerlogic(0,0);
    sf::Vector2f neighbors(0,0);
    bool first = false;
    int n = 0;

    while( cluster.size() < 64 ) {
      for( mapit = modmap.begin(); mapit != modmap.end(); mapit++ ){
	sf::Vector2f temp = (*mapit).second.getPosition();
	sf::Vector2f D = nodetemp - temp;
	float distance = sqrt( pow(D.x,2) + pow(D.y,2) );

	centerlogic = temp;
	
	// Find starting point, or center of a Logic Cluster
	if( distance < 0.5*size38 && cluster.size() < 64 ) {
	  cluster[mapit->first] = mapit->second;
	
	  for( clusterit = cluster.begin(); clusterit != cluster.end(); clusterit++ ) {
	    centerlogic = clusterit->second.getPosition();
	    n++;

	    // Get the closest modules and add to Logic Cluster
	    for( clustit = modmap.begin(); clustit != modmap.end(); clustit++ ) {
	      if( clustit != mapit && clustit != clusterit ){
		neighbors = clustit->second.getPosition();
		D = neighbors - centerlogic;
		distance = sqrt( pow(D.x,2) + pow(D.y,2) );
	      
		//if( distance < 4.5*size42 && cluster.size() < 64 ) {
		if( fabs(D.x) < 4.0*size42 && fabs(D.y) < 4.0*size42 && cluster.size() < 64 ) {
		  cluster[clustit->first] = clustit->second;
		} 
	      }
	    }
	  
	  }
	}

      }
    }
    // Change color of clusters: 
    // This just gives a cluster color properties. Overlapping effects are
    // handled in colorthelogic() routine. 
    for( clustit = cluster.begin(); clustit != cluster.end(); clustit++ ) {
      int temp = i % colors.size();
      (*clustit).second.setFillColor( colors[temp] );
    }

    // Add to global logic vector, and clear current cluster in order to do the next one
    global_logic.push_back( cluster );
    cluster.clear();
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

  std::map<float, std::vector<sf::RectangleShape> > rows_new;
  std::map<float, std::vector<sf::RectangleShape> >::iterator rowit_new;

  // Necessary to carry module size of max/min
  std::vector<sf::RectangleShape> mm_modules;
  std::vector<sf::RectangleShape>::iterator mm_modit;

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
	  mm_modules.push_back( clustit->second );
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
	rows_new[ currenty ] = mm_modules;
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
	  // Get module size
	  std::vector<sf::RectangleShape> sizet = rowit_new->second;
	  sf::Vector2f size = sizet[0].getSize();
	
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

	  // Handle the exception when max = min, i.e. one cell in a row
	  if( max == 10000 ) {
	    max = min;
	  }
	  if( offsetYorN % 2 == 0 ){
	    yoffset = 0;
	  }
	  else{
	    yoffset = 1.0;
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
	  if( dupe != rows_xpos_mm.end() ) {
	    lines[0].position = sf::Vector2f( max+0.5*size.x-yoffset, tempy+0.5*size.x-yoffset );
	    lines[0].color = boardercolors[ tempC ];
	    lines[1].position = sf::Vector2f( nextmax+0.5*size.x-yoffset, nexty-0.5*size.x-yoffset );
	    lines[1].color = boardercolors[ tempC ];
	    boarderthelogic.push_back( lines );
	    // left
	    lines[0].position = sf::Vector2f( min-0.5*size.x-yoffset, tempy+0.5*size.x-yoffset );
	    lines[0].color = boardercolors[ tempC ];
	    lines[1].position = sf::Vector2f( nextmin-0.5*size.x-yoffset, nexty-0.5*size.x-yoffset );
	    lines[1].color = boardercolors[ tempC ];
	    boarderthelogic.push_back( lines );
	  }
	}
      }
    }    
    manyboarders.push_back( boarderthelogic );
    boarderthelogic.clear();
    rows_xpos_mm.clear();
    mm_modules.clear();
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
