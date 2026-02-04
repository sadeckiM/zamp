#ifndef PROGRAMINTERPRETER_H__
#define PROGRAMINTERPRETER_H__

#include "Set4LibInterfaces.hh"
#include "Configuration.hh"
#include "Scene.hh"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include "xmlinterp.hh"
#include "ComChannel.hh"
#include <thread>

using namespace xercesc;

class ProgramInterpreter {
    bool expandMacros(const std::string& inputName, const std::string & outputName);
    Set4LibInterfaces _LibManager;
    Scene _Scene;
    ComChannel _Chann;
    std::list<std::thread> _Threads;
    std::list<AbstractInterp4Command *> _ParCmd;
    bool LoadPlugins(const char* FileName);
    bool XML_initialize(void);
    DefaultHandler* XML_configuration(SAX2XMLReader* pParser, Configuration &rConfig);
    bool XSD_config(SAX2XMLReader* pParser, const char* sFileName);
    void printExc(const SAXParseException& Exception);
    void LoadObjs(const Configuration & rConfig);
    bool SyncThreads(bool& isParallel);
    AbstractInterp4Command * PrepareCmds(std::ifstream& file, std::string word);
    bool ExecTasks(std::ifstream& file, std::string word, bool& isParallel);
public:
    bool ReadXML(const char* sFileName, Configuration &rConfig);
    bool ExecProgram(const char* FileName);
    void LoadXMLConfig(const Configuration & rConfig);
    ProgramInterpreter();
    ~ProgramInterpreter() {_Chann.Send("Close \n"); }
};

#endif
