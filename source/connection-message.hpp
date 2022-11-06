//! @file
//! @brief Connection (message) module - header file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @date 2021-2022
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2021-2022, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#include "types.hpp"
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
    //! Maksymalny rozmiar linii, gdy odczytywany jest wiersz zapytania, odpowiedzi lub nagłówka.
    std::size_t maxRead=0;
    //! Minimalny rozmiar danych do zapisy, gdy zapisywany jest wiersz zapytania, odpowiedzi lub nagłówka.
    std::size_t minWrite=0;
public:
    //! Typ pomocniczy do generowania wskaźnika.
    typedef  std::enable_shared_from_this<message> enable_shared_t;
    //! Typ - Funkcja do obsługi zapisu lub odczytu.
    typedef std::function<void(const ict::asio::error_code_t&)> handler_t;
    //! Interfejs połączenia (string).
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
private:
    //! 
    //! @brief Zapisuje body wiadomości.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_body(const handler_t &handler);
    //! 
    //! @brief Odczytuje body wiadomości.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_body(const handler_t &handler);
public:
    //! 
    //! @brief Konstruktor.
    //! 
    //! @param i Wskaźnik do interfejsu string.
    //! 
    message(const string_ptr & i):connection(i){}
    //! 
    //! @brief Zapisuje wiersz zapytania.
    //! 
    //! @param request Dane zapytania.
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_request(request_t & request,const handler_t &handler);
    //! 
    //! @brief Odczytuje wiersz zapytania.
    //! 
    //! @param request Dane zapytania.
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_request(request_t & request,const handler_t &handler);
    //! 
    //! @brief Zapisuje wiersz odpowiedzi.
    //! 
    //! @param response Dane odpowiedzi.
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_response(response_t & response,const handler_t &handler);
    //! 
    //! @brief Odczytuje wiersz odpowiedzi.
    //! 
    //! @param response Dane odpowiedzi.
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_response(response_t & response,const handler_t &handler);
    //! 
    //! @brief Zapisuje nagłówek.
    //! 
    //! @param header Dane nagłówka. Jeśli header.name jest ustawione na ":", to oznacza koniec nagłówków.
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_header(header_t & header,const handler_t &handler);
    //! 
    //! @brief Odczytuje nagłówek.
    //! 
    //! @param header Dane nagłówka. Jeśli header.name jest ustawione na ":", to oznacza koniec nagłówków.
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_header(header_t & header,const handler_t &handler);
    //! 
    //! @brief Zapisuje dane body wiadomości.
    //! 
    //! @param data Dane do zapisu.
    //! @param bytesLeft Informacja ile bajtów body zostało do zapisania (aktualizowana).
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_body(std::string & data,std::size_t & bytesLeft,const handler_t &handler);
    //! 
    //! @brief Odczytuje dane body wiadomości.
    //! 
    //! @param data Odczytane dane.
    //! @param bytesLeft Informacja ile bajtów body zostało do odczytania (aktualizowana).
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_body(std::string & data,std::size_t & bytesLeft,const handler_t &handler);
    //! 
    //! @brief Zapisuje nagłówki.
    //! 
    //! @param headers Dane nagłówków. Jeśli header.name jest ustawione na ":", to oznacza koniec nagłówków (tak poiwnien być ustawiony ostatni).
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_headers(headers_t & headers,const handler_t &handler);
    //! 
    //! @brief Odczytuje nagłówki.
    //! 
    //! @param headers Dane nagłówków. Jeśli header.name jest ustawione na ":", to oznacza koniec nagłówków (tak będzie ustawiony ostatni).
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_headers(headers_t & headers,const handler_t &handler);
    //! 
    //! @brief Zapisuje wiersz zapytania oraz nagłówki.
    //! 
    //! @param request Dane zapytania oraz nagłówków. Jeśli header.name jest ustawione na ":", to oznacza koniec nagłówków (tak poiwnien być ustawiony ostatni).
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_request_headers(request_headers_t & request,const handler_t &handler);
    //! 
    //! @brief Odczytuje wiersz zapytania oraz nagłówki.
    //! 
    //! @param request Dane zapytania oraz nagłówków. Jeśli header.name jest ustawione na ":", to oznacza koniec nagłówków (tak będzie ustawiony ostatni).
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_request_headers(request_headers_t & request,const handler_t &handler);
    //! 
    //! @brief Zapisuje wiersz odpowiedzi oraz nagłówki.
    //! 
    //! @param request Dane odpowiedzi oraz nagłówków. Jeśli header.name jest ustawione na ":", to oznacza koniec nagłówków (tak poiwnien być ustawiony ostatni).
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_response_headers(response_headers_t & response,const handler_t &handler);
    //! 
    //! @brief Odczytuje wiersz odpowiedzi oraz nagłówki.
    //! 
    //! @param request Dane odpowiedzi oraz nagłówków. Jeśli header.name jest ustawione na ":", to oznacza koniec nagłówków (tak będzie ustawiony ostatni).
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_response_headers(response_headers_t & response,const handler_t &handler);
    //! Dodaje zadanie do wykonania w ramach ::asio::strand
    //! @param handler Zadanie do wykonania.
    void post(const asio_handler_t &handler);
};
//===========================================
//! Wskaźnik do interfejsu do obsługi połączeń.
typedef std::shared_ptr<message> message_ptr;
//============================================
//! Zwraca interfejs do obsługi połączenia (message).
//! @param iface Wskaźnik do interfejsu do obsługi połączenia (string).
//! @returns Wskaźnik do interfejsu do obsługi połączenia (message).
message_ptr get(string_ptr iface);
//! Zwraca interfejs do obsługi połączenia (message).
//! @param iface Wskaźnik do interfejsu do obsługi połączenia (podstawowy).
//! @returns Wskaźnik do interfejsu do obsługi połączenia (message).
message_ptr getMessage(interface_ptr iface);
//! Zwraca interfejs do obsługi połączenia (message).
//! @param iface Wskaźnik do interfejsu do obsługi połączenia (string).
//! @returns Wskaźnik do interfejsu do obsługi połączenia (message).
message_ptr getMessage(string_ptr iface);
//! Zwraca interfejs (message) do obsługi połączenia (bez SSL).
//! @param socket Gniazdo TCP.
//! @returns Wskaźnik do interfejsu (message) do obsługi połączenia.
message_ptr getMessage(::asio::ip::tcp::socket & socket);
//! Zwraca interfejs (message) do obsługi połączenia (bez SSL).
//! @param socket Gniazdo lokalne.
//! @returns Wskaźnik do interfejsu (message) do obsługi połączenia.
message_ptr getMessage(::asio::local::stream_protocol::socket & socket);
//! Zwraca interfejs (message) do obsługi połączenia (z SSL).
//! @param socket Gniazdo TCP.
//! @param context Wskaźnik do kontekstu połączenia SSL.
//! @param setSNI Ustawia nazwę serwera (SNI) - gdy połączenie jako klient.
//! @returns Wskaźnik do interfejsu (message) do obsługi połączenia.
message_ptr getMessage(::asio::ip::tcp::socket & socket,context_ptr & context,const std::string & setSNI="");
//! Zwraca interfejs (message) do obsługi połączenia (z SSL).
//! @param socket Gniazdo lokalne.
//! @param context Wskaźnik do kontekstu połączenia SSL.
//! @param setSNI Ustawia nazwę serwera (SNI) - gdy połączenie jako klient.
//! @returns Wskaźnik do interfejsu (message) do obsługi połączenia.
message_ptr getMessage(::asio::local::stream_protocol::socket & socket,context_ptr & context,const std::string & setSNI="");
//============================================
}}}
//===========================================
#endif
