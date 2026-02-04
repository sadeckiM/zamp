#include "gsv-ServerCommand.hh"
#include <utility>
#include <memory>
#include "gsv-Scene.hh"
#include "gsv-Cuboid.hh"
#include "gtsDebug.hh"
#include "gsv-WgViewer3D.hh"
#include "gsv-Vector3-Utils.hh"
#include "gsv-WgViewer3D.hh"


using namespace std;







/*!
 *  Czyta z napisu, interpretuje i wykonuje polecenie.
 *  \param[in] rCmdText - zawiera treść polecenia przesłanego do serwera,
 *  \param[in,out] rScn - scena robocza, dla której ma zostać wykonane dane polecenie.
 *  \retval true - gdy wykonanie polecenia zakończyło się pomyślnie,
 *  \retval false - w przypadku przeciwnym.
 */
bool gsv::ServerCommand::ReadCmd_Exec(const std::string &rCmdText, Scene &rScn)
{
  istringstream  IStrm(rCmdText);

  if ((IStrm >> *this).fail()) return false;
  return ExecCmd(rScn);
}



/*!
 *  Wstawia do sceny nowy obiekt.
 *  \param[in,out] rScn - scena robocza, dla której ma zostać wykonane dane polecenie.
 *  \retval true - gdy wykonanie polecenia zakończyło się pomyślnie,
 *  \retval false - w przypadku przeciwnym.
 */
bool gsv::ServerCommand::InsertNewObj(Scene &rScn)
{
  shared_ptr<GraphObject>  PObj = make_shared<Cuboid>();
  PObj->UseParams().Update(_ObjParams);
  PObj->UseParams().AbsorbChanges();
  return rScn.InsertNoNameGraphObj_UseQualifiedName(GetQName(),PObj);
}




/*!
 *  Wyszukuje obiekt na scenie i aktualizuje jego parametry.
 *  Wyszukiwanie jest realizowane na podstawie pamiętanej nazwy obiektu.
 *  \param[in,out] rScn - scena robocza, dla której ma zostać wykonane dane polecenie.
 *  \retval true - gdy wykonanie polecenia zakończyło się pomyślnie,
 *  \retval false - w przypadku przeciwnym.
 */
bool gsv::ServerCommand::UpdateObj(Scene &rScn)
{
  GraphObject* pObj = rScn.FindNode(GetQName());
  if (pObj == nullptr) return false;
  pObj->UseParams().Update(_ObjParams);
  pObj->UseParams().AbsorbChanges();
  return true;
}


inline
void SetDrawMode(const std::string &rModeName)
{
  if (rModeName == "Fill") gsv::WgViewer3D::Use_Viewer()->SetDrawMode(gsv::DrawMode::Fill);
  else if (rModeName == "Line") gsv::WgViewer3D::Use_Viewer()->SetDrawMode(gsv::DrawMode::Line);
  else {
    cerr << " Nieznany parametr '"<< rModeName << "' polecenia: DrawMode" << endl
	 << "  Dopuszczalne wartosci to: Fill, Line." << endl;
  }
}


inline
void SetShowLocalCoords(const std::string &rModeName)
{
  if (rModeName == "yes") gsv::WgViewer3D::Use_Viewer()->SetShowLocalCrds(true);
  else if (rModeName == "no") gsv::WgViewer3D::Use_Viewer()->SetShowLocalCrds(false);
  else {
    cerr << " Nieznany parametr '"<< rModeName << "' polecenia: ShowLocalCoords" << endl
	 << "  Dopuszczalne wartosci to: no, yes." << endl;
  }
}



inline
void SetBgColor(const gsv::Vector3i &rColor)
{
#define  COLOR_NORM(IDX) (rColor[IDX]/(255.0f))
  gsv::Set(gsv::Use_Viewer()->UseBgColor(), COLOR_NORM(0), COLOR_NORM(1), COLOR_NORM(2));
}


inline
void SetBgImageFileName(const std::string &rName)
{
  gsv::WgViewer3D::Use_Viewer()->Check_SetBgImageFileName(rName.c_str());
}


