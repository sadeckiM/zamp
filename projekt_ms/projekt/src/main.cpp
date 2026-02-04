#include <iostream>
#include "AbstractInterp4Command.hh"
#include "ProgramInterpreter.hh"
#include "xmlinterp.hh"
#include <iostream>
#include <list>
#include <sstream>

using namespace std;
using namespace xercesc;

int main(int argc, char *argv[])
{
  const char * fileName = "src/commands.cmds";
  
  ProgramInterpreter interp;
  Configuration Config;

  if (!interp.ReadXML("config/config.xml",Config)) {
     cerr << "Blad podczas parsowania pliku" << fileName << endl;
     return 1;
  }

  cout << "Parsowanie ukonczone." << endl;

  interp.LoadXMLConfig(Config);

  if (!interp.ExecProgram(fileName)) {
    cerr << "Cos poszlo nie tak z wykonaniem programu." << endl;
    return 1;
  }

  return 0;
}