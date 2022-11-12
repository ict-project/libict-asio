//! @file
//! @brief ASIO conection module - Source file.
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
//============================================
#include <vector>
#include <memory>
#include <map>
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <asio/ssl/context.hpp>
#include "service.h"
#include "connection.h"
//============================================
namespace ict { namespace asio { namespace connection {
//============================================
const static std::string _socket_type_("socket_type");
const static std::string _socket_enc_("socket_enc");
const static std::string _socket_local_("socket_local");
const static std::string _socket_remote_("socket_remote");
const static std::string _tcp_("tcp");
const static std::string _local_("local");
const static std::string _0_("0");
const static std::string _1_("1");
const static std::string _colon_(":");
const static std::string _empty_("");
//============================================
template <class Stream> class ifc : public interface{
protected:
  Stream stream;
  ::asio::io_service::strand strand;
public:
  ifc(Stream & s):stream(std::move(s)),strand(ict::asio::ioService()){}
  template<class Socket> ifc(Socket & s,::asio::ssl::context & c):stream(std::move(s),c),strand(ict::asio::ioService()){}
  void async_write_some(buffer_t& buffer,const handler_t &handler){
    auto self(interface::enable_shared_t::shared_from_this());
    strand.post([self,this,&buffer,handler](){
      stream.async_write_some(::asio::buffer(buffer.data(),buffer.size()),[self,this,handler](const ict::asio::error_code_t& ec,std::size_t s){
        handler(ec,s);
      });
    });
  }
  void async_read_some(buffer_t& buffer,const handler_t &handler){
    auto self(interface::enable_shared_t::shared_from_this());
    strand.post([self,this,&buffer,handler](){
      stream.async_read_some(::asio::buffer(buffer.data(),buffer.size()),[self,this,handler](const ict::asio::error_code_t& ec,std::size_t s){
        handler(ec,s);
      });
    });
  }
  void post(const asio_handler_t &handler){
    strand.post(handler);
  }
};
template <class Stream> class ifc_raw : public ifc<Stream>{
public:
  ifc_raw(Stream & s):ifc<Stream>(s){}
  void close(){
    auto self(interface::enable_shared_t::shared_from_this());
    ifc<Stream>::strand.post([self,this](){
      ifc<Stream>::stream.close();
    });
  }
  bool is_open() const {
    return(ifc<Stream>::stream.is_open());
  }
  std::size_t available() const{
    return(ifc<Stream>::stream.available());
  };
  void cancel(){
    auto self(interface::enable_shared_t::shared_from_this());
    ifc<Stream>::strand.post([self,this](){
      ifc<Stream>::stream.cancel();
    });
  }
  void cancel(error_code_t& ec){
    auto self(interface::enable_shared_t::shared_from_this());
    ifc<Stream>::strand.post([self,this,ec](){
      error_code_t e(ec);
      ifc<Stream>::stream.cancel(e);
    });
  }
};
struct _sni_t{
  std::mutex mutex;
  std::map<::SSL*,std::string> map;
};
static _sni_t & _sni_(){
  static _sni_t s;
  return(s);
}
static int sni_callback(SSL * ssl,int * i, void * arg){
  std::unique_lock<std::mutex> lock(_sni_().mutex);
  const char * name(::SSL_get_servername(ssl,TLSEXT_NAMETYPE_host_name));
  if (name){
    _sni_().map[ssl].assign(name);
  } else {
    _sni_().map[ssl].assign("");
  }
  return(0);
}
template <class Stream> class ifc_ssl : public ifc<Stream>{
private:
  ::asio::ssl::context context;
public:
  template<class Socket> ifc_ssl(Socket & s,const context_ptr & c,const std::string & sni):context(c),ifc<Stream>(s,context){
    std::unique_lock<std::mutex> lock(_sni_().mutex);
    if (sni.size()) ::SSL_set_tlsext_host_name(ifc<Stream>::stream.native_handle(),sni.c_str());
    ::SSL_CTX_set_tlsext_servername_callback(c,sni_callback);
    _sni_().map[ifc<Stream>::stream.native_handle()].assign(sni);
  }
  ~ifc_ssl(){
    std::unique_lock<std::mutex> lock(_sni_().mutex);
    _sni_().map.erase(ifc<Stream>::stream.native_handle());
  }
  void close(){
    auto self(interface::enable_shared_t::shared_from_this());
    ifc<Stream>::strand.post([self,this](){
      ifc<Stream>::stream.shutdown();
      ifc<Stream>::stream.lowest_layer().close();
    });
  }
  bool is_open() const {
    return(ifc<Stream>::stream.lowest_layer().is_open());
  }
  std::size_t available() const{
    return(ifc<Stream>::stream.lowest_layer().available());
  };
  void cancel(){
    auto self(interface::enable_shared_t::shared_from_this());
    ifc<Stream>::strand.post([self,this](){
      ifc<Stream>::stream.lowest_layer().cancel();
    });
  }
  void cancel(error_code_t& ec){
    auto self(interface::enable_shared_t::shared_from_this());
    ifc<Stream>::strand.post([self,this,ec](){
      error_code_t e(ec);
      ifc<Stream>::stream.lowest_layer().cancel(e);
    });
  }
  const std::string & getSNI() {
    std::unique_lock<std::mutex> lock(_sni_().mutex);
    return(_sni_().map.at(ifc<Stream>::stream.native_handle()));
  };
};
//============================================
interface_ptr get(::asio::ip::tcp::socket & socket){
  interface_ptr ptr(std::make_shared<ifc_raw<::asio::ip::tcp::socket>>(socket));
  ptr->info[_socket_type_]=_tcp_;
  ptr->info[_socket_enc_]=_0_;
  try {
    ptr->info[_socket_local_]=socket.local_endpoint().address().to_string()+_colon_+std::to_string(socket.local_endpoint().port());
  } catch(...) {
    ptr->info[_socket_local_]=_empty_;
  }
  try {
    ptr->info[_socket_remote_]=socket.remote_endpoint().address().to_string()+_colon_+std::to_string(socket.remote_endpoint().port());
  } catch(...) {
    ptr->info[_socket_remote_]=_empty_;
  }
  return(ptr);
}
interface_ptr get(::asio::local::stream_protocol::socket & socket){
  interface_ptr ptr(std::make_shared<ifc_raw<::asio::local::stream_protocol::socket>>(socket));
  ptr->info[_socket_type_]=_local_;
  ptr->info[_socket_enc_]=_0_;
  try {
    ptr->info[_socket_local_]=socket.local_endpoint().path();
  } catch(...){
    ptr->info[_socket_local_]=_empty_;
  }
  try {
    ptr->info[_socket_remote_]=socket.remote_endpoint().path();
  } catch(...){
    ptr->info[_socket_remote_]=_empty_;
  }
  return(ptr);
}
interface_ptr get(::asio::ip::tcp::socket & socket,const context_ptr & context,const std::string & setSNI){
  if (context){
    interface_ptr ptr(std::make_shared<ifc_ssl<::asio::ssl::stream<::asio::ip::tcp::socket>>>(socket,context,setSNI));
    ptr->info[_socket_type_]=_tcp_;
    ptr->info[_socket_enc_]=_1_;
    try {
      ptr->info[_socket_local_]=socket.local_endpoint().address().to_string()+_colon_+std::to_string(socket.local_endpoint().port());
    } catch(...) {
      ptr->info[_socket_local_]=_empty_;
    }
    try {
      ptr->info[_socket_remote_]=socket.remote_endpoint().address().to_string()+_colon_+std::to_string(socket.remote_endpoint().port());
    } catch(...){
      ptr->info[_socket_remote_]=_empty_;
    }
    return(ptr);
  }
  return(get(socket));
}
interface_ptr get(::asio::local::stream_protocol::socket & socket,const context_ptr & context,const std::string & setSNI){
  if (context){
    interface_ptr ptr(std::make_shared<ifc_ssl<::asio::ssl::stream<::asio::local::stream_protocol::socket>>>(socket,context,setSNI));
    ptr->info[_socket_type_]=_local_;
    ptr->info[_socket_enc_]=_1_;
    try {
      ptr->info[_socket_local_]=socket.local_endpoint().path();
    } catch(...){
      ptr->info[_socket_local_]=_empty_;
    }
    try {
      ptr->info[_socket_remote_]=socket.remote_endpoint().path();
    } catch(...){
      ptr->info[_socket_remote_]=_empty_;
    }
    return(ptr);
  }
  return(get(socket));
}
//============================================
}}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include "asio.hpp"
#include "connector.hpp"
static int test__connection(ict::asio::connection::context_ptr & s_ctx,ict::asio::connection::context_ptr & c_ctx){
  ict::asio::ioSignal();
  ict::asio::ioRun();
  {
    std::atomic<int> k=4;
    std::string port;
    ::asio::steady_timer t(ict::asio::ioService());
    ict::asio::connection::interface::buffer_t s_write_buffer={'a','b','c','d'};
    ict::asio::connection::interface::buffer_t s_read_buffer;
    ict::asio::connection::interface::buffer_t c_write_buffer={1,2,3,4,5,6,7,8,9,0};
    ict::asio::connection::interface::buffer_t c_read_buffer;
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
    ict::asio::connection::interface_ptr s1c;
    ict::asio::connection::interface_ptr c1c;
    
    s1->async_connection([&](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-100;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      }
      if (ptr) {
        std::cout<<"s1 "<<k<<" "<<ptr->getInfo()<<" "<<ptr->getSNI()<<std::endl;
        s1c=ptr;
        s_read_buffer.resize(c_write_buffer.size()+1);
        ptr->async_write_some(s_write_buffer,[&](const ict::asio::error_code_t& ec,std::size_t s){
          if (ec){
            k=-200;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<"|"<<s<<std::endl;
          } else {
            k--;
            if (s!=s_write_buffer.size()){
              k=-300;
              std::cerr<<__LINE__<<"|"<<s<<"|"<<s_write_buffer.size()<<std::endl;
            }
          }
          if (k<=0) ict::asio::ioService().stop();
        });
        ptr->async_read_some(s_read_buffer,[&](const ict::asio::error_code_t& ec,std::size_t s){
          if (ec){
            k=-400;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<"|"<<s<<std::endl;
          } else {
            k--;
            if (s!=c_write_buffer.size()){
              k=-500;
              std::cerr<<__LINE__<<"|"<<s<<"|"<<c_write_buffer.size()<<std::endl;
            } else if (s_read_buffer.at(0)!=1) {
              k=-600;
              std::cerr<<__LINE__<<"|"<<s_read_buffer.at(0)<<std::endl;
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
        c1c=ptr;
        c_read_buffer.resize(s_write_buffer.size()+1);
        ptr->async_write_some(c_write_buffer,[&](const ict::asio::error_code_t& ec,std::size_t s){
          if (ec){
            k=-800;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<"|"<<s<<std::endl;
          } else {
            k--;
            if (s!=c_write_buffer.size()){
              k=-900;
              std::cerr<<__LINE__<<"|"<<s<<"|"<<c_write_buffer.size()<<std::endl;
            }
          }
          if (k<=0) ict::asio::ioService().stop();
        });
        ptr->async_read_some(c_read_buffer,[&](const ict::asio::error_code_t& ec,std::size_t s){
          if (ec){
            k=-1000;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<"|"<<s<<std::endl;
          } else {
            k--;
            if (s!=s_write_buffer.size()){
              k=-1100;
              std::cerr<<__LINE__<<"|"<<s<<"|"<<s_write_buffer.size()<<std::endl;
            } else if (c_read_buffer.at(0)!='a') {
              k=-1200;
              std::cerr<<__LINE__<<"|"<<c_read_buffer.at(0)<<std::endl;
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
REGISTER_TEST(connection,tc1){
  ict::asio::connection::context_ptr ctx=NULL;
  return(test__connection(ctx,ctx));
}
#endif
//===========================================