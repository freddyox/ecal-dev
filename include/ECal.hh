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
  int countnodes;

  std::set<int> cellset;
  std::set<int>::iterator cellit;
  std::map<int,sf::RectangleShape> modmap;
  std::map<int,sf::RectangleShape>::iterator mapit;

  // MODULE Properties
  sf::Vector2f s42, s40, s38;
  sf::RectangleShape module42, module40, module38;
  sf::RectangleShape boarder;

  std::vector<sf::RectangleShape> modules;
  std::vector<sf::RectangleShape>::iterator modit;

  // NODE Properties
  float nodeR;
  int increment;
  sf::CircleShape node;
  std::vector<sf::CircleShape> nodes,logic;
  std::vector<sf::CircleShape>::iterator nodit;

public:
  ECal(float,float);
  ~ECal() {};

  void draw(sf::RenderTarget&, sf::RenderStates) const;
  void initializeECal();
  void specs();
  void triggerlogic();

};
#endif
