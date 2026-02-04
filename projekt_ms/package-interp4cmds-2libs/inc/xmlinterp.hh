#ifndef XMLINTERP4ACTIONS_HH
#define XMLINTERP4ACTIONS_HH

#include <string>
#include <xercesc/util/XMLString.hpp>
//#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/Locator.hpp>

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

//XERCES_CPP_NAMESPACE_USE

#include "Configuration.hh"


/*!
 * \brief Implementuje reakcje na napotkane elementu opisu akcji
 *
 * Klasa zawiera zestaw metod, które wywoływane są w trakcie czytania i analizy
 * pliku XML.
 */
class XMLInterp4Config : public xercesc::DefaultHandler {
  public:
   /*!
    * \brief Inicjalizuje obiekt i kojarzy go z listą poleceń robota
    */
  XMLInterp4Config(Configuration &rConfig) : rConf(rConfig) {}

   /*!
    * \brief Wywoływana jest na początku dokumentu
    */
    virtual void startDocument() override;
   /*!
    * \brief Wywoływana jest na końcu dokumentu
    */
    virtual void endDocument() override;

   /*!
    * \brief Wywoływana jest po napotkaniu nowego elementu XML
    */
    virtual void startElement(
                    const XMLCh *const               pURI, 
                    const XMLCh *const               pLocalName, 
                    const XMLCh *const               pQNname,
                    const   xercesc::Attributes&     rAttrs
                  ) override;

   /*!
    * \brief Wywoływana jest po dojściu do końca elementu XML
    */
    virtual  void endElement(
                    const XMLCh *const pURI, 
                    const XMLCh *const pLocalName, 
                    const XMLCh *const pQNname
                   ) override;
   /*!
    * \brief Wywoływana jest gdy napotkany zostanie błąd fatalny
    */
    virtual void fatalError(const xercesc::SAXParseException&) override;
   /*!
    * \brief Wywoływana jest gdy napotkany zostanie błąd
    */
    virtual void error(const xercesc::SAXParseException &) override;
   /*!
    * \brief Wywoływana jest gdy parser sygnalizuje ostrzeżenie
    */
    virtual void warning(const xercesc::SAXParseException &) override;


    /*!
     * \brief Wykonuje operacje związane z danym elementem XML
     */
    void WhenStartElement( const std::string&             rElemName,
		           const xercesc::Attributes&     rAttrs
                         );
    /*!
     * \brief Analizuje atrybuty i  odpwiednio je interpretuje
     */
    void ProcessLibAttrs(const xercesc::Attributes&   rAttrs);
    /*!
     * \brief Analizuje atrybuty i odpwiednio je interpretuje
     */
    void ProcessCubeAttrs(const xercesc::Attributes&   rAttrs); 
  private:
  Configuration &rConf;
  std::string GetAttr(const xercesc::Attributes &rAttrs, const std::string & AttrName);
  Vector3D str_to_Vector3D(const std::string& s);
  Vector3D_int str_to_Vector3D_int(const std::string& s);
  Vector3D GetVectorAttr(const xercesc::Attributes& rAttrs, 
                                                const std::string& AttrName, 
                                                const Vector3D& DefVal);
  Vector3D_int GetVectorIntAttr(const xercesc::Attributes& rAttrs, 
                                                const std::string& AttrName, 
                                                const Vector3D_int& DefVal);
};

#endif
