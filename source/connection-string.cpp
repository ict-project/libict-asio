//! @file
//! @brief Connection (string) module - Source file.
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
//============================================
#include "asio.hpp"
#include "service.h"
#include "connection-string.h"
//============================================
namespace ict { namespace asio { namespace connection {
//============================================
static const std::size_t max(0x10000);
void string::async_write_string(std::string & buffer,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    if (buffer.empty()){
        ioServicePost([self,handler](){
            ict::asio::error_code_t ec(ENODATA,std::generic_category());
            handler(ec);
        });
    } else if (connection){
        connection->post([this,self,handler,&buffer](){
          std::size_t size=(max<buffer.size())?max:buffer.size();
          write.resize(size);
          buffer.copy((char*)write.data(),size,0);
          connection->async_write_some(write,[this,self,handler,&buffer](const ict::asio::error_code_t& ec,std::size_t s){
            write.clear();
            buffer.erase(0,s);
            handler(ec);
        });
      });
    } else {
        ioServicePost([self,handler](){
            ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
            handler(ec);
        });
    }
}
void string::async_read_string(std::string & buffer,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    if (buffer.max_size()<(buffer.size()+max)){
        ioServicePost([self,handler](){
            ict::asio::error_code_t ec(ENOBUFS,std::generic_category());
            handler(ec);
        });
    } else if (connection){
        connection->post([this,self,handler,&buffer](){
          std::size_t size=max;
          read.resize(size);
          connection->async_read_some(read,[this,self,handler,&buffer](const ict::asio::error_code_t& ec,std::size_t s){
            buffer.append((char*)read.data(),s);
            read.clear();
            handler(ec);
          });
        });
    } else {
        ioServicePost([self,handler](){
            ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
            handler(ec);
        });
    }
}
void string::post(const asio_handler_t &handler){
  if (connection){
    connection->post(handler);
  }
}
string_ptr get(interface_ptr iface){
    return string_ptr(std::make_shared<string>(iface));
}
string_ptr getString(interface_ptr iface){
    return get(iface);
}
string_ptr getString(::asio::ip::tcp::socket & socket){
    return get(get(socket));
}
string_ptr getString(::asio::local::stream_protocol::socket & socket){
    return get(get(socket));
}
string_ptr getString(::asio::ip::tcp::socket & socket,context_ptr & context,const std::string & setSNI){
    return get(get(socket,context,setSNI));
}
string_ptr getString(::asio::local::stream_protocol::socket & socket,context_ptr & context,const std::string & setSNI){
    return get(get(socket,context,setSNI));
}
//============================================
}}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include "asio.hpp"
#include "connector.hpp"

static const std::string server_example="'a','b','c','d'";
static const std::string client_example="1,2,3,4,5,6,7,8,9,0";

static int test__connection(ict::asio::context_ptr & s_ctx,ict::asio::context_ptr & c_ctx){
  ict::asio::ioSignal();
  ict::asio::ioRun();
  {
    std::atomic<int> k=4;
    std::string port;
    ::asio::steady_timer t(ict::asio::ioService());
    std::string s_write_buffer=server_example;
    std::string s_read_buffer;
    std::string c_write_buffer=client_example;
    std::string c_read_buffer;
    srand(time(NULL));

    t.expires_from_now(std::chrono::seconds(60));
    t.async_wait(
      [](const ict::asio::error_code_t& ec){
        ict::asio::ioService().stop();
      }
    );

    port="300"+std::to_string(rand()%90+10);
    std::cout<<port<<std::endl;
    ict::asio::connector::interface_ptr s1(ict::asio::connector::get("localhost",port,true,s_ctx));
    ict::asio::connector::interface_ptr c1(ict::asio::connector::get("localhost",port,false,c_ctx,"c1"));
    ict::asio::connection::string_ptr s1c;
    ict::asio::connection::string_ptr c1c;
    
    s1->async_connection([&](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-100;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      }
      if (ptr) {
        std::cout<<"s1 "<<k<<" "<<ptr->getInfo()<<" "<<ptr->getSNI()<<std::endl;
        s1c=ict::asio::connection::getString(ptr);
        s1c->async_write_string(s_write_buffer,[&](const ict::asio::error_code_t& ec){
          if (ec){
            k=-200;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
          } else {
            k--;
          }
          if (k<=0) ict::asio::ioService().stop();
        });
        s1c->async_read_string(s_read_buffer,[&](const ict::asio::error_code_t& ec){
          if (ec){
            k=-400;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
          } else {
            k--;
            if (s_read_buffer!=client_example){
              k=-500;
              std::cerr<<__LINE__<<"|"<<s_read_buffer<<"|"<<client_example<<std::endl;
            }
          }
          if (k<=0) ict::asio::ioService().stop();
        });
      }
    });

    usleep(5000);
    c1->async_connection([&](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-700;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      }
      if (ptr) {
        c1c=ict::asio::connection::getString(ptr);
        c1c->async_write_string(c_write_buffer,[&](const ict::asio::error_code_t& ec){
          if (ec){
            k=-800;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
          } else {
            k--;
          }
          if (k<=0) ict::asio::ioService().stop();
        });
        c1c->async_read_string(c_read_buffer,[&](const ict::asio::error_code_t& ec){
          if (ec){
            k=-1000;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
          } else {
            k--;
            if (c_read_buffer!=server_example){
              k=-1100;
              std::cerr<<__LINE__<<"|"<<c_read_buffer<<"|"<<server_example<<std::endl;
            }
          }
          if (k<=0) ict::asio::ioService().stop();
        });
      }
    });

    ict::asio::ioJoin();
    if (k) return(k);
  }
  return(0);
}
REGISTER_TEST(connection_string,tc1){
  ict::asio::context_ptr ctx=NULL;
  return(test__connection(ctx,ctx));
}
#endif
//===========================================
