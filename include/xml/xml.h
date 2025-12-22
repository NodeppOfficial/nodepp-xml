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

namespace nodepp { class xml_t{
protected:

    using ARRAY = array_t<object_t>;
    map_t<string_t,void*> cache;

protected:

    string_t base64( const string_t& data ) const noexcept {
        return encoder::base64::atob( data );
    }

    string_t ascii( const string_t& data ) const noexcept {
        return encoder::base64::btoa( data );
    }

    /*─······································································─*/

    ptr_t<ulong> check_key( const string_t& str, ulong offset ) const noexcept {
        static ptr_t<string_t> pattern ({ "<!--", "</", "<", "-->", ">", "\n" });
        auto   data = str.slice( offset, offset+4 ); ulong y=0, z=0;

        for( auto x: pattern ){
        if ( memcmp(data.get (),x.get (), min(data.size(),x.size()) )==0 )
           { z=min (data.size(),x.size()); break; } y++; }

        return ptr_t<ulong>({ offset, y, z });
    }

    void wait_next( const string_t& str, ulong& offset ) const noexcept {
        static ptr_t<string_t> pattern ({ "/*", "*/", "#", "//", "\n", "\"" });
        auto   data = str.slice( offset, offset+2 ); ulong y=0;
        auto   pttr = string_t();

        for( auto x: pattern ){
        if ( memcmp(data.get(),x.get(),min(data.size(),x.size()))==0 )
           { break; } y++; } 
           
        switch(y){
        case 0 : pttr=pattern[1]; break; case 2 : pttr=pattern[4]; break; 
        case 3 : pttr=pattern[4]; break; case 5 : pttr=pattern[5]; break;
        /*----------------------------*/ default: return; /*----*/ break; 
        } offset++;

        do { auto data = str.slice( offset, offset+2 );
        if ( memcmp(data.get(),pttr.get(),min(data.size(),pttr.size()))==0 )
           { break; }} while( offset++<str.size() );

        offset += pttr.size(); offset--;

    }

    ptr_t<ulong> get_next_key( const string_t& str, ulong& offset ) const noexcept {
        ptr_t<ulong> act=check_key( str, offset );
        ulong start =offset; offset += act[2];
        
        ulong y= 0 , w = act[1]; switch( act[1] ){
            case 0 : y=3; break; case 1 : y=4; break;
            case 2 : y=4; break; case 5 : y=6; break;
            /*----------------*/ case 6 : y=6; break; 
            default: /*-----*/ return nullptr; break;
        }

        if( y==6 ){ offset++;

            while( offset < str.size() ){ 
                /*-*/ wait_next( str,offset );
                act = check_key( str,offset );
            if( act[1]<5 ){ break; } offset++; } offset--; 

            return ptr_t<ulong>({ 3UL, start, offset+1 });

        } else {

            while( offset < str.size() ){ 
                /*-*/ wait_next( str, offset );
                act = check_key( str, offset );
            if( act[1]==y ){ break; } offset++; }
            
            offset+=act[2]; /**/ offset--; 

            ulong type = w==0 ? 2UL : /**/
                  type = w==1 ? 1UL : 0UL; 
            
            return ptr_t<ulong>({ type, start, offset+1 });

        }
    
    return nullptr; }

    /*─······································································─*/

    object_t parse_tag( const string_t& str, const ptr_t<ulong>& data ) const noexcept {

        thread_local static regex_t reg1( "(\\w+=\"[^\"]+\")|(\\w+)" ); reg1.clear_memory();
        thread_local static regex_t reg2( "^(\\w+)=\"([^\"]+)\"$" );    reg2.clear_memory();
        /*--------*/ static regex_t reg3( "[\n\t ]+" ); auto raw=str.slice(data[1],data[2]);

        switch( data[0] ){ case 3 : do {
            return object_t({ { "_text_",    base64 ( raw ) } });
        } while(0); break; case 2 : do {
            return object_t({ { "_comment_", base64 ( raw ) } });
        } while(0); break; case 1 : do {
            auto data = reg1.match( raw ); cache[data] = nullptr;
            return object_t({ { "_close_", reg1.match(data) } });
        } while(0); break; default: do {
            reg1.match_all( raw ); /*--*/ auto mem = reg1.get_memory();
            object_t attr; for( ulong x=0; x++<mem.size()-1; ){
                reg2.match_all( mem[x] ); auto mem = reg2.get_memory();
                attr[ mem[0] ] =mem.size()!=2 ? string::null( /*--*/ )
                /*----------*/ :reg3.replace_all(mem[1],string::space());
                reg2.clear_memory();
            }   return object_t({ { "type", mem[0] }, { "attr", attr } });
        } while(0); break; }
        
    return nullptr; }

