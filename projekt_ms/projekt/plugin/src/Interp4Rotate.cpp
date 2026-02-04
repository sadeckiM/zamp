#include <iostream>
#include "Interp4Rotate.hh"
#include <sstream>
#include <unistd.h>

using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Rotate"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Rotate::CreateCmd();
}


/*!
 *
 */
Interp4Rotate::Interp4Rotate(): _Angle(0), _AngSpeed_mmS(0)
{
  _Name[0] = ' ';
  _Axis[0] = ' ';
}


/*!
 *
 */
void Interp4Rotate::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() << " " << _Name << " " << _Axis << " " << _AngSpeed_mmS << " " << _Angle << endl;
}


/*!
 *
 */
const char* Interp4Rotate::GetCmdName() const
{
  return "Rotate";
}


/*!
 *
 */
bool Interp4Rotate::ExecCmd( AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann
			 )
{
  std::shared_ptr<AbstractMobileObj> pObj = rScn.FindMobileObj(_Name);

  if(!pObj) {
    std::cerr << "Nie znaleziono obiektu " << _Name << std::endl;
    return false;
  }

  const int steps = 50;
  double dstep = _Angle / steps;
  double delay = ((_Angle / _AngSpeed_mmS)*1000*1000) / steps;

  double curr_ang_ox = pObj->GetAng_Pitch_deg();
  double curr_ang_oy = pObj->GetAng_Roll_deg();
  double curr_ang_oz = pObj->GetAng_Yaw_deg();

  for(int i = 0; i < steps; ++i) {
    pObj->LockAccess();
    std::string sAxis(_Axis);
    if(sAxis == "OX") {
      curr_ang_ox += dstep;
      pObj->SetAng_Pitch_deg(curr_ang_ox);
    }
    else if(sAxis == "OY") {
      curr_ang_oy += dstep;
      pObj->SetAng_Roll_deg(curr_ang_oy);
  }
    else if(sAxis == "OZ") {
      curr_ang_oz += dstep;
      pObj->SetAng_Yaw_deg(curr_ang_oz);
    }
    else {
    std::cerr << "Bledna os." << _Axis << std::endl;
    pObj->UnlockAccess();
    return false;
    }

    pObj->UnlockAccess();
    performRotate(rComChann, delay, pObj);
  }

  return true;
}

void Interp4Rotate::performRotate(AbstractComChannel &rComChann, double delay, 
                  std::shared_ptr<AbstractMobileObj> pObj) {

  Vector3D rot = pObj->GetRotation();

  std::ostringstream updateMess;
  updateMess << "UpdateObj Name=" << pObj->GetName() << " RotXYZ_deg="
          << rot << "\n";
  std::string sMsg = updateMess.str();
  rComChann.LockAccess();
  rComChann.Send(sMsg.c_str());
  rComChann.UnlockAccess();
  usleep(delay);
}


/*!
 *
 */
bool Interp4Rotate::ReadParams(std::istream& Strm_CmdsList)
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  if(!(Strm_CmdsList >> _Name >> _Axis >> _AngSpeed_mmS >> _Angle)) {
    std::cerr << "Blad podczas wczytywania parametrow dla Rotate" << endl;
    Interp4Rotate::PrintSyntax();
    return false; 
  }
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Rotate::CreateCmd()
{
  return new Interp4Rotate();
}


/*!
 *
 */
void Interp4Rotate::PrintSyntax() const
{
  cout << "   Rotate  NazwaObiektu  NazwaOsi  SzybkoscKatowa[deg/s] Kat[deg]" << endl;
}
