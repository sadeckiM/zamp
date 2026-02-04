#ifndef LIBINTERFACE_H__
#define LIBINTERFACE_H__

#include <dlfcn.h>
#include <string>
#include "AbstractInterp4Command.hh"

class LibInterface {
    void * _LibHandler;
    std::string _CmdName;
    AbstractInterp4Command *(* _pCreateCmd)(void);
public:
    LibInterface() : _LibHandler(nullptr), _pCreateCmd(nullptr) {}
    ~LibInterface() {if(_LibHandler) dlclose(_LibHandler);}
    bool Init(const std::string& libName);
    const std::string& getCmdName() const {return _CmdName;}
    AbstractInterp4Command* CreateCmd() const {
        return _pCreateCmd ? _pCreateCmd() : nullptr;
    }
};

#endif
