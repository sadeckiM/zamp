#include "xmlinterp.hh"
#include <sstream>
#include <xercesc/sax2/Attributes.hpp>

void XMLInterp4Config::startDocument() {
    std::cout << "Rozpoczecie intepretacji dokumentu XML." << std::endl;
}
void XMLInterp4Config::endDocument() {
    std::cout << "Zakonczenie interpretacji dokumentu XML." << std::endl << std::endl;
}
void XMLInterp4Config::startElement(const XMLCh *const pURI, 
                                    const XMLCh *const pLocalName, 
                                    const XMLCh *const pQNname,
                                    const xercesc::Attributes& rAttrs) {
    char* elem = xercesc::XMLString::transcode(pLocalName);
    std::cout << "Poczatek elementu: " << elem << std::endl;
    WhenStartElement(elem, rAttrs);
    xercesc::XMLString::release(&elem);
}

void XMLInterp4Config::endElement(const XMLCh *const pURI, 
                                  const XMLCh *const pLocalName, 
                                  const XMLCh *const pQNname) {
    char* uri = xercesc::XMLString::transcode(pURI);
    char * elem = xercesc::XMLString::transcode(pLocalName);
    char * qname = xercesc::XMLString::transcode(pQNname);

    std::cout << "URI: " << uri << std::endl;
    std::cout << "QName: " << qname << std::endl;
    std::cout << "KONIEC ELEMENTU: " << elem << std::endl;

    xercesc::XMLString::release(&elem);
    xercesc::XMLString::release(&uri);
    xercesc::XMLString::release(&qname);
}

void XMLInterp4Config::fatalError(const xercesc::SAXParseException& rExc) {

    char* msg = xercesc::XMLString::transcode(rExc.getMessage());
    char* sid = xercesc::XMLString::transcode(rExc.getSystemId());

    std::cerr << "FATAL ERROR" << std::endl;
    std::cerr << "Plik: " << sid << std::endl;
    std::cerr << "Linia: " << rExc.getLineNumber() << std::endl;
    std::cerr << "Info: " << msg << std::endl;

    xercesc::XMLString::release(&msg);
    xercesc::XMLString::release(&sid);

    throw rExc;
}

void XMLInterp4Config::error(const xercesc::SAXParseException & rExc) {
    char* msg = xercesc::XMLString::transcode(rExc.getMessage());
    char* sid = xercesc::XMLString::transcode(rExc.getSystemId());

    std::cerr << "ERROR" << std::endl;
    std::cerr << "Plik: " << sid << std::endl;
    std::cerr << "Linia: " << rExc.getLineNumber() << std::endl;
    std::cerr << "Info: " << msg << std::endl;

    xercesc::XMLString::release(&msg);
    xercesc::XMLString::release(&sid);

    throw rExc;
}

void XMLInterp4Config::warning(const xercesc::SAXParseException & rExc) {
    char* msg = xercesc::XMLString::transcode(rExc.getMessage());
    char* sid = xercesc::XMLString::transcode(rExc.getSystemId());

    std::cerr << "Warning" << std::endl;
    std::cerr << "Plik: " << sid << std::endl;
    std::cerr << "Linia: " << rExc.getLineNumber() << std::endl;
    std::cerr << "Info: " << msg << std::endl;

    xercesc::XMLString::release(&msg);
    xercesc::XMLString::release(&sid);
}


void XMLInterp4Config::WhenStartElement(const std::string& rElemName,
                                        const xercesc::Attributes& rAttrs) {
    if(rElemName == "Lib") {
        ProcessLibAttrs(rAttrs);
        return;
    }

    if(rElemName == "Cube") {
        ProcessCubeAttrs(rAttrs);
        return;
    }
}

void XMLInterp4Config::ProcessLibAttrs(const xercesc::Attributes& rAttrs) {
    if(rAttrs.getLength() != 1) {
        std::cerr << "Niepoprawna liczba atrybutow dla Lib" << std::endl;
        exit(1);
    }

    char* name = xercesc::XMLString::transcode(rAttrs.getQName(0));

    if(strcmp(name, "Name")) {
        std::cerr << "Zla nazwa atrybutu dla Lib" << std::endl;
        exit(1);
    }

    XMLSize_t idx = 0;
    char* libname = xercesc::XMLString::transcode(rAttrs.getValue(idx));

    std::cout << "Nazwa biblioteki: " << libname << std::endl;

    rConf.AddLib(libname);

    xercesc::XMLString::release(&name);
    xercesc::XMLString::release(&libname);
}

