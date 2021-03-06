//! @file
//! @brief Connector module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 2.0
//! @date 2020-2021
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2020-2021, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#ifndef _ASIO_CONNECTOR_HEADER
#define _ASIO_CONNECTOR_HEADER
//============================================
#include "asio.hpp"
#include "connection.hpp"
//============================================
namespace ict { namespace asio { namespace connector {
//===========================================
class interface : public std::enable_shared_from_this<interface>{
public:
    typedef  std::enable_shared_from_this<interface> enable_shared_t;
    map_info_t info;
public:
    //! Destruktor
    virtual ~interface(){}
    //! Funkcja zamyka konektora.
    virtual void close()=0;
    //! Sprawdza, czy konektor jest nadal otwarty.
    virtual bool is_open() const=0;
    //! Sprawdza, czy w konektorze wyst??pi?? b????d.
    virtual bool is_error() const=0;
    //! Obs??uguje nowe po????czenie.
    //! @param handler Funkcja do obs??ugi nowego po??aczenia.
    virtual void async_connection(const ict::asio::connection::connection_handler_t &handler)=0;
};
//===========================================
//! Wska??nik do interfejsu konektora.
typedef std::shared_ptr<interface> interface_ptr;
//===========================================
//! Funkcja do tworzenia konektor??w dla TCP.
//! @param host Host, na kt??rym ma si?? bindowa?? (jako serwer), lub do kt??rego ma si?? ????czy?? (jako klient).
//! @param port Port, na kt??rym ma si?? bindowa?? (jako serwer), lub do kt??rego ma si?? ????czy?? (jako klient).
//! @param server Informacja, czy to ma by?? konektor typu serwer, czy typu klient.
interface_ptr get(const std::string & host,const std::string & port,bool server=true);
//! Funkcja do tworzenia konektor??w dla gniazd lokalnych.
//! @param path ??cie??ka, na kt??rej ma si?? bindowa?? (jako serwer), lub do kt??rej ma si?? ????czy?? (jako klient).
//! @param server Informacja, czy to ma by??  konektor typu serwer, czy typu klient.
interface_ptr get(const std::string & path,bool server=true);
//! Funkcja do tworzenia konektor??w dla TCP.
//! @param host Host, na kt??rym ma si?? bindowa?? (jako serwer), lub do kt??rego ma si?? ????czy?? (jako klient).
//! @param port Port, na kt??rym ma si?? bindowa?? (jako serwer), lub do kt??rego ma si?? ????czy?? (jako klient).
//! @param context Informacja, czy po????czenia maj?? by?? szyfrowane, czy nie (je??li tak, to trzeba ustawi?? kontekst).
//! @param server Informacja, czy to ma by?? konektor typu serwer, czy typu klient.
interface_ptr get(const std::string & host,const std::string & port,const ict::asio::connection::context_ptr & context,bool server=true,const std::string & setSNI="");
//! Funkcja do tworzenia konektor??w dla gniazd lokalnych.
//! @param path ??cie??ka, na kt??rej ma si?? bindowa?? (jako serwer), lub do kt??rej ma si?? ????czy?? (jako klient).
//! @param context Informacja, czy po????czenia maj?? by?? szyfrowane, czy nie (je??li tak, to trzeba ustawi?? kontekst).
//! @param server Informacja, czy to ma by??  konektor typu serwer, czy typu klient.
interface_ptr get(const std::string & path,const ict::asio::connection::context_ptr & context,bool server=true,const std::string & setSNI="");
//============================================
}}}
//===========================================
#endif