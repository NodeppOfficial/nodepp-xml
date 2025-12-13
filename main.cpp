/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h> 
#include <nodepp/fs.h>
#include <xml/xml.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    file_t finp ( "index.html", "r" );
    auto data = stream::await( finp );
    auto out  = xml_t();
    auto raw  = out.parse( data );

    xml::iterate_all( raw, [=]( object_t item ){
        console::log( "->", item["type"].as<string_t>() );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/