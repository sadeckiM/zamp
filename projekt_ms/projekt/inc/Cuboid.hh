#ifndef __CUBE_HH__
#define __CUBE_HH__

#include "AbstractMobileObj.hh"
#include <string>
#include <mutex>

class Cuboid : public AbstractMobileObj {
    std::string name;
    Vector3D pos;
    Vector3D rot;
    Vector3D scale;
    Vector3D_int color;
    Vector3D shift;
    std::mutex mtx;
public:
    Cuboid() {}
    Cuboid(const Cuboid& other) {
        name = other.name;
        pos = other.pos;
        rot = other.rot;
        scale = other.scale;
        color = other.color;
        shift = other.shift;
    }
    virtual double GetAng_Roll_deg() const override {return rot[0];}
    virtual double GetAng_Pitch_deg() const override {return rot[1];}
    virtual double GetAng_Yaw_deg() const override {return rot[2];}
    virtual void SetAng_Roll_deg(double Ang_Roll_deg) override {rot[0] = Ang_Roll_deg;}
    virtual void SetAng_Pitch_deg(double Ang_Pitch_deg) override {rot[1] = Ang_Pitch_deg;}
    virtual void SetAng_Yaw_deg(double Ang_Yaw_deg) override {rot[2] = Ang_Yaw_deg;}
    virtual const Vector3D & GetPosition_m() const override {return pos;}
    virtual void SetPosition_m(const Vector3D &rPos) override {pos = rPos;}
    virtual void SetName(const char* sName) override {name = sName;}
    virtual const std::string & GetName() const override {return name;}

    virtual const Vector3D_int & GetColor() const override {return color;}
    virtual const Vector3D & GetScale() const override {return scale;}
    const Vector3D & GetShift() const {return shift;}
    const Vector3D & GetRotation() const override {return rot;}

    virtual void SetColor(const Vector3D_int & col) override {color = col;}
    virtual void SetScale(const Vector3D & sc) override {scale = sc;}
    void SetShift(const Vector3D & sh) {shift = sh;}

    virtual void LockAccess() override {mtx.lock(); }
    virtual void UnlockAccess() override {mtx.unlock(); }
};

#endif
