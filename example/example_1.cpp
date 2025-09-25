/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h> 
#include <nodepp/fs.h>
#include <xml/xml.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    file_t fout ( "out.html" , "w" );
    file_t finp ( "test.html", "r" );
    auto data = stream::await( finp );

    auto out = xml::parse( data );
    fout.write( xml::format( out ) );

}

/*────────────────────────────────────────────────────────────────────────────*/