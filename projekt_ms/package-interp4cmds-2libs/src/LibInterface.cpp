#include "LibInterface.hh"


bool LibInterface::Init(const std::string& libName) {
    std::string fullName = libName;

    _LibHandler = dlopen(fullName.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if(!_LibHandler) {
        std::cerr << "Brak biblioteki: " << fullName << std::endl;
        return false;
    }

    void * ptrFun = dlsym(_LibHandler, "CreateCmd");
    if(!ptrFun) {
        std::cerr << "Nie znaleziono funkcji CreateCmd w " << fullName << std::endl;
        return false;
    }

    _pCreateCmd = reinterpret_cast<AbstractInterp4Command* (*)(void)>(ptrFun);

    ptrFun = dlsym(_LibHandler, "GetCmdName");

    if(!ptrFun) {
        std::cerr << "Nie znaleziono funkcji GetCmdName w " << fullName << std::endl;
        return false;
    }

    const char * (*pGetCmdName)(void) = reinterpret_cast<const char * (*)(void)>(ptrFun);
    _CmdName = pGetCmdName();

    return true;
}
