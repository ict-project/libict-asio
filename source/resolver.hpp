//! @file
//! @brief DNS resolver module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 2.0
//! @date 2016-2021
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2016-2021, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#ifndef _ASIO_RESOLVER_HEADER
#define _ASIO_RESOLVER_HEADER
//============================================
#include <functional>
#include <vector>
#include <string>
#include <system_error>
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
//============================================
namespace ict { namespace asio { namespace resolver {
//===========================================
//! Kod b????du
typedef std::error_code error_code_t;
//! Lista endpoint??w dla TCP
typedef std::vector<::asio::ip::tcp::endpoint> tcp_endpoint_t;
//! Informacja o endpointach dla TCP
struct tcp_endpoint_info {
    //! Host, na podstawie kt??rego powsta??a lista endpoint??w.
    std::string host;
    //! Port, na podstawie kt??rego powsta??a lista endpoint??w.
    std::string port;
    //! Lista endpoint??w
    tcp_endpoint_t endpoint;
};
//! Wska??nik do informacji o endpointach dla TCP
typedef std::shared_ptr<tcp_endpoint_info> tcp_endpoint_info_ptr;
//! Funkcja do obs??ugi roziwjania nazw dla TCP
//! @param ptr Wska??nik do do informacji o endpointach.
//! @param ec Kod b????du.
typedef std::function<void(tcp_endpoint_info_ptr&,const error_code_t&)> tcp_handler_t;
//! Lista endpoint??w dla gniazd lokalnych
typedef std::vector<::asio::local::stream_protocol::endpoint> stream_endpoint_t;
//! Informacja o endpointach dla gniazd lokalnych
struct stream_endpoint_info {
    //! ??cie??ka, na podstawie kt??rej powsta??a lista endpoint??w.
    std::string path;
    //! Lista endpoint??w
    stream_endpoint_t endpoint;
};
//! Wska??nik do informacji o endpointach dla gniazd lokalnych
typedef std::shared_ptr<stream_endpoint_info> stream_endpoint_info_ptr;
//! Funkcja do obs??ugi roziwjania nazw dla gniazd lokalnych
//! @param ptr Wska??nik do do informacji o endpointach.
//! @param ec Kod b????du.
typedef std::function<void(stream_endpoint_info_ptr&,const error_code_t&)> stream_handler_t;
//===========================================
//! Funkcja do rozwijania nazw dla TCP.
//! @param host Host - dana wej??ciowa.
//! @param port Port - dana wej??ciowa.
//! @param handler Funkcja do obs??ugi roziwjania nazw.
void get(const std::string & host,const std::string & port,const tcp_handler_t & handler);
//! Funkcja do rozwijania nazw dla gniazd lokalnych.
//! @param path ??cie??ka - dana wej??ciowa.
//! @param handler Funkcja do obs??ugi roziwjania nazw.
void get(const std::string & path,const stream_handler_t & handler);
//===========================================
}}}
//===========================================
#endif