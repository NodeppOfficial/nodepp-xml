/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_XML
#define NODEPP_XML

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/json.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class xml_t {
private:

    using QUEUE = map_t<string_t,object_t>;
    using ARRAY = array_t<object_t>;

protected:

    string_t base64( const string_t& data ) const noexcept {
        return encoder::base64::atob( regex::replace_all( data, "[\r\n\t ]+", " " ) );
    }

    string_t ascii( const string_t& data ) const noexcept {
        return encoder::base64::btoa( data );
    }

    /*─······································································─*/

    object_t parse_tag( const string_t& str, ulong start, ulong end ) const {
        static regex_t reg( "^<([^ >]+)|(\"[^\"]*\")|(\\w+)" );
        auto raw = str.slice( start, end ); /*---------------*/
        if(!regex::test( raw,"^<(/|\\w)" ) ){ return nullptr; }
        if(!regex::test( raw,"^<|>$" ) ) /*------------------*/
          { throw except_t( "invalid xml format" ); }

        /*-------*/ reg.search_all( raw );
        auto attr = map_t<string_t,string_t>();
        auto data = reg.get_memory(); reg.clear_memory(); 

        for( auto &x: data.slice(1) ){
        if ( x[0] != '"' ){ attr[x]=string::null(); continue; }
        if ( attr.raw().empty() ) /*-----------*/ { continue; }
        auto data = regex::replace_all(x.slice(1,-1),"[\n\t\r ]+"," ");
        attr.raw().last()->data.second = data.empty()?string::null():data; }

        return object_t({
            { "type", data[0] /*-------*/ },
            { "attr", json::parse( attr ) },
        });

    }

    ARRAY parse_child( const string_t& str, const ARRAY& list ) const {

        if( str .empty() )/*----------------------*/{ return nullptr; }
        if( list.empty() )/*----------------------*/{ return nullptr; }
        if( list[0]["type"].as<string_t>()[0]=='/' ){ return nullptr; } 

        auto out = ARRAY(); /*------------------------*/ uint k=0, j=0;
        
        for( auto x=0; x<list.size(); x++ ){ auto a = list[x]["type"].as<string_t>(); k=0;
        for( auto y=x; y<list.size(); y++ ){ auto b = list[y]["type"].as<string_t>(); ++j;

        if  ( b[0]=='/' && b.slice(1)==a ){ --k; }
        elif( /*--------*/ b /*----*/==a ){ ++k; }
        if  ( k==0 ) /*--------------*/ { break; }

        } if( k!=0 ) { out.push( list[x] ); } else { do {

            auto n_item = list[x];
            auto n_list = list.slice( x+1, j-1 );

            if( n_list.empty() ){ 
                auto z=list[x+1]["offset"].as<ptr_t<ulong>>(/*-*/); 
                n_item["text"] = base64( str.slice( z[0], z[1] ) );
                out.push( n_item ); /*--------------------*/ break;
            }
            
            auto n_out = parse_child( str, n_list );
            n_item["children"]=n_out; out.push( n_item );

        } while(0); x=j-1; } j=x+1; }

    return out; }

    ARRAY parse_list( const string_t& str ) const {
        static regex_t reg ( "<!--[^-]+-->|<[^>]+>" );
        auto data = reg.search_all( str );
        auto out  = ARRAY(); ulong off =0;
        
        for( auto &x: data ){
             auto y = parse_tag ( str, x[0], x[1] );
        if ( !y.has_value() ){ off=x[1];continue; } 
        
             auto z = ptr_t<ulong>({ off, x[0] });
             auto data = regex::replace_all( str.slice( z[0], z[1] ), "[\r\n\t ]+", " " );

        if ( !data.empty() && !regex::test( data, "^[\r\n\t ]+$" ) )
           { out.push(object_t({ { "type", "_text_" }, { "data", base64( data ) } })); }
        
        out.push(y); off=x[1]; }

    return out; }

    /*─······································································─*/

    string_t format_tag( const object_t& obj ) const {
    if( !obj.has_value() ){ throw except_t( "invalid tag" ); }
    if( !obj.has("type") ){ throw except_t( "invalid tag" ); }

        string_t type= obj["type"].as<string_t>();

    if( type=="_text_" ){ return ascii( obj["data"].as<string_t>() ); }

        string_t out = string::format( "<%s", type.get() );
        string_t end = string::format( ">${0}</%s>", type.get() ); 

        for( auto x: obj["attr"].keys() ){ 
             auto y= json::stringify ( obj["attr"][x] ); 
             out += regex::format( " ${0}=${1}", x, y ); 
        }

        if ( !obj.has("children") )
             { return out + "/>"; }
        else { return out + end ; }

    }

    string_t format_object( const object_t& obj ) const {
    if( !obj.has_value() ){ return string::null(); }
        
        string_t tag = format_tag( obj );

        if( obj.has( "children" ) ){
            string_t borrow= format_array( obj["children"].as<ARRAY>() );
            return regex::format( tag, borrow );
        } else { return tag; } 

    }

    string_t format_array( const ARRAY& obj ) const { 
    if( obj.empty() ){ return string::null(); }
    string_t borrow; for( auto x: obj ){
    /*----*/ borrow += format_object( x );
    } return borrow; }

public:

    /*----*/ xml_t() noexcept {}
    virtual ~xml_t() noexcept {}

    ARRAY parse( const string_t& str ) const {
        return parse_child( str, parse_list(str) );
    }

    string_t format( const ARRAY& obj ) const {
        return format_array( obj );
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    string_t format( const array_t<object_t>& data ){ 
        return xml_t().format(data); 
    }

    array_t<object_t> parse( const string_t& data ){ 
        return xml_t().parse(data); 
    }

} }

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    array_t<object_t> find_element( array_t<object_t> node, string_t pttr ){
        if( node.empty() ){ return nullptr; }
        queue_t<object_t> out; /*----------*/

        if( !regex::test( pttr, "," ) ){
        auto patt = "\\[[^\\]]+\\]|[^\\[ *=>]+|[ *=>]?";
        auto data = regex::match_all( pttr, patt );

            console::log( pttr, data.size(), data.join(" | ") );

        return nullptr; }
        
        for( auto x: regex::split( pttr, "," ) )
           { return find_element( node, x ); }
        
        return nullptr;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    void set_attribute( object_t obj, string_t name, string_t value ) {
         obj["attr"][ name ] = value;
    }
    
    object_t append_child( object_t& parent, object_t child ){
        if(!parent.has_value() ){ return /*--*/ nullptr; }
        if( parent.has( "children" ) ){
            parent["children"].as<array_t<object_t>>()
            /*--------------*/.push( child );
        } else {
            auto array = array_t<object_t>();
            /**/ array.push( child );
            parent["children"]=array;
        }   return parent;
    }

    object_t create_element( string_t type ) { return object_t({
        { "type", type }, { "attr", object_t() }
    }); }

    string_t get_attribute( object_t obj, string_t name ) {
         return obj["attr"][ name ].as<string_t>();
    }

} }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/