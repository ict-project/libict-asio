//! @file
//! @brief Connection (string) module - header file.
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
#ifndef _CONNECTION_STRING_HEADER
#define _CONNECTION_STRING_HEADER
//============================================
#include <string>
#include "types.hpp"
#include "connection.hpp"
//============================================
namespace ict { namespace asio { namespace connection {
//===========================================
class string : public std::enable_shared_from_this<string>{
private:
    //! Bufor do odczytu danych.
    ict::asio::connection::interface::buffer_t read;
    //! Bufor do zapisu danych.
    ict::asio::connection::interface::buffer_t write;
public:
    //! Typ pomocniczy do generowania wskaźnika.
    typedef  std::enable_shared_from_this<string> enable_shared_t;
    //! Typ - Funkcja do obsługi zapisu lub odczytu.
    typedef std::function<void(const ict::asio::error_code_t&)> handler_t;
    //! Interfejs połączenia
    interface_ptr connection;
public:
    //!
    //! @brief Konstruktor.
    //! 
    //! @param i Wskaźnik do podstawowego interfejsu.
    //! 
    string(const interface_ptr & i):connection(i){}
    //! 
    //! @brief Funkcja do asynchronicznego zapisu.
    //! 
    //! @param buffer Bufor zapisu.
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_string(std::string & buffer,const handler_t &handler);
    //! 
    //! @brief Funkcja do asynchronicznego odczytu.
    //! 
    //! @param buffer Bufor odczytu.
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_string(std::string & buffer,const handler_t &handler);
};
//===========================================
//! Wskaźnik do interfejsu do obsługi połączeń.
typedef std::shared_ptr<string> string_ptr;
//===========================================
//! Zwraca interfejs do obsługi połączenia (string).
//! @param iface Wskaźnik do interfejsu do obsługi połączenia (podstawowy).
//! @returns Wskaźnik do interfejsu do obsługi połączenia (string).
string_ptr get(interface_ptr iface);
//! Zwraca interfejs do obsługi połączenia (string).
//! @param iface Wskaźnik do interfejsu do obsługi połączenia (podstawowy).
//! @returns Wskaźnik do interfejsu do obsługi połączenia (string).
string_ptr getString(interface_ptr iface);
//! Zwraca interfejs (string) do obsługi połączenia (bez SSL).
//! @param socket Gniazdo TCP.
//! @returns Wskaźnik do interfejsu (string) do obsługi połączenia.
string_ptr getString(::asio::ip::tcp::socket & socket);
//! Zwraca interfejs (string) do obsługi połączenia (bez SSL).
//! @param socket Gniazdo lokalne.
//! @returns Wskaźnik do interfejsu (string) do obsługi połączenia.
string_ptr getString(::asio::local::stream_protocol::socket & socket);
//! Zwraca interfejs (string) do obsługi połączenia (z SSL).
//! @param socket Gniazdo TCP.
//! @param context Wskaźnik do kontekstu połączenia SSL.
//! @param setSNI Ustawia nazwę serwera (SNI) - gdy połączenie jako klient.
//! @returns Wskaźnik do interfejsu (string) do obsługi połączenia.
string_ptr getString(::asio::ip::tcp::socket & socket,context_ptr & context,const std::string & setSNI="");
//! Zwraca interfejs (string) do obsługi połączenia (z SSL).
//! @param socket Gniazdo lokalne.
//! @param context Wskaźnik do kontekstu połączenia SSL.
//! @param setSNI Ustawia nazwę serwera (SNI) - gdy połączenie jako klient.
//! @returns Wskaźnik do interfejsu (string) do obsługi połączenia.
string_ptr getString(::asio::local::stream_protocol::socket & socket,context_ptr & context,const std::string & setSNI="");
//============================================
}}}
//===========================================
#endif