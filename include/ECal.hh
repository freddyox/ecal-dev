#ifndef ECAL_HH
#define ECAL_HH

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <set>

class ECal : public sf::Drawable, public sf::Transformable {

private:
  float displayx, displayy;
  sf::Vector2f center;
  float mylar;
  float size42, size40, size38;
  int count, count42, count40, count38;
  int minx, maxx, miny, maxy;
  int ecalminy, ecalmaxy, ecalminx, ecalmaxx;
  int countnodes;
  int maxclustersize;
  float clustercutx, clustercutxneg, clustercuty;

  // MODULE and LOGIC Properties
  sf::Vector2f s42, s40, s38;
  sf::RectangleShape module42, module40, module38;
  sf::RectangleShape boarder;

  std::vector<sf::RectangleShape> modules;
  std::vector<sf::RectangleShape>::iterator modit;
  std::map<int,sf::RectangleShape> modmap, cluster, final;
  std::map<int,sf::RectangleShape>::iterator mapit, clustit, clusterit, lastone;

  std::vector<std::map<int,sf::RectangleShape> > global_logic;
  std::vector<std::map<int,sf::RectangleShape> >::iterator glit, glit_rest;

  // Boarders
  std::vector<sf::Color> colors, boardercolors;
  sf::VertexArray lines;
  std::vector<sf::VertexArray> boarderthelogic;
  std::vector<std::vector<sf::VertexArray> > manyboarders;

  // NODE Properties
  float nodeR;
  int increment,incrementy;
  sf::CircleShape node;
  std::vector<sf::CircleShape> nodes;
  std::vector<sf::CircleShape>::iterator nodit;
  std::vector<sf::Text> textnodes;
  sf::Font font;
  sf::Text textind;

  // Control drawings with keyboard 
  float time;
  bool logboarders;
  bool logcolors;
  bool crescent;
  bool control;
  bool indexthenodes;
  int count1, count2, count3, count4;

public:
  ECal(float,float);
  ~ECal() {};

  void draw(sf::RenderTarget&, sf::RenderStates) const;
  void controldrawings(sf::Time);
  void initializeECal();
  void specs();
  void triggerlogic();
  void colorthelogic();
  void logicboarder();
  bool onoroff() {return crescent;}
  void indexnodes();
  bool index() { return indexthenodes; }
  void logicinfo();
};
#endif
