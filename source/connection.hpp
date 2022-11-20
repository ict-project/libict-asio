//! @file
//! @brief ASIO connection module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @date 2020-2022
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2020-2022, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#ifndef _ASIO_CONNECTION_HEADER_HPP
#define _ASIO_CONNECTION_HEADER_HPP
//============================================
#include <vector>
#include <string>
#include <memory>
#include "types.hpp"
//============================================
namespace ict { namespace asio { namespace connection {
//===========================================
//! Interfejs do obsługi połączeń.
class interface : public std::enable_shared_from_this<interface> {
public:
  //! Typ pomocniczy do generowania wskaźnika.
  typedef  std::enable_shared_from_this<interface> enable_shared_t;
  //! Typ - Funkcja do obsługi zapisu lub odczytu.
  typedef std::function<void(const ict::asio::error_code_t&,std::size_t)> handler_t;
  //! Typ - Bufor do odczytu lub zapisu (Uwaga: rozmiar musi być ustawiony przed użyciem!).
  typedef std::vector<unsigned char> buffer_t;
  //! Metadane połączenia
  map_info_t info;
  std::string getInfo() const {
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
  //! Funkcja zamyka połączenie.
  virtual void close()=0;
  //! Sprawdza, czy połaczenie jest nadal otwarte.
  virtual bool is_open() const=0;
  //! Zwraca ilość bajtów oczekujących na odczyt.
  virtual std::size_t available() const=0;
  //! Anuluje wszystkie asynchroniczne operacje w połączeniu.
  virtual void cancel()=0;
  virtual void cancel(error_code_t& ec)=0;
  //! Zapisuje dane do połączenia
  //! @param buffer Bufor z danymi do zapisu (Uwaga: rozmiar musi być ustawiony przed użyciem!).
  //! @param handler Funkcja do obsługi zapisu.
  virtual void async_write_some(buffer_t& buffer,const handler_t &handler)=0;
  //! Odczytuje dane z połączenia
  //! @param buffer Bufor dla danych z odczytu (Uwaga: rozmiar musi być ustawiony przed użyciem!).
  //! @param handler Funkcja do obsługi odczytu.
  virtual void async_read_some(buffer_t& buffer,const handler_t &handler)=0;
  //! Dodaje zadanie do wykonania w ramach ::asio::strand
  //! @param handler Zadanie do wykonania.
  virtual void post(const asio_handler_t &handler)=0;
  //! Zwraca nazwę serwera (SNI).
  //! @returns Nazwa serwera (SNI).
  virtual const std::string & getSNI() {static const std::string nic;return(nic);};
};
//===========================================
//! Wskaźnik do interfejsu do obsługi połączeń.
typedef std::shared_ptr<interface> interface_ptr;
//! Handler do obsługi nowych połączeń
//! @param ec Kod błędu
//! @param interface  Wskaźnik do interfejsu do obsługi połączeń.
typedef std::function<void(const error_code_t&,interface_ptr)> connection_handler_t;
//============================================
}}}
//===========================================
#endif
