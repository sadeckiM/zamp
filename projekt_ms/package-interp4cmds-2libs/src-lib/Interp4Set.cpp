#include <iostream>
#include "Interp4Set.hh"
#include <sstream>


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Set"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Set::CreateCmd();
}


/*!
 *
 */
Interp4Set::Interp4Set() : _x_pos(0), _y_pos(0), _z_pos(0), _x_rot(0), _y_rot(0), _z_rot(0)
{
  _Name[0] = '\0';
}


/*!
 *
 */
void Interp4Set::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() << " " << _Name << " " << _x_pos << " " << _y_pos;
  cout << " " << _z_pos << " " << _x_rot << " " << _y_rot << " " << _z_rot << endl;
}


/*!
 *
 */
const char* Interp4Set::GetCmdName() const
{
  return "Set";
}


/*!
 *
 */
bool Interp4Set::ExecCmd( AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann
			 )
{
  std::shared_ptr<AbstractMobileObj> pObj = rScn.FindMobileObj(_Name);

  if(!pObj) {
    std::cerr << "Nie znaleziono obiektu " << _Name << std::endl;
    return false;
  }

  Vector3D newPos;
  newPos[0] = _x_pos;
  newPos[1] = _y_pos;
  newPos[2] = _z_pos;

  pObj->LockAccess();
  pObj->SetPosition_m(newPos);
  pObj->SetAng_Roll_deg(_z_rot);
  pObj->SetAng_Pitch_deg(_y_rot);
  pObj->SetAng_Yaw_deg(_x_rot);
  pObj->UnlockAccess();
  std::ostringstream updateMess;
  updateMess << "UpdateObj Name=" << pObj->GetName() << " Trans_m="
             << pObj->GetPosition_m() << "\n";
  std::string sMsg = updateMess.str();
  rComChann.LockAccess();
  rComChann.Send(sMsg.c_str());
  rComChann.UnlockAccess();

  return true;
}


/*!
 *
 */
bool Interp4Set::ReadParams(std::istream& Strm_CmdsList)
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  if(!(Strm_CmdsList >> _Name >> _x_pos >> _y_pos >> _z_pos >> _x_rot >> _y_rot >> _z_rot)) {
    std::cerr << "Blad podczas wczytywania parametrow dla Set" << endl;
    Interp4Set::PrintSyntax();
    return false; 
  }
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Set::CreateCmd()
{
  return new Interp4Set();
}


/*!
 *
 */
void Interp4Set::PrintSyntax() const
{
  cout << "   Set  NazwaObiektu  WspX WspY WspZ KatOX KatOY KatOZ" << endl;
}
