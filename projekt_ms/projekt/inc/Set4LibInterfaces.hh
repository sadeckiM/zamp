#ifndef SET4LIBINTERFACES_H__
#define SET4LIBINTERFACES_H__

#include <map>
#include <memory>
#include "LibInterface.hh"

typedef std::map<std::string, std::shared_ptr<LibInterface>>::iterator map_it;

class Set4LibInterfaces : std::map<std::string, std::shared_ptr<LibInterface>> {
public:
    bool AddPlugin(const std::string& LibName);
    std::shared_ptr<LibInterface> FindPlugin(const std::string& CmdName);
};

#endif