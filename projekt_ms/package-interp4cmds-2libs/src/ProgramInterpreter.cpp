#include "ProgramInterpreter.hh"
#include "Set4LibInterfaces.hh"
#include <fstream>
#include <sstream>
#include <iostream>

ProgramInterpreter::ProgramInterpreter() {
    _Chann.OpenConnection();
    _Chann.Send("Clear \n");
}

bool ProgramInterpreter::expandMacros(const std::string& inputName, const std::string & outputName) {
    
    std::string cmd = "g++ -E -P -x c++ " + inputName + " -o " + outputName;

    if(system(cmd.c_str()) != 0) {
        std::cerr << "Blad rozwijania makra przez preprocesor." << std::endl;
        exit(1);
    }

    return true;
}

bool ProgramInterpreter::SyncThreads(bool &isParallel) {
    if(!isParallel) {
        std::cerr << "Nie mozna zakonczyc nierozpoczetej akcji." << std::endl;
        return false;
    }

    for(std::thread &t : _Threads) {
        if(t.joinable())
            t.join();
    }
    _Threads.clear();

    for(AbstractInterp4Command * cmd : _ParCmd)
        delete cmd;
    _ParCmd.clear();
    isParallel = false;

    std::cout << "Koniec bloku rownoleglego." << std::endl;
    return true;
}

AbstractInterp4Command * ProgramInterpreter::PrepareCmds(std::ifstream &file, std::string word) {
    std::shared_ptr<LibInterface> plugin = _LibManager.FindPlugin(word);

    if(!plugin) {
        std::cerr << "Nieprawidlowa wtyczka: " << word << std::endl;
        return nullptr;
    }

    AbstractInterp4Command * pCmd = plugin->CreateCmd();

    if(!pCmd->ReadParams(file)) {
        std::cerr << "Blad wczytywania parametrow komendy: " << word << std::endl;
        delete pCmd;
        return nullptr;
    }

    std::cout << "Wczytano: ";
    pCmd->PrintCmd();
    std::cout << std::endl;
    return pCmd;
}

bool ProgramInterpreter::ExecTasks(std::ifstream& file, std::string word, bool& isParallel) {
    while(file >> word) {
        if(word == "Begin_Parallel_Actions") {
            isParallel = true;
            std::cout << "Blok rownolegly" << std::endl;
            continue;
        }

        if(word == "End_Parallel_Actions") {
            if(!SyncThreads(isParallel))
                return false;
            continue;
        }

        AbstractInterp4Command *pCmd = PrepareCmds(file, word);

        if(pCmd == nullptr) {
            std::cerr << "Blad wczytywania komendy." << std::endl;
            return false;
        }

        if(isParallel) {
            _ParCmd.push_back(pCmd);
            _Threads.emplace_back(&AbstractInterp4Command::ExecCmd,
                                    pCmd, std::ref(_Scene), pCmd->GetObjName(),
                                     std::ref(_Chann));
        } else {
            try {
            if (!pCmd->ExecCmd(_Scene, nullptr, _Chann)) {
                std::cerr << "Blad wykonywania komendy: " << word << std::endl;
                delete pCmd;
                return false;
            }
            
            delete pCmd;
            }
            catch (const std::exception& e) {
                std::cerr << "Zlapano wyjatek w ExecTasks: " << e.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Zlapano nieznany wyjatek w ExecTasks!" << std::endl;
            }
        }
    }
    return true;
}

bool ProgramInterpreter::LoadPlugins(const char * FileName) {
    std::ifstream file(FileName);

    if(!file.is_open()) {
        std::cerr << "Blad otwarcia pliku dla makr." << FileName << std::endl;
        exit(1);
    }

    std::string word;
    bool isParallel = false;

    if(!ExecTasks(file, word, isParallel)) {
        std::cerr << "Blad w wykonywaniu zadan w watkach." << std::endl;
        return false;
    }
    return true;
}

bool ProgramInterpreter::ExecProgram(const char* FileName) {
    const char* TMP_file = "temp_commands.txt";

    if(!expandMacros(FileName, TMP_file))
        return false;
    
    if(!LoadPlugins(TMP_file))
        return false;
    
    remove(TMP_file);
    return true;
}

