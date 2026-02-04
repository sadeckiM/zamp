#include "Scene.hh"

std::shared_ptr<AbstractMobileObj> Scene::FindMobileObj(const char *sName) {
    scene_it it = Set_MobileObjs.find(sName);
    if(it != Set_MobileObjs.end())
        return it->second;
    else
        return nullptr;
}

void Scene::AddMobileObj(std::shared_ptr<AbstractMobileObj> pMobObj) {
    if(!pMobObj) {
        std::cerr << "Wskaznik obiektu jest pusty." << std::endl;
        return;
    }

    std::string name = pMobObj->GetName();
    Set_MobileObjs[name] = pMobObj;
}