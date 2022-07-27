//! @file
//! @brief ASIO module - header file.
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
#ifndef _ASIO__HEADER
#define _ASIO__HEADER
//============================================
#include <functional>
#include <map>
#include <string>
#include <asio/io_service.hpp>
//============================================
namespace ict { namespace asio {
//============================================
//! Typ - metadane obiektu
typedef std::map<std::string,std::string> map_info_t;
//! Kod błędu
typedef std::error_code error_code_t;
//! Handler do obsługi sygnałów
//! @param ec Kod błędu
//! @param signal Numer sygnału
typedef std::function<void(const error_code_t&,int)> signal_handler_t;
//===========================================
//! Dostęp do ::asio::io_service
::asio::io_service & ioService();
//! Ustawienie obsługi sygnałów
//! @param handler Funkcja do wykonania
void ioSignal(const signal_handler_t & handler);
//! Ustawienie obsługi sygnałów
void ioSignal();
//! Uruchamia ::asio::io_service::run() w wielu osobnych wątkach
void ioRun();
//! Oczekuje na zakończenie ::asio::io_service::run() w wielu osobnych wątkach
void ioJoin();
//! Uruchamia i oczekuje na zakończenie ::asio::io_service::run() w wielu osobnych wątkach
void ioRunJoin();
//! Wykonuje ::asio::io_service::stop()
void ioStop();
//============================================
}}
//===========================================
#endif
