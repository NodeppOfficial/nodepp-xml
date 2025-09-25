/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h> 
#include <nodepp/https.h>
#include <nodepp/fs.h>
#include <xml/xml.h>

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {

    fetch_t args; ssl_t ssl; 
    /*---*/ args.method = "GET";
    /*---*/ args.url    = "https://webscraper.io/test-sites/tables";

    https::fetch( args, ssl )

    .then([=]( https_t cli ){
        auto raw = stream::await( cli );
        auto data= xml::parse   ( raw );

        for( auto x: xml::find_element( data,"[class*=table-bordered]" ) ){
        for( auto y: xml::find_element( x   , "tr" ) ){
             console::log( xml::get_text( y ) );
        }}

    })

    .fail([=]( except_t err ){
        console::log( err.what() );
    });

}

/*────────────────────────────────────────────────────────────────────────────*/