Vector3D XMLInterp4Config::str_to_Vector3D(const std::string& s) {
    if(s.empty()) {
        std::cerr << "Nieprawidlowa konwersja string na Vector3D. Wymagany string, np. \"0.0 0.0 0.0\"" << std::endl;
        exit(1);
    }

    std::istringstream iss(s);
    Vector3D vec;
    iss >> vec[0];
    iss >> vec[1];
    iss >> vec[2];

    if(iss.fail()) {
        std::cerr << "Nieprawidlowe przypisanie stream na Vector3D." << std::endl;
        exit(1);
    }

    return vec;
}

Vector3D_int XMLInterp4Config::str_to_Vector3D_int(const std::string& s) {
    if(s.empty()) {
        std::cerr << "Nieprawidlowa konwersja string na Vector3D_int. Wymagany string, np. \"0 0 0\"" << std::endl;
        exit(1);
    }

    std::istringstream iss(s);
    Vector3D_int vec;
    iss >> vec[0];
    iss >> vec[1];
    iss >> vec[2];

    if(iss.fail()) {
        std::cerr << "Nieprawidlowe przypisanie stream na Vector3D_int." << std::endl;
        exit(1);
    }

    return vec;
}

Vector3D XMLInterp4Config::GetVectorAttr(const xercesc::Attributes& rAttrs, 
                       const std::string& AttrName, 
                       const Vector3D& DefVal) {
    std::string sVal = GetAttr(rAttrs, AttrName);

    if(sVal.empty())
        return DefVal;
    return str_to_Vector3D(sVal);
}

Vector3D_int XMLInterp4Config::GetVectorIntAttr(const xercesc::Attributes& rAttrs, 
                       const std::string& AttrName, 
                       const Vector3D_int& DefVal) {
    std::string sVal = GetAttr(rAttrs, AttrName);

    if(sVal.empty())
        return DefVal;
    return str_to_Vector3D_int(sVal);
}


std::string XMLInterp4Config::GetAttr(const xercesc::Attributes &rAttrs, const std::string & AttrName) {
    XMLCh* xml_name = xercesc::XMLString::transcode(AttrName.c_str());
    const XMLCh* xml_val = rAttrs.getValue(xml_name);

    xercesc::XMLString::release(&xml_name);

    if(xml_val == nullptr) {
        return "";
    }

    char* val = xercesc::XMLString::transcode(xml_val);
    std::string sval = val;

    xercesc::XMLString::release(&val);
    return sval;
}

void XMLInterp4Config::ProcessCubeAttrs(const xercesc::Attributes& rAttrs) {
    if(rAttrs.getLength() < 1) {
        std::cerr << "Niepoprawna liczba liczba argumentow dla Cube." << std::endl;
        exit(1);
    }

    Cuboid cube;
    Vector3D vecDef;
    Vector3D scaleDef;
    scaleDef[0] = 1; scaleDef[1] = 1; scaleDef[2] = 1;
    Vector3D_int colorDef;
    
    std::string name = GetAttr(rAttrs, "Name");
    if(name.empty()) {
        std::cerr << "Brak nazwy obiektu." << std::endl;
        exit(1);
    }
    cube.SetName(name.c_str());

    cube.SetPosition_m(GetVectorAttr(rAttrs, "Trans_m", vecDef));

    Vector3D rot_vec = GetVectorAttr(rAttrs, "RotXYZ_deg", vecDef);
    cube.SetAng_Roll_deg(rot_vec[0]);
    cube.SetAng_Pitch_deg(rot_vec[1]);
    cube.SetAng_Yaw_deg(rot_vec[2]);

    cube.SetScale(GetVectorAttr(rAttrs, "Scale", scaleDef));

    cube.SetShift(GetVectorAttr(rAttrs, "Shift", vecDef));

    cube.SetColor(GetVectorIntAttr(rAttrs, "RGB", colorDef));

    std::cout << "[DEBUG]: " << GetVectorIntAttr(rAttrs, "RGB", colorDef) << std::endl;
    std::cout << "[DEBUG2]: " << GetVectorAttr(rAttrs, "Trans_m", vecDef) << std::endl;

    std::cout << "Wczytano Cube: " << cube.GetName() << std::endl;

    rConf.AddObj(cube);
}
