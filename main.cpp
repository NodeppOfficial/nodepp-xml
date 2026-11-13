/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h> 
#include <nodepp/fs.h>
#include <xml/xml.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    auto raw  = xml::parse( NODEPP_STRINGIFY(
    <!DOCTYPE html>
    <body>
        <div>
            <h1> Hello World 1! </h1>
            <h2> Hello World 2! </h2>
            <h3> Hello World 3! </h3>
            <input type="checkbox" />
            <img src="http://image.png" />
        </div>
    </body>
    ));

    auto el = xml::find_element( raw, "h1" );
    console::log( xml::get_text( el ) );

    auto im = xml::find_element( raw, "img" );
    console::log( xml::get_attribute( im, "src" ) );

    fs::writable( "out.json" ).write( json::stringify( raw ) );

}

/*────────────────────────────────────────────────────────────────────────────*/