/*!
 *  Wykonuje polecenie, którego parametry znajdują się w obiekcie, dla którego
 *  wywoływna jest ta metoda.
 *  Na czas wykonania polecenie, scena jest blokowana.
 *  \param[in,out] rScn - scena robocza, dla której ma zostać wykonane dane polecenie.
 *  \retval true - gdy wykonanie polecenia zakończyło się pomyślnie,
 *  \retval false - w przypadku przeciwnym.
 */
bool gsv::ServerCommand::ExecCmd(Scene &rScn)
{
  switch (GetCmdID()) {
    case CommandID::Nothing: return true;
    case CommandID::AddObj:
      return InsertNewObj(rScn);

    case CommandID::UpdateObj:
      return UpdateObj(rScn);

    case CommandID::Clear:
      rScn.ClearScene();
      return true;

    case CommandID::DrawMode:
      SetDrawMode(GetParamWord());
      break;

    case CommandID::ShowLocalCrds:
      SetShowLocalCoords(GetParamWord());
      break;

    case CommandID::BgColor:
      SetBgColor(GetColor());
      break;

    case CommandID::BgImage:
      SetBgImageFileName(GetParamWord());
      break;
      
    case CommandID::ShowBgTexture:
      break;
      
    case CommandID::Display:
    case CommandID::Close:
      break;      
  }
  return true;
}



/*!
 * \brief Wczytuje identyfikator polecenia ze strumienia
 */
std::istream &operator >> (std::istream &rIStrm, gsv::CommandID &rCmdID)
{
#define MAKE_PAIR( Name ) { #Name, gsv::CommandID::Name }
  using Pair4CmdID =  std::pair<std::string,gsv::CommandID>;
  Pair4CmdID  PairTab[] = { MAKE_PAIR( Nothing ),
			    MAKE_PAIR( AddObj ),
			    MAKE_PAIR( UpdateObj ),
			    MAKE_PAIR( Display ),
			    MAKE_PAIR( Close ),
			    MAKE_PAIR( Clear ),
			    MAKE_PAIR( DrawMode ),
			    MAKE_PAIR( ShowLocalCrds ),
			    MAKE_PAIR( ShowBgTexture ),
			    MAKE_PAIR( BgColor ),
			    MAKE_PAIR( BgImage )
                          };
    
  std::string  Keyword;

  if ((rIStrm >> Keyword).fail()) { return rIStrm; }

  for (const Pair4CmdID &rPair : PairTab ) {
    if (rPair.first == Keyword) { rCmdID = rPair.second; return rIStrm; }
  }
  CErr_1(" Unknown command keyword: " << Keyword);
  return rIStrm;
}



/*!
 * \brief Zapisuje identyfikator polecenia do strumienia
 */
std::ostream &operator << (std::ostream &rOStrm, gsv::CommandID CmdID)
{
  const char *IDsName[] = {
			   "Nothing",
                           "AddObj",
                           "UpdateObj",
			   "Display",
			   "Close",
			   "Clear",
			   "DrawMode",
			   "ShowLocalCrds",
			   "ShowBgTexture",
			   "BgColor"
			   "BgImage"
                         };
  return rOStrm << IDsName[static_cast<int>(CmdID)];
}


/*!
 * \brief Czyta wektor koloru zapisany w formie np. "RGB=(13,34,10)"
 */
bool ReadColor(istream &rIStrm, gsv::Vector3i &rColor)
{
  constexpr  int Size = 256;
  char       Line[Size], *pLine = Line ;
  
  rIStrm.getline(Line,Size,'=');
  for (; isspace(*pLine) && *pLine; ++pLine);
  if (strcmp("RGB",pLine)) return false;
  rIStrm >> rColor;
  return !rIStrm.fail();
}


/*
 * \brief Wczytuje nazwę parametru i jego wartość.
 *
 *  Wczytuje nazwę parametru i jego wartość.
 *  Powinna ona być ujęta w cudzysłowy zgodnie z przykładem poniżej.
   \verbatim
              File="/home/jk/img/starts.jpg"
   \endverbatim
    Ścieżka do pliku może być względna lub bezwzględna.
 */
