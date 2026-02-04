#include "Set4LibInterfaces.hh"

bool Set4LibInterfaces::AddPlugin(const std::string& LibName) {
    std::shared_ptr<LibInterface> libInterface = std::make_shared<LibInterface>();

    if(!libInterface -> Init(LibName)) {
        std::cerr << "Nie udalo sie zaladowac wtyczki: " << LibName << std::endl;
        return false;
    }

    std::string commName = libInterface -> getCmdName();

    (*this)[commName] = libInterface;

    std::cout << "Udalo sie zaladowac wtyczke " << commName << std::endl;
    return true;
}

std::shared_ptr<LibInterface> Set4LibInterfaces::FindPlugin(const std::string& CmdName) {
    map_it it = find(CmdName);
    if (it != end())
        return it -> second;
    
    return nullptr;

}