//! @file
//! @brief Broker module - header file.
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
#ifndef _ASIO_BROKER_HEADER
#define _ASIO_BROKER_HEADER
//============================================
#include "types.hpp"
#include <memory>
#include <functional>
//============================================
namespace ict { namespace asio { namespace broker {
//===========================================
class interface : public std::enable_shared_from_this<interface>{
public:
    //! Typ pomocniczy do generowania wskaźnika.
    typedef  std::enable_shared_from_this<interface> enable_shared_t;
    enum status_t {
        unknown,
        request_headers,
        request_body,
        response_headers,
        response_body,
    };
protected:
    status_t status=unknown;
public:
    //! Typ - Funkcja do obsługi zapisu lub odczytu.
    typedef ict::asio::error_handler_t handler_t;
    struct {
        ict::asio::message::request_headers_t headers;
        std::string body;
        std::size_t bytesLeft=0;
    } request;
    struct {
        ict::asio::message::response_headers_t headers;
        std::string body;
        std::size_t bytesLeft=0;
    } response;
public:
    //! Destruktor
    virtual ~interface(){}
    const status_t & getStatus() const {return status;}
    virtual map_info_t & info()=0;
    //! Funkcja zamyka połączenie.
    virtual void close()=0;
    //! Sprawdza, czy połaczenie jest nadal otwarte.
    virtual bool is_open() const=0;
    //! Zwraca ilość bajtów oczekujących na odczyt.
    virtual std::size_t available() const=0;
    //! Anuluje wszystkie asynchroniczne operacje w połączeniu.
    virtual void cancel()=0;
    virtual void cancel(error_code_t& ec)=0;
    //! Dodaje zadanie do wykonania w ramach ::asio::strand
    //! @param handler Zadanie do wykonania.
    virtual void post(const asio_handler_t &handler)=0;
    //! Zwraca nazwę serwera (SNI).
    //! @returns Nazwa serwera (SNI).
    virtual const std::string & getSNI()=0;
    //! 
    //! @brief Zapisuje dane body wiadomości.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    virtual void async_write_body(const handler_t &handler)=0;
    //! 
    //! @brief Odczytuje dane body wiadomości.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    virtual void async_read_body(const handler_t &handler)=0;
    //! 
    //! @brief Zapisuje wiersz zapytania oraz nagłówki.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    virtual void async_write_request_headers(const handler_t &handler)=0;
    //! 
    //! @brief Odczytuje wiersz zapytania oraz nagłówki.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    virtual void async_read_request_headers(const handler_t &handler)=0;
    //! 
    //! @brief Zapisuje wiersz odpowiedzi oraz nagłówki.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    virtual void async_write_response_headers(const handler_t &handler)=0;
    //! 
    //! @brief Odczytuje wiersz odpowiedzi oraz nagłówki.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    virtual void async_read_response_headers(const handler_t &handler)=0;
};
//===========================================
//! Wskaźnik do interfejsu brokera.
typedef std::shared_ptr<interface> interface_ptr;
//! Handler zwracający brokera.
typedef std::function<void(const error_code_t&,interface_ptr)> broker_handler_t;
//===========================================
//! Funkcja do połączeń TCP.
//! @param host Host, na którym ma się bindować (jako serwer), lub do którego ma się łączyć (jako klient).
//! @param port Port, na którym ma się bindować (jako serwer), lub do którego ma się łączyć (jako klient).
//! @param server Informacja, czy to ma być połaczenie typu serwer, czy typu klient.
//! @param context Informacja, czy połączenia mają być szyfrowane, czy nie (jeśli tak, to trzeba ustawić kontekst).
//! @param sni Ustawnia SNI dla szyfrowanych połączeń wychodzących (klient) lub oczkuje podanego SNI (server).
void get(const broker_handler_t & handler,const std::string & host,const std::string & port,bool server=true,const ict::asio::context_ptr & context=NULL,const std::string & sni="");
//! Funkcja połączeń lokalnych.
//! @param path Ścieżka, na której ma się bindować (jako serwer), lub do której ma się łączyć (jako klient).
//! @param server Informacja, czy to ma być  połaczenie typu serwer, czy typu klient.
//! @param context Informacja, czy połączenia mają być szyfrowane, czy nie (jeśli tak, to trzeba ustawić kontekst).
//! @param sni Ustawnia SNI dla szyfrowanych połączeń wychodzących (klient) lub oczkuje podanego SNI (server).
void get(const broker_handler_t & handler,const std::string & path,bool server=true,const ict::asio::context_ptr & context=NULL,const std::string & sni="");
//============================================
}}}
//===========================================
#endif