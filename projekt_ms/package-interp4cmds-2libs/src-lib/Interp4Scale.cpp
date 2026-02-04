#include <iostream>
#include "Interp4Scale.hh"
#include <sstream>
#include <unistd.h>

using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Scale"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Scale::CreateCmd();
}


/*!
 *
 */
Interp4Scale::Interp4Scale()
{
  _Name[0] = '\0';
  scale[0] = 1;
  scale[1] = 1;
  scale[2] = 1;
  _duration_s = 2;
}


/*!
 *
 */
void Interp4Scale::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() << " " << _Name << " " << scale << endl;
}


/*!
 *
 */
const char* Interp4Scale::GetCmdName() const
{
  return "Scale";
}


/*!
 *
 */
bool Interp4Scale::ExecCmd( AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann
			 )
{
  std::cout << "Komenda Scale dla: " << _Name << std::endl;
  std::shared_ptr<AbstractMobileObj> pObj = rScn.FindMobileObj(_Name);

  if(!pObj) {
    std::cerr << "Nie znaleziono obiektu " << _Name << std::endl;
    return false;
  }

  const int FPS = 40;

  int steps = static_cast<int>(_duration_s * FPS);
  
  if(steps == 0) steps = 1;

  int delay = 1000*1000/FPS;
  Vector3D cur_scale = pObj->GetScale();
  ExecAnim(pObj, rComChann, cur_scale, steps, delay);

  return true;
}

void Interp4Scale::ExecAnim(std::shared_ptr<AbstractMobileObj> pObj,
			   AbstractComChannel &rComChann, Vector3D &startScale, 
         int steps, int delay) const {

  Vector3D diff = scale - startScale;
  Vector3D dS;
  dS[0] = diff[0] / steps;
  dS[1] = diff[1] / steps;
  dS[2] = diff[2] / steps;

  Vector3D currS = startScale;

  for(int i = 1; i <= steps; ++i) {
    currS[0] = startScale[0] + (dS[0] * i);
    currS[1] = startScale[1] + (dS[1] * i);
    currS[2] = startScale[2] + (dS[2] * i);
    pObj->LockAccess();
      pObj->SetScale(currS);
    pObj->UnlockAccess();
    std::ostringstream updateMess;
    updateMess << "UpdateObj Name=" << pObj->GetName() << " Scale="
             << pObj->GetScale() << "\n";
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
bool Interp4Scale::ReadParams(std::istream& Strm_CmdsList)
{
  /*
   *  Tu trzeba napisać odpowiedni kod.
   */
  if(!(Strm_CmdsList >> _Name >> scale)) {
    std::cerr << "Blad podczas wczytywania parametrow dla Scale" << endl;
    Interp4Scale::PrintSyntax();
    return false; 
  }
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Scale::CreateCmd()
{
  return new Interp4Scale();
}


/*!
 *
 */
void Interp4Scale::PrintSyntax() const
{
  cout << "   Scale  NazwaObiektu  Skala_X Skala_Y Skala_Z" << endl;
}
