#include <iostream>
#include "Interp4Color.hh"
#include "AbstractComChannel.hh"
#include "Vector3D.hh"
#include <sstream>
#include <unistd.h>

using std::cout;
using std::endl;

extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "RGB"; }
}

AbstractInterp4Command* CreateCmd(void) {
  return Interp4Color::CreateCmd();
}

Interp4Color::Interp4Color() {
  _Name[0] = '\0';
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;
}

void Interp4Color::PrintCmd() const {
  cout << GetCmdName() << " " << _Name << " " << color << endl;
}

const char* Interp4Color::GetCmdName() const
{
  return "RGB";
}

bool Interp4Color::ExecCmd( 
    AbstractScene &rScn, 
    const char *sMobObjName,
	AbstractComChannel &rComChann
) {
    std::cout << "Komenda RGB dla: " << _Name << std::endl;
    std::shared_ptr<AbstractMobileObj> pObj = rScn.FindMobileObj(_Name);
    if(!pObj) {
        std::cerr << "Nie znaleziono obiektu " << _Name << std::endl;
        return false;
    }
    const int FPS = 40;
    int duration_s = 1;
    int steps = FPS * duration_s;  
    int delay = 1000 * 1000 / FPS;
    ExecAnim(pObj, rComChann, steps, delay);
    return true;
}

void Interp4Color::ExecAnim(
    std::shared_ptr<AbstractMobileObj> pObj,
    AbstractComChannel &rComChann,
    int steps, int delay
) const {
    Vector3D_int start_color = pObj->GetColor();
    double dRed = (double)(color[0] - start_color[0]) / steps;
    double dGreen = (double)(color[1] - start_color[1]) / steps;
    double dBlue = (double)(color[2] - start_color[2]) / steps;

    double currR = start_color[0];
    double currG = start_color[1];
    double currB = start_color[2];

    for(int i = 0; i < steps; ++i) {
        currR += dRed;
        currG += dGreen;
        currB += dBlue;

        Vector3D_int newCol;

        newCol[0] = currR;
        newCol[1] = currG;
        newCol[2] = currB;

        pObj->LockAccess();
          pObj->SetColor(newCol);
        pObj->UnlockAccess();

        std::ostringstream updateMess;
        updateMess << "UpdateObj Name=" << pObj->GetName() <<
        " RGB=" << pObj->GetColor() << "\n";
        rComChann.LockAccess();
          std::string sMsg = updateMess.str();
          rComChann.Send(sMsg.c_str());
        rComChann.UnlockAccess();
        usleep(delay);
    }
}

bool Interp4Color::ReadParams(std::istream& Strm_CmdsList) {
  if(!(Strm_CmdsList >> _Name >> color)) {
    std::cerr << "Blad podczas wczytywania parametrow dla RGB" << endl;
    Interp4Color::PrintSyntax();
    return false; 
  }
  return true;
}

AbstractInterp4Command* Interp4Color::CreateCmd() {
  return new Interp4Color();
}

void Interp4Color::PrintSyntax() const {
  cout << "   RGB  NazwaObiektu  R(0,255) G(0,255), B(0,255)" << endl;
}
