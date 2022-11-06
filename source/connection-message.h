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
#ifndef _CONNECTION_MESSAGE_HEADER_H
#define _CONNECTION_MESSAGE_HEADER_H
//============================================
#include "connection-message.hpp"
#include "connection-string.h"
//============================================
namespace ict { namespace asio { namespace connection {
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