bool ReadParam_FileName(istream &rIStrm)
{
  constexpr  int Size = 1000;
  char       Line[Size], *pLine = Line ;
  char       Ch;

  //--------------------------------------
  // Ekstrakcja nazwy parametru.
  //
  if (rIStrm.getline(Line,Size,'=').fail()) {
    CErr_1( " Oczekiwano parametry, a po niej znaku '='\n" <<
	    "        Brak znaku '='."
	    );
    return false;
  }
  for (; isspace(*pLine) && *pLine; ++pLine);
  if (strcmp("File",pLine)) {
    CErr_1(" Oczekiwano nazwe parametru:  \"File\"" << endl <<
           "        Zamiast tego wczytano nazwe: \"" << pLine << "\""
	   );
    return false;
  }
  //---------------------------------------
  // Ekstrakcja nazwy pliku.
  //
  Ch=' ';
  if (rIStrm >> Ch, Ch != '"') {
    CErr_1( " Nazwa pliku powinna byc w cudzyslowach. Brak cudzysłowu otwierajacego."
	  );
    return false;
  }
  if (rIStrm.getline(Line,Size,'"').fail()) {
    CErr_1( " Nazwa pliku powinna byc w cudzyslowach. Brak cudzysłowu zamykajacego."
	  );
    return false;    
  }
  return gsv::WgViewer3D::Use_Viewer()->Check_SetBgImageFileName(Line);
}



/*!
 * \brief Wczytuje polecenie ze strumienia
 */
std::istream &operator >> (std::istream &rIStrm, gsv::ServerCommand &rCmd)
{
  std::string  Keyword;
  std::string  Line;  
  gsv::CommandID  CmdID;

  
  if ((rIStrm >> CmdID).fail()) return rIStrm;
  rCmd.SetCmdID(CmdID);

  switch (CmdID) {
   case gsv::CommandID::Nothing:
      break;
      
    case gsv::CommandID::AddObj:
    case gsv::CommandID::UpdateObj:
      break;

    case gsv::CommandID::Display:
    case gsv::CommandID::Close:
    case gsv::CommandID::Clear:
      return rIStrm;

   case gsv::CommandID::ShowLocalCrds:
   case gsv::CommandID::ShowBgTexture:
   case gsv::CommandID::DrawMode:
      {
	std::string  &Word = Keyword;
	rIStrm >> Word;
	rCmd.SetParamWord(Word);	
      }
      return rIStrm; // Czyta jeden parametr będący pojedynczym słowem.

   case gsv::CommandID::BgColor:
     {
       gsv::Vector3i  Color;
       if (!ReadColor(rIStrm,Color)) {
          rIStrm.setstate(ios::failbit);
       } else {
  	  rCmd.SetColor(Color);
       }

     }
     return rIStrm;

   case gsv::CommandID::BgImage:
     if (!ReadParam_FileName(rIStrm)) {
       rIStrm.setstate(ios::failbit);
     }
     return rIStrm;
     
  }
  
  // if (CmdID != gsv::AddObj && CmdID != gsv::UpdateObj) return rIStrm;

  if (getline(rIStrm,Line,'=').fail()) return rIStrm;

  istringstream IStrm_tmp(Line);
  IStrm_tmp >> Keyword;
  if (Keyword != "Name") {
    CErr_1(" Unknown keyword: \"" << Keyword << "\"" << endl <<
	   " It was expected the word \"Name\"");
    return rIStrm;
  }

  std::string  Name;
  if ((rIStrm >> Name).fail()) return rIStrm;
  rCmd.SetQName(Name);
  return rIStrm >> rCmd.UseParams();
}




/*!
 * \brief Zapisuje polecenie do strumienia i parametry (o ile są przewidziane)
 */
std::ostream &operator << (std::ostream &rOStrm, const gsv::ServerCommand &rCmd)
{
  rOStrm << rCmd.GetCmdID();
  switch (rCmd.GetCmdID()) {
     case gsv::CommandID::Nothing:
     case gsv::CommandID::Close:
     case gsv::CommandID::Clear:
     case gsv::CommandID::Display: return rOStrm;
       
     case gsv::CommandID::AddObj:
     case gsv::CommandID::UpdateObj: break;

     case gsv::CommandID::DrawMode:
     case gsv::CommandID::ShowLocalCrds:
     case gsv::CommandID::ShowBgTexture:
       break;
       
     case gsv::CommandID::BgColor:
       break;

     case gsv::CommandID::BgImage:
       break;
       
     default: return rOStrm << "Unknown command ID!!!";
  }
  return rOStrm << " Name=" << rCmd.GetQName() << ' ' << rCmd.GetParams();
}