    /*─······································································─*/

    ARRAY parse_list( const string_t& str ) const { ARRAY list;
        for( ulong x=0; x<str.size(); x++ ){
        if ( str[x]=='\n' || str[x]==' ' || str[x]=='\t' )
           { continue; } auto data=get_next_key( str, x );
        if ( !data.empty() ){ list.push ( parse_tag( str, data ) ); }
       else{  list.push ( parse_tag( str, ptr_t<ulong>({ 3UL, x-1, x }) ) ); x--; }}
    return list; }

    /*─······································································─*/

    ARRAY parse_child( const ARRAY& raw, ulong& offset ) const noexcept { ARRAY list;
        while( offset <raw.size() ){ auto x = raw[offset]; offset++;
        if ( x.has( "_close_"   ) ){ /*--------------*/ break; }
        if ( x.has( "_comment_" ) ){ list.push( x ); continue; }
        if ( x.has( "_text_"    ) ){ list.push( x ); continue; }
        if ( cache.has( x["type"].as<string_t>( ) ) )
           { x["children"]=parse_child(raw,offset); }
        list.push( x ); }
    return list; }

    /*─······································································─*/

    string_t format_tag( const object_t& obj ) const {

        if( obj.has("_root_"   ) ){ return format_array( obj["children"].as<array_t<object_t>>() ); }
        if( obj.has("_comment_") ){ return ascii( obj["_comment_"].as<string_t>() ); }
        if( obj.has("_text_"   ) ){ return ascii( obj["_text_"]   .as<string_t>() ); }
        if(!obj.has("type")      ){ throw  except_t( "invalid tag" ); }

        string_t type= obj["type"].as<string_t>();
        string_t out = string::format( "<%s", type.get() );
        string_t end = string::format( ">${0}</%s>", type.get() ); 

        for( auto x: obj["attr"].keys() ){ 
             auto y= json::stringify ( obj["attr"][x] ); 
             out  += regex::format   ( " ${0}=${1}", x, y ); 
        }

        if   ( !obj.has("children") )
             { return out + "/>"; }
        else { return out + end ; }

    }

    string_t format_object( const object_t& obj ) const {
    if( !obj.has_value() ){ return string::null(); }
        
        string_t tag = format_tag( obj );

        if( obj.has( "children" )   ){
        if( obj["children"].empty() ){ return regex::format( tag, "" ); }
            string_t borrow= format_array( obj["children"].as<ARRAY>() );
            return regex::format( tag, borrow );
        } else { return tag; } 

    }

    string_t format_array( const ARRAY& obj ) const { 
    if( obj.empty() ){ return string::null(); }
    string_t borrow; for( auto x: obj ){
    /*----*/ borrow += format_object(x);
    } return borrow; }

public:

    /*----*/ xml_t() {}
    virtual ~xml_t() {}

    object_t parse( const string_t& str ) const {
        ulong offset = 0; return object_t({
            { "_root_", "xml" }, 
            { "children", parse_child( parse_list(str), offset ) }
        });
    }

