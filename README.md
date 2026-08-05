# Nodepp XML Library - A lightweight CML parsing and formatting Library for Nodepp.
The Nodepp-XML library provides a simple and efficient way to handle XML data in C++. It converts XML strings into a structured **JSON-like object model**, allowing for easy manipulation and traversal. The library also supports formatting these objects back into well-formed XML strings.

## Dependencies & Cmake Integration
```bash
include(FetchContent)

FetchContent_Declare(
	nodepp
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp)

FetchContent_Declare(
	nodepp-xml
	GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-xml
	GIT_TAG          origin/main
	GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp-xml)

#[...]

target_link_libraries( #[...]
	PUBLIC nodepp nodepp-xml #[...]
)
```

## Features
- **XML Parsing**: Converts XML documents into a a hierarchical `object_t` structure.
- **XML Formatting**: Generates XML strings from the structured object model.
- **DOM-like API**: Includes helper functions to create, append, and modify elements and attributes.
- **Base64 Encoding**: Automatically handles text content and whitespace preservation using Base64.
- **Lightweight**: Built with a focus on performance and minimal dependencies within the Nodepp ecosystem.

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

## License
**Nodepp-xml** is distributed under the MIT License. See the LICENSE file for more details.