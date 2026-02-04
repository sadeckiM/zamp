#include <iostream>
#include "Interp4Move.hh"
#include <sstream>
#include <unistd.h>

using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Move"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Move::CreateCmd();
}


/*!
 *
 */
Interp4Move::Interp4Move(): _Distance(0), _Speed_mS(0)
{
 _Name[0] = '\0';
}


/*!
 *
 */
void Interp4Move::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() << " " << _Name << " " << _Speed_mS << " " << _Distance << endl;
}


/*!
 *
 */
const char* Interp4Move::GetCmdName() const
{
  return "Move";
}


/*!
 *
 */
bool Interp4Move::ExecCmd( AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann
			 )
{
  std::cout << "Komenda Move dla: " << _Name << std::endl;
  std::shared_ptr<AbstractMobileObj> pObj = rScn.FindMobileObj(_Name);

  if(!pObj) {
    std::cerr << "Nie znaleziono obiektu " << _Name << std::endl;
    return false;
  }

  const int FPS = 40;
  int dir = 1;

  if(_Speed_mS < 0)
    dir = -1;

  double dist = _Distance;
  if(_Speed_mS == 0) return true;
  double moveTime = std::abs(_Distance / _Speed_mS);

  int steps = static_cast<int>(moveTime * FPS);
  if(steps == 0) steps = 1;

  int delay = 1000*1000/FPS;

  double disp_step = dir * (dist / steps);

  ExecAnim(pObj, rComChann, disp_step, steps, delay);

  return true;
}

void Interp4Move::ExecAnim(std::shared_ptr<AbstractMobileObj> pObj,
			   AbstractComChannel &rComChann, double disp_step, 
         int steps, int delay) const {

    for(int i = 1; i <= steps; ++i) {
        pObj->LockAccess();
          double roll_rad  = pObj->GetAng_Roll_deg()  * M_PI / 180.0;
          double pitch_rad = pObj->GetAng_Pitch_deg() * M_PI / 180.0;
          double yaw_rad   = pObj->GetAng_Yaw_deg()   * M_PI / 180.0;
          Vector3D currPos = pObj->GetPosition_m();
          double dx = disp_step * cos(pitch_rad) * cos(yaw_rad);
          double dy = disp_step * (cos(roll_rad)*sin(yaw_rad) + cos(yaw_rad)*sin(pitch_rad)*sin(roll_rad));
          double dz = disp_step * (sin(roll_rad)*sin(yaw_rad) - cos(roll_rad)*cos(yaw_rad)*sin(pitch_rad));
          currPos[0] += dx;
          currPos[1] += dy;
          currPos[2] += dz;
          pObj->SetPosition_m(currPos);
        pObj->UnlockAccess();
    std::ostringstream updateMess;
    updateMess << "UpdateObj Name=" << pObj->GetName() << " Trans_m="
             << pObj->GetPosition_m() << "\n";
    rComChann.LockAccess();
      std::string sMsg = updateMess.str(); 
      rComChann.Send(sMsg.c_str());
    rComChann.UnlockAccess();
    usleep(delay);
  }
}


/*!
 *
 */
bool Interp4Move::ReadParams(std::istream& Strm_CmdsList)
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  if(!(Strm_CmdsList >> _Name >> _Speed_mS >> _Distance)) {
    std::cerr << "Blad podczas wczytywania parametrow dla Move" << endl;
    Interp4Move::PrintSyntax();
    return false; 
  }
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Move::CreateCmd()
{
  return new Interp4Move();
}


/*!
 *
 */
void Interp4Move::PrintSyntax() const
{
  cout << "   Move  NazwaObiektu  Szybkosc[m/s]  DlugoscDrogi[m]" << endl;
}