    string_t format( const object_t& obj ) const {
        return format_object( obj );
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    string_t format( const object_t& data ){ return xml_t().format(data); }

    object_t parse ( const string_t& data ){ return xml_t().parse (data); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    inline void raw_iterate_all( object_t dom, function_t<void,object_t> callback ){
        callback( dom ); if( !dom.has("children") ){ return; }
        for( auto x: dom["children"].as<array_t<object_t>>() )
           { raw_iterate_all( x, callback ); }
    }

    inline bool raw_iterate( object_t dom, function_t<bool,object_t> callback ){
        if ( !callback( dom ) )/**/{ return false; } 
        if ( !dom.has("children") ){ return true ; }
        for( auto x: dom["children"].as<array_t<object_t>>() ){
        if ( !raw_iterate( x, callback ) ){ return false; }}
    return true; }

    /*─······································································─*/

    inline void iterate_all( object_t dom, function_t<void,object_t> callback ){
        raw_iterate_all( dom, [&]( object_t item ){
            if( item.has( "type" ) ){ callback( item ); }
        });
    }

    inline void iterate( object_t dom, function_t<bool,object_t> callback ){
        raw_iterate( dom, [&]( object_t item ){
            if( item.has( "type" ) ){ return callback( item ); }
            return true;
        });
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    inline bool is_valid_element( object_t dom, string_t type ){ try {

        static regex_t reg ( "[^\\[\\^\\|\\$\\]=>*\"\']+" );
            
        if( !dom.has_value()  ) /*-------------*/ { return false; }
        if( !dom.has( "type" )) /*-------------*/ { return false; }
        
        if( type.empty() ) /*------------------*/ { return true ; }
        if( type[0]=='*' ) /*------------------*/ { return true ; }

        if( type[0] == '[' ){ auto data = reg.match_all( type );
        if( data.empty() || !dom.has("attr") )   { return false; }
        if( data.size()==1 ){ return dom["attr"].has( data[0] ); }
        if( !dom["attr"].has( data[0] ) ) /*--*/ { return false; }
            
            if  ( regex::test( type, "\\*=" ) )
                { return regex::test( dom["attr"][data[0]].as<string_t>(), data[1], true ); }

            elif( regex::test( type, "\\$=" ) )
                { return regex::test( dom["attr"][data[0]].as<string_t>(), data[1]+"$", true ); }

            elif( regex::test( type, "\\^=" ) )
                { return regex::test( dom["attr"][data[0]].as<string_t>(), "^"+data[1], true ); }
            
            elif( regex::test( type, "=" ) )
                { return regex::test( dom["attr"][data[0]].as<string_t>(), "^"+data[1]+"$", true ); }

        }   else{ return dom["type"].as<string_t>()==type; }

    } catch( except_t ) {} return false; };

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    object_t find_element_all( object_t node, string_t data ){

        auto pttr = regex::split( data, "\\s+|,+" );
        if( node.empty() ){ return nullptr; }
        queue_t<object_t> out; /*----------*/

        iterate_all( node, [&]( object_t item ){
        if ( pttr.some([=]( string_t pttr ){
             return is_valid_element( item, pttr );
        })){ out.push( item ); } });
        
    return object_t({
        { "_root_", "xml" }, { "children", array_t<object_t>( out.data() ) }
    }); }

    /*─······································································─*/

    object_t find_element( object_t node, string_t data ){

        auto pttr = regex::split( data, "\\s+|,+" );
        if( node.empty() ){ return nullptr; }
        object_t out; 

        iterate( node, [&]( object_t item ){
        if ( pttr.some([=]( string_t pttr ){
             return is_valid_element( item, pttr );
        })){ out=item; return false; } return true; });
        
    return out; }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    void set_attribute( object_t obj, string_t name, string_t value ) {
         obj["attr"][ name ] = value;
    }

    string_t get_attribute( object_t obj, string_t name ) {
         return obj["attr"][ name ].as<string_t>();
    }

    string_t get_xml( object_t dom ){ return format( dom ); }

    /*─······································································─*/
    
    object_t append_child( object_t& parent, object_t child ){
        if(!parent.has_value() ){ return /*--*/ nullptr; }
        if( parent.has( "children" ) ){
            parent["children"].as<array_t<object_t>>()
            /*--------------*/.push( child );
        } else {
            auto array=array_t<object_t>();
            /**/ array.push( child );
            parent["children"]=array;
        }   return parent;
    }

    object_t create_element( string_t type ) { return object_t({
        { "type", type }, { "attr", object_t() }
    }); }

    /*─······································································─*/

    void set_text( object_t obj, string_t data ) { 
         if( !obj.has( "children" ) ){ obj["children"]=array_t<object_t>(); }
         obj["children"].as<array_t<object_t>>().push( object_t({
            { "_text_", encoder::base64::atob( data ) }
         }) );
    }

    string_t get_text( object_t element ) {
        if( !element.has( "children" ) ) /*--*/ { return nullptr; }
        string_t out; 
        for( auto x: element["children"].as<array_t<object_t>>() ){
             raw_iterate_all( x,/**/ [&]( object_t dom ){
        if ( dom.has ("_text_") ){ out += format ( dom ); }
        });} return out;
    }

} }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
