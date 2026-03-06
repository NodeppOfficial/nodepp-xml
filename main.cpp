/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h> 
#include <nodepp/fs.h>
#include <xml/xml.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    auto data = fs::read_file( "test.html" ).await().value();
    auto out  = xml_t();
    auto raw  = out.parse( data );

    console::log( encoder::base64::btoa(
        xml::find_element_all( raw, "h1" )["children"][0]["children"][1]["children"][0]["_text_"].as<string_t>()
    ) );

}

/*────────────────────────────────────────────────────────────────────────────*/