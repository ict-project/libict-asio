//! @file
//! @brief ASIO connection module - header file.
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
#ifndef _ASIO_CONNECTION_HEADER
#define _ASIO_CONNECTION_HEADER
//============================================
#include <vector>
#include <string>
#include <memory>
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/ssl/context.hpp>
#include "asio.hpp"
//============================================
namespace ict { namespace asio { namespace connection {
//===========================================
//! Interfejs do obs??ugi po????cze??.
class interface : public std::enable_shared_from_this<interface> {
public:
  //! Typ pomocniczy do generowania wska??nika.
  typedef  std::enable_shared_from_this<interface> enable_shared_t;
  //! Typ - Funkcja do obs??ugi zapisu lub odczytu.
  typedef std::function<void(const ict::asio::error_code_t&,std::size_t)> handler_t;
  //! Typ - Bufor do odczytu lub zapisu (Uwaga: rozmiar musi by?? ustawiony przed u??yciem!).
  typedef std::vector<unsigned char> buffer_t;
  //! Metadane po????czenia
  map_info_t info;
  std::string getInfo(){
    std::string o;
    for (map_info_t::const_iterator it=info.begin();it!=info.end();++it){
      if (it!=info.begin()) o+=",";
      o+=it->first;
      o+="=";
      o+=it->second;
    }
    return(o);
  }
public:
  //! Destruktor
  virtual ~interface(){}
  //! Funkcja zamyka po????czenie.
  virtual void close()=0;
  //! Sprawdza, czy po??aczenie jest nadal otwarte.
  virtual bool is_open() const=0;
  //! Zwraca ilo???? bajt??w oczekuj??cych na odczyt.
  virtual std::size_t available() const=0;
  //! Zapisuje dane do po????czenia
  //! @param buffer Bufor z danymi do zapisu (Uwaga: rozmiar musi by?? ustawiony przed u??yciem!).
  //! @param handler Funkcja do obs??ugi zapisu.
  virtual void async_write_some(buffer_t& buffer,const handler_t &handler)=0;
  //! Odczytuje dane z po????czenia
  //! @param buffer Bufor dla danych z odczytu (Uwaga: rozmiar musi by?? ustawiony przed u??yciem!).
  //! @param handler Funkcja do obs??ugi odczytu.
  virtual void async_read_some(buffer_t& buffer,const handler_t &handler)=0;
  //! Zwraca nazw?? serwera (SNI).
  //! @returns Nazwa serwera (SNI).
  virtual const std::string & getSNI() {static const std::string nic;return(nic);};
};
//===========================================
//! Wska??nik do interfejsu do obs??ugi po????cze??.
typedef std::shared_ptr<interface> interface_ptr;
//! Wska??nik do kontekstu po????czenia SSL
typedef std::shared_ptr<::asio::ssl::context> context_ptr;
//! Handler do obs??ugi nowych po????cze??
//! @param ec Kod b????du
//! @param interface  Wska??nik do interfejsu do obs??ugi po????cze??.
typedef std::function<void(const error_code_t&,interface_ptr)> connection_handler_t;
//===========================================
//! Zwraca interfejs do obs??ugi po????czenia (bez SSL).
//! @param socket Gniazdo TCP.
//! @returns Wska??nik do interfejsu do obs??ugi po????czenia.
interface_ptr get(::asio::ip::tcp::socket & socket);
//! Zwraca interfejs do obs??ugi po????czenia (bez SSL).
//! @param socket Gniazdo lokalne.
//! @returns Wska??nik do interfejsu do obs??ugi po????czenia.
interface_ptr get(::asio::local::stream_protocol::socket & socket);
//! Zwraca interfejs do obs??ugi po????czenia (z SSL).
//! @param socket Gniazdo TCP.
//! @param context Wska??nik do kontekstu po????czenia SSL.
//! @param setSNI Ustawia nazw?? serwera (SNI) - gdy po????czenie jako klient.
//! @returns Wska??nik do interfejsu do obs??ugi po????czenia.
interface_ptr get(::asio::ip::tcp::socket & socket,context_ptr & context,const std::string & setSNI="");
//! Zwraca interfejs do obs??ugi po????czenia (z SSL).
//! @param socket Gniazdo lokalne.
//! @param context Wska??nik do kontekstu po????czenia SSL.
//! @param setSNI Ustawia nazw?? serwera (SNI) - gdy po????czenie jako klient.
//! @returns Wska??nik do interfejsu do obs??ugi po????czenia.
interface_ptr get(::asio::local::stream_protocol::socket & socket,context_ptr & context,const std::string & setSNI="");
//============================================
}}}
//===========================================
#endif
