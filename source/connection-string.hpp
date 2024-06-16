//! @file
//! @brief Connection (string) module - header file.
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
#ifndef _CONNECTION_STRING_HEADER_HPP
#define _CONNECTION_STRING_HEADER_HPP
//============================================
#include <string>
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
    //! Dodaje zadanie do wykonania w ramach ::asio::strand
    //! @param handler Zadanie do wykonania.
    void post(const asio_handler_t &handler);
};
//===========================================
//! Wskaźnik do interfejsu do obsługi połączeń.
typedef std::shared_ptr<string> string_ptr;
//! Handler do obsługi nowych połączeń
//! @param ec Kod błędu
//! @param interface  Wskaźnik do interfejsu do obsługi połączeń.
typedef std::function<void(const error_code_t&,string_ptr)> string_handler_t;
//============================================
class string2 : public std::enable_shared_from_this<string2>{
private:
    bool is_ok=false;
    //! Bufor do odczytu danych.
    std::string read;
    //! Bufor do zapisu danych.
    std::string write;
    //! Interfejs połączenia
    string_ptr connection;
public:
    //! Typ pomocniczy do generowania wskaźnika.
    typedef  std::enable_shared_from_this<string2> enable_shared_t;
    //! Typ - Funkcja do obsługi zapisu lub odczytu.
    typedef std::function<void(const ict::asio::error_code_t&,std::string&)> handler_t;
public:
    //!
    //! @brief Konstruktor.
    //! 
    //! @param i Wskaźnik do podstawowego interfejsu.
    //! 
    string2(const interface_ptr & i):connection(new string(i)){
        if (connection) if (connection->connection) is_ok=true;
    }
    string2(const string_ptr & i):connection(i){
        if (connection) if (connection->connection) is_ok=true;
    }
    //! 
    //! @brief Funkcja do asynchronicznego zapisu.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu zapisu.
    //! 
    void async_write_string(const handler_t &handler);
    //! 
    //! @brief Funkcja do asynchronicznego odczytu.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana po zakończeniu odczytu.
    //! 
    void async_read_string(const handler_t &handler);
        //! 
    //! @brief Funkcja do operacji na buforze zapisu.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana.
    //! 
    void post_write_string(const handler_t &handler);
    //! 
    //! @brief Funkcja do operacji na buforze odczytu.
    //! 
    //! @param handler Funkcja, która ma zostać wykonana.
    //! 
    void post_read_string(const handler_t &handler);
    //! Funkcja zamyka połączenie.
    void close();
    //! Sprawdza, czy połaczenie jest nadal otwarte.
    bool is_open() const;
    //! Zwraca ilość bajtów oczekujących na odczyt.
    std::size_t available() const;
    //! Anuluje wszystkie asynchroniczne operacje w połączeniu.
    void cancel();
    void cancel(error_code_t& ec);
    //! Zwraca nazwę serwera (SNI).
    //! @returns Nazwa serwera (SNI).
    const std::string & getSNI();
    //! Metadane połączenia
    map_info_t & getInfoMap();
    std::string getInfo() const;
};
//===========================================
//! Wskaźnik do interfejsu do obsługi połączeń.
typedef std::shared_ptr<string2> string2_ptr;
//! Handler do obsługi nowych połączeń
//! @param ec Kod błędu
//! @param interface  Wskaźnik do interfejsu do obsługi połączeń.
typedef std::function<void(const error_code_t&,string2_ptr)> string2_handler_t;
//============================================
}}}
//===========================================
#endif