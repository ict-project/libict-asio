//! @file
//! @brief Types - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @date 2022
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2022, ICT-Project Mariusz Ornowski (ict-project.pl)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the ICT-Project Mariusz Ornowski nor the names
of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**************************************************************/
#ifndef _ASIO_TYPES__HEADER
#define _ASIO_TYPES__HEADER
//============================================
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <system_error>
//============================================
namespace ict { namespace asio {
//============================================
//! Typ - metadane obiektu
typedef std::map<std::string,std::string> map_info_t;
//! Kod błędu
typedef std::error_code error_code_t;
//! Ogólny handler
typedef std::function<void(void)> asio_handler_t;
//! Handler z obsługą błedu.
typedef std::function<void(const error_code_t&)> error_handler_t;
//============================================
namespace message {
//============================================
//! Struktura z danymi zapytania.
struct request_t {
    std::string method;
    std::string uri;
    std::string version;
};
//! Struktura z danymi odpowiedzi.
struct response_t {
    std::string version;
    std::string code;
    std::string explanation;
};
//! Struktura z danymi nagłówka.
struct header_t {
    std::string name;
    std::string value;  
};
//! Typ listy nagłówków.
typedef std::vector<header_t> headers_t;
//! Struktura z danymi zapytania oraz nagłówków.
struct request_headers_t {
    request_t request;
    headers_t headers;
};
//! Struktura z danymi odpowiedzi oraz nagłówków.
struct response_headers_t {
    response_t response;
    headers_t headers;
};
//============================================
}}}
//===========================================
#endif