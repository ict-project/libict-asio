//! @file
//! @brief ASIO module - Source file.
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
//============================================
#include <vector>
#include <memory>
#include <thread>
#include <asio.hpp>
#include <asio/ssl.hpp>
#include "asio.hpp"
#include "service.hpp"
//============================================
namespace ict { namespace asio {
//============================================
typedef std::vector<std::thread> vector_thread_t;
typedef std::unique_ptr<vector_thread_t> vector_thread_ptr;
//============================================
vector_thread_ptr & ioThreads(){
  static vector_thread_ptr ptr;
  return(ptr);
}
void ioSignal(const signal_handler_t & handler){
  static ::asio::signal_set signals(ioService(),SIGINT,SIGTERM);
  signals.clear();
  signals.async_wait([handler](const ::asio::error_code& error,int signal_number){
    if (!error) {
      switch(signal_number){
        case SIGINT: case SIGTERM:{
          if (handler) handler(error,signal_number);
        } break;
      }
    }
  });
}
void ioSignal(){
  ioSignal([](const ::asio::error_code& error,int signal_number){
    ioService().stop();
  });
}
void ioServiceRun(){
  ioService().run();
}
void ioServicePost(const asio_handler_t &f){
  ioService().post(f);
}
void ioRun(const asio_handler_t &f){
  static const unsigned int t(std::thread::hardware_concurrency());
  if (!ioThreads()){
    ioService().restart();
    ioThreads().reset(new vector_thread_t);
    for (;ioThreads()->size()<t;){
      ioThreads()->emplace_back(f);
    }
  }
}
void ioJoin(){
  unsigned int t(0);
  if (ioThreads()) {
    for(vector_thread_t::iterator it=ioThreads()->begin();it!=ioThreads()->end();++it){
      it->join();
      ++t;
    }
    ioThreads().reset(nullptr);
  }
}
void ioRunJoin(const asio_handler_t &f){
  ioRun(f);
  ioJoin();
}
void ioStop(){
  ioService().stop();
}
//============================================
}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
REGISTER_TEST(asio,tc1){
  ict::asio::ioSignal();
  ict::asio::ioRun();
  ict::asio::ioServicePost([](){
    ict::asio::ioService().stop();
  });
  ict::asio::ioJoin();
  return(0);
}
#endif
//===========================================