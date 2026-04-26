/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h> 
#include <nodepp/fs.h>
#include <xml/xml.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    auto data = fs::read_file( "test.html" ).await().value();
    auto raw  = xml::parse( data );

    fs::write_file( "out.html", xml::format( raw ) ).await();

}

/*────────────────────────────────────────────────────────────────────────────*/