bool ProgramInterpreter::XML_initialize(void) {
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        std::cerr << "Error during initialization! :\n";
        std::cerr << "Exception message is: \n"
            << message << "\n";
        XMLString::release(&message);
        return false;
    }
    return true; 
}

DefaultHandler* ProgramInterpreter::XML_configuration(SAX2XMLReader* pParser, Configuration &rConfig) {
    pParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
    pParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    pParser->setFeature(XMLUni::fgXercesDynamic, false);
    pParser->setFeature(XMLUni::fgXercesSchema, true);
    pParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);

    pParser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

    DefaultHandler* pHandler = new XMLInterp4Config(rConfig);
    pParser->setContentHandler(pHandler);
    pParser->setErrorHandler(pHandler);
    return pHandler;
}

bool ProgramInterpreter::XSD_config(SAX2XMLReader* pParser, const char* sFileName) {
    try {
     
        if (!pParser->loadGrammar("config/config.xsd",
                                xercesc::Grammar::SchemaGrammarType,true)) {
        std::cerr << "!!! Plik grammar/actions.xsd, '" << std::endl
                << "!!! ktory zawiera opis gramatyki, nie moze zostac wczytany."
                << std::endl;
        return false;
        }
        pParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse,true);
        pParser->parse(sFileName);
   }
   catch (const XMLException& Exception) {
        char* sMessage = XMLString::transcode(Exception.getMessage());
        std::cerr << "Informacja o wyjatku: \n"
                << "   " << sMessage << "\n";
        XMLString::release(&sMessage);
        return false;
   }
   catch (const SAXParseException& Exception) {
        printExc(Exception);
        return false;
   }
   catch (...) {
        std::cout << "Zgloszony zostal nieoczekiwany wyjatek!\n" ;
        return false;
   }
   return true;
}

void ProgramInterpreter::printExc(const SAXParseException& Exception) {
    char* sMessage = XMLString::transcode(Exception.getMessage());
    char* sSystemId = xercesc::XMLString::transcode(Exception.getSystemId());

    std::cerr << "Blad! " << std::endl
        << "    Plik:  " << sSystemId << std::endl
        << "   Linia: " << Exception.getLineNumber() << std::endl
        << " Kolumna: " << Exception.getColumnNumber() << std::endl
        << " Informacja: " << sMessage 
        << std::endl;

        XMLString::release(&sMessage);
        XMLString::release(&sSystemId);
}

bool ProgramInterpreter::ReadXML(const char* sFileName, Configuration &rConfig) {
    if(!XML_initialize())
        return false;

    SAX2XMLReader* pParser = XMLReaderFactory::createXMLReader();
    DefaultHandler* pHandler = XML_configuration(pParser, rConfig);
    if(!XSD_config(pParser, sFileName))
        return false;
    
    delete pParser;
    delete pHandler;
    return true;
}

void ProgramInterpreter::LoadXMLConfig(const Configuration & rConfig) {
    std::cout << "Wtyczki do zaladowania:" << std::endl;
    //ladowanie wtyczek
    for(std::string lib : rConfig.GetLibs())
        std::cout << lib << std::endl;
    std::cout << std::endl;
    //wyswietlanie wtyczek zaladowanych/niezaladowanych
    for(const std::string& lib : rConfig.GetLibs()) {
        if(!_LibManager.AddPlugin(lib))
            std::cerr << "Blad ladowania wtyczki: " << lib << std::endl;
        else
            std::cout << "Zaladowano wtyczke: " << lib << std::endl;
    }
    std::cout << std::endl;
    LoadObjs(rConfig);
}

void ProgramInterpreter::LoadObjs(const Configuration & rConfig) {

    std::cout << "Dodano obiekty: " << std::endl;
    for(const Cuboid & cube : rConfig.GetObj()) {
        _Scene.AddMobileObj(std::make_shared<Cuboid>(cube));
         std::ostringstream updateMess;
        updateMess << "AddObj " << "Name=" << cube.GetName() << " RotXYZ_deg=" << cube.GetRotation() <<
        " Scale=" << cube.GetScale() << " Shift=" << cube.GetShift() <<
        " Trans_m=" << cube.GetPosition_m() << " RGB=" << cube.GetColor() <<
        std::endl;

        std::string msg = updateMess.str();
        std::cout << msg;

        _Chann.Send(msg.c_str());
    }
}
