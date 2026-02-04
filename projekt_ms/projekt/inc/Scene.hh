#ifndef __SCENE_HH__
#define __SCENE_HH__

#include "AbstractScene.hh"
#include "AbstractMobileObj.hh"

#include <map>
#include <memory>

typedef std::map<std::string, std::shared_ptr<AbstractMobileObj>>::iterator scene_it;

class Scene : public AbstractScene {
    std::map<std::string, std::shared_ptr<AbstractMobileObj>> Set_MobileObjs;
public:
    virtual std::shared_ptr<AbstractMobileObj> FindMobileObj(const char *sName) override;
    virtual void AddMobileObj(std::shared_ptr<AbstractMobileObj> pMobObj) override;
};

#endif