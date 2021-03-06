//! @file
//! @brief Connection (message) module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 1.0
//! @date 2021
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2021, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#ifndef _CONNECTION_MESSAGE_HEADER
#define _CONNECTION_MESSAGE_HEADER
//============================================
#include <string>
#include <vector>
#include "asio.hpp"
#include "connection.hpp"
#include "connection-string.hpp"
//============================================
namespace ict { namespace asio { namespace connection {
//===========================================
class message : public std::enable_shared_from_this<message>{
private:
    //! Bufor odzytu.
    std::string read;
    //! Bufor zapisu.
    std::string write;
    //! Maksymalny rozmiar linii, gdy odczytywany jest wiersz zapytania, odpowiedzi lub nag????wka.
    std::size_t maxRead=0;
    //! Minimalny rozmiar danych do zapisy, gdy zapisywany jest wiersz zapytania, odpowiedzi lub nag????wka.
    std::size_t minWrite=0;
public:
    //! Typ pomocniczy do generowania wska??nika.
    typedef  std::enable_shared_from_this<message> enable_shared_t;
    //! Typ - Funkcja do obs??ugi zapisu lub odczytu.
    typedef std::function<void(const ict::asio::error_code_t&)> handler_t;
    //! Interfejs po????czenia (string).
    string_ptr connection;
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
    //! Struktura z danymi nag????wka.
    struct header_t {
        std::string name;
        std::string value;  
    };
    //! Typ listy nag????wk??w.
    typedef std::vector<header_t> headers_t;
    //! Struktura z danymi zapytania oraz nag????wk??w.
    struct request_headers_t {
        request_t request;
        headers_t headers;
    };
    //! Struktura z danymi odpowiedzi oraz nag????wk??w.
    struct response_headers_t {
        response_t response;
        headers_t headers;
    };
private:
    //! 
    //! @brief Zapisuje body wiadomo??ci.
    //! 
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu zapisu.
    //! 
    void async_write_body(const handler_t &handler);
    //! 
    //! @brief Odczytuje body wiadomo??ci.
    //! 
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu odczytu.
    //! 
    void async_read_body(const handler_t &handler);
public:
    //! 
    //! @brief Konstruktor.
    //! 
    //! @param i Wska??nik do interfejsu string.
    //! 
    message(const string_ptr & i):connection(i){}
    //! 
    //! @brief Zapisuje wiersz zapytania.
    //! 
    //! @param request Dane zapytania.
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu zapisu.
    //! 
    void async_write_request(request_t & request,const handler_t &handler);
    //! 
    //! @brief Odczytuje wiersz zapytania.
    //! 
    //! @param request Dane zapytania.
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu odczytu.
    //! 
    void async_read_request(request_t & request,const handler_t &handler);
    //! 
    //! @brief Zapisuje wiersz odpowiedzi.
    //! 
    //! @param response Dane odpowiedzi.
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu zapisu.
    //! 
    void async_write_response(response_t & response,const handler_t &handler);
    //! 
    //! @brief Odczytuje wiersz odpowiedzi.
    //! 
    //! @param response Dane odpowiedzi.
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu odczytu.
    //! 
    void async_read_response(response_t & response,const handler_t &handler);
    //! 
    //! @brief Zapisuje nag????wek.
    //! 
    //! @param header Dane nag????wka. Je??li header.name jest ustawione na ":", to oznacza koniec nag????wk??w.
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu zapisu.
    //! 
    void async_write_header(header_t & header,const handler_t &handler);
    //! 
    //! @brief Odczytuje nag????wek.
    //! 
    //! @param header Dane nag????wka. Je??li header.name jest ustawione na ":", to oznacza koniec nag????wk??w.
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu odczytu.
    //! 
    void async_read_header(header_t & header,const handler_t &handler);
    //! 
    //! @brief Zapisuje dane body wiadomo??ci.
    //! 
    //! @param data Dane do zapisu.
    //! @param bytesLeft Informacja ile bajt??w body zosta??o do zapisania (aktualizowana).
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu zapisu.
    //! 
    void async_write_body(std::string & data,std::size_t & bytesLeft,const handler_t &handler);
    //! 
    //! @brief Odczytuje dane body wiadomo??ci.
    //! 
    //! @param data Odczytane dane.
    //! @param bytesLeft Informacja ile bajt??w body zosta??o do odczytania (aktualizowana).
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu odczytu.
    //! 
    void async_read_body(std::string & data,std::size_t & bytesLeft,const handler_t &handler);
    //! 
    //! @brief Zapisuje nag????wki.
    //! 
    //! @param headers Dane nag????wk??w. Je??li header.name jest ustawione na ":", to oznacza koniec nag????wk??w (tak poiwnien by?? ustawiony ostatni).
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu zapisu.
    //! 
    void async_write_headers(headers_t & headers,const handler_t &handler);
    //! 
    //! @brief Odczytuje nag????wki.
    //! 
    //! @param headers Dane nag????wk??w. Je??li header.name jest ustawione na ":", to oznacza koniec nag????wk??w (tak b??dzie ustawiony ostatni).
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu odczytu.
    //! 
    void async_read_headers(headers_t & headers,const handler_t &handler);
    //! 
    //! @brief Zapisuje wiersz zapytania oraz nag????wki.
    //! 
    //! @param request Dane zapytania oraz nag????wk??w. Je??li header.name jest ustawione na ":", to oznacza koniec nag????wk??w (tak poiwnien by?? ustawiony ostatni).
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu zapisu.
    //! 
    void async_write_request_headers(request_headers_t & request,const handler_t &handler);
    //! 
    //! @brief Odczytuje wiersz zapytania oraz nag????wki.
    //! 
    //! @param request Dane zapytania oraz nag????wk??w. Je??li header.name jest ustawione na ":", to oznacza koniec nag????wk??w (tak b??dzie ustawiony ostatni).
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu odczytu.
    //! 
    void async_read_request_headers(request_headers_t & request,const handler_t &handler);
    //! 
    //! @brief Zapisuje wiersz odpowiedzi oraz nag????wki.
    //! 
    //! @param request Dane odpowiedzi oraz nag????wk??w. Je??li header.name jest ustawione na ":", to oznacza koniec nag????wk??w (tak poiwnien by?? ustawiony ostatni).
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu zapisu.
    //! 
    void async_write_response_headers(response_headers_t & response,const handler_t &handler);
    //! 
    //! @brief Odczytuje wiersz odpowiedzi oraz nag????wki.
    //! 
    //! @param request Dane odpowiedzi oraz nag????wk??w. Je??li header.name jest ustawione na ":", to oznacza koniec nag????wk??w (tak b??dzie ustawiony ostatni).
    //! @param handler Funkcja, kt??ra ma zosta?? wykonana po zako??czeniu odczytu.
    //! 
    void async_read_response_headers(response_headers_t & response,const handler_t &handler);
};
//===========================================
//! Wska??nik do interfejsu do obs??ugi po????cze??.
typedef std::shared_ptr<message> message_ptr;
//============================================
//! Zwraca interfejs do obs??ugi po????czenia (message).
//! @param iface Wska??nik do interfejsu do obs??ugi po????czenia (string).
//! @returns Wska??nik do interfejsu do obs??ugi po????czenia (message).
message_ptr get(string_ptr iface);
//! Zwraca interfejs do obs??ugi po????czenia (message).
//! @param iface Wska??nik do interfejsu do obs??ugi po????czenia (podstawowy).
//! @returns Wska??nik do interfejsu do obs??ugi po????czenia (message).
message_ptr getMessage(interface_ptr iface);
//! Zwraca interfejs do obs??ugi po????czenia (message).
//! @param iface Wska??nik do interfejsu do obs??ugi po????czenia (string).
//! @returns Wska??nik do interfejsu do obs??ugi po????czenia (message).
message_ptr getMessage(string_ptr iface);
//! Zwraca interfejs (message) do obs??ugi po????czenia (bez SSL).
//! @param socket Gniazdo TCP.
//! @returns Wska??nik do interfejsu (message) do obs??ugi po????czenia.
message_ptr getMessage(::asio::ip::tcp::socket & socket);
//! Zwraca interfejs (message) do obs??ugi po????czenia (bez SSL).
//! @param socket Gniazdo lokalne.
//! @returns Wska??nik do interfejsu (message) do obs??ugi po????czenia.
message_ptr getMessage(::asio::local::stream_protocol::socket & socket);
//! Zwraca interfejs (message) do obs??ugi po????czenia (z SSL).
//! @param socket Gniazdo TCP.
//! @param context Wska??nik do kontekstu po????czenia SSL.
//! @param setSNI Ustawia nazw?? serwera (SNI) - gdy po????czenie jako klient.
//! @returns Wska??nik do interfejsu (message) do obs??ugi po????czenia.
message_ptr getMessage(::asio::ip::tcp::socket & socket,context_ptr & context,const std::string & setSNI="");
//! Zwraca interfejs (message) do obs??ugi po????czenia (z SSL).
//! @param socket Gniazdo lokalne.
//! @param context Wska??nik do kontekstu po????czenia SSL.
//! @param setSNI Ustawia nazw?? serwera (SNI) - gdy po????czenie jako klient.
//! @returns Wska??nik do interfejsu (message) do obs??ugi po????czenia.
message_ptr getMessage(::asio::local::stream_protocol::socket & socket,context_ptr & context,const std::string & setSNI="");
//============================================
}}}
//===========================================
#endif
