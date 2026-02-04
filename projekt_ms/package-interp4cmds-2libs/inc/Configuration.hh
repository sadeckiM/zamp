#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include "Cuboid.hh"
#include <list>

class Configuration {
  std::list<Cuboid> obj;
  std::list<std::string> libs;
public:
    void AddLib(const std::string& LibName) {libs.push_back(LibName);}
    void AddObj(const Cuboid& cube) {obj.push_back(cube);}

    const std::list<std::string> & GetLibs() const {return libs;}
    const std::list<Cuboid> & GetObj() const {return obj;}
};


#endif
