# Nodepp XML Library

A lightweight C++ library for parsing and formatting XML, designed for high-performance, asynchronous applications.

---

## Overview

The Nodepp-XML library provides a simple and efficient way to handle XML data in C++. It converts XML strings into a structured **JSON-like object model**, allowing for easy manipulation and traversal. The library also supports formatting these objects back into well-formed XML strings.

This library is a component of the [Nodepp Project](https://github.com/NodeppOfficial/nodepp), an open-source C++ framework inspired by Node.js.

---

## Features

- **XML Parsing**: Converts XML documents into a a hierarchical `object_t` structure.
- **XML Formatting**: Generates XML strings from the structured object model.
- **DOM-like API**: Includes helper functions to create, append, and modify elements and attributes.
- **Base64 Encoding**: Automatically handles text content and whitespace preservation using Base64.
- **Lightweight**: Built with a focus on performance and minimal dependencies within the Nodepp ecosystem.

---

## Getting Started

### Prerequisites
```bash
- A C++11 or above compliant compiler.
- The Nodepp framework.
```

### Usage
```cpp
#include <nodepp/nodepp.h> 
#include <nodepp/fs.h>
#include <xml/xml.h>

using namespace nodepp;

void onMain() {

    file_t fout ( "out.html" , "w" );
    file_t finp ( "test.html", "r" );

    auto data = stream::await( finp );
    auto out  = xml::parse   ( data );

    fout.write( xml::format( out ) );

}
```