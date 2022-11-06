//! @file
//! @brief DNS resolver module - Source file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 2.0
//! @date 2016-2021
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2016-2021, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#include <chrono>
#include <asio.hpp>
#include <asio/ssl.hpp>
#include "asio.hpp"
#include "resolver.h"
#include "service.h"
//============================================
namespace ict { namespace asio { namespace resolver {
//============================================
class TcpResolver : public std::enable_shared_from_this<TcpResolver>{
private:
  typedef std::enable_shared_from_this<TcpResolver> enable_shared_t;
  //! Asynchroniczne rozwiązywanie nazw DNS.
  ::asio::ip::tcp::resolver r;
  //! Zapytanie  DNS.
  ::asio::ip::tcp::resolver::query q;
  //! Timer dla zapytania DNS.
  ::asio::steady_timer d;
  //! Handler 
  const tcp_handler_t h;
  //! Czy wykonano handler
  bool done=false;
  //! Strand dla handler
  ::asio::io_service::strand s;
  //! Endpoint
  tcp_endpoint_info_ptr endpoint;
  //! Wykonanie handler
  void doHandler(const error_code_t& ec){
    auto self(enable_shared_t::shared_from_this());
    s.post([this,self,ec](){
      if (!done){
        done=true;
        if (h) {
          h(endpoint,ec);
        }
      }
    });
  }
public:
  TcpResolver(const std::string & host,const std::string & port,const tcp_handler_t& handler):r(ict::asio::ioService()),q(host,port),d(ict::asio::ioService()),s(ict::asio::ioService()),h(handler){
    endpoint.reset(new tcp_endpoint_info);
    if (endpoint){
      endpoint->host=host;
      endpoint->port=port;
    }
  }
  void doIt(){
    auto self(enable_shared_t::shared_from_this());
    d.expires_from_now(std::chrono::seconds(60));
    d.async_wait(
      [this,self](const error_code_t& ec){
        r.cancel();
        if (ec){
          if (ec.value()==ECANCELED){
          } else {
            doHandler(ec);
          }
        } else {
          static const error_code_t e(ETIME,std::generic_category());
          r.cancel();
          doHandler(e);
        }
      }
    );
    r.async_resolve(
      q,
      [this,self](const error_code_t& ec,::asio::ip::tcp::resolver::iterator ei){
        d.cancel();
        if (ec){
          if (ec.value()==ECANCELED){
          } else {
            doHandler(ec);
          }
        } else {
          if (endpoint) for (;ei!=::asio::ip::tcp::resolver::iterator();++ei) endpoint->endpoint.push_back(ei->endpoint());
          doHandler(ec);
        }
      }
    );
  }
};
//============================================
void get(const std::string & host,const std::string & port,const tcp_handler_t & handler){
  if ((host=="")||(host=="0.0.0.0")||(host=="[::]")){
    ioServicePost([=](){
      error_code_t ec;
      tcp_endpoint_info_ptr endpoint(new tcp_endpoint_info);
      if (endpoint){
        endpoint->host=host;
        endpoint->port=port;
        endpoint->endpoint.emplace_back(::asio::ip::tcp::v6(),std::stol(port));
      }
      if (handler){
        handler(endpoint,ec);
      } else {
      }
    });
  } else {
    auto resolver=std::make_shared<TcpResolver>(host,port,handler);
    resolver->doIt();
  }
}
void get(const std::string & path,const stream_handler_t & handler){
  ioServicePost([path,handler](){
    error_code_t ec;
    stream_endpoint_info_ptr endpoint(new stream_endpoint_info);
    if (endpoint){
      endpoint->path=path;
      endpoint->endpoint.emplace_back(path);
    }
    handler(endpoint,ec);
  });
}
//============================================
}}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include "asio.hpp"
REGISTER_TEST(resolver,tc1){
  bool err=true;
  ict::asio::ioSignal();
  ict::asio::ioRun();
  ict::asio::resolver::get("","80",[&](const ict::asio::resolver::tcp_endpoint_info_ptr& te,const ict::asio::error_code_t& ec){
    std::cout<<"ict::asio::resolver::get - ";
    if (ec){
      err=true;
      std::cout<<"ERR: "<<ec;
    } else {
      err=false;
      std::cout<<"OK: ("<<te->endpoint.size()<<") ";
      for (const auto & ep : te->endpoint) std::cout<<ep<<",";
    }
    std::cout<<std::endl;
    ict::asio::ioStop();
  });
  ict::asio::ioJoin();
  if (err) return(-1);
  return(0);
}
REGISTER_TEST(resolver,tc2){
  bool err=true;
  ict::asio::ioSignal();
  ict::asio::ioRun();
  ict::asio::resolver::get("wp.pl","80",[&](const ict::asio::resolver::tcp_endpoint_info_ptr& te,const ict::asio::error_code_t& ec){
    std::cout<<"ict::asio::resolver::get - ";
    if (ec){
      err=true;
      std::cout<<"ERR: "<<ec;
    } else {
      err=false;
      std::cout<<"OK: ("<<te->endpoint.size()<<") ";
      for (const auto & ep : te->endpoint) std::cout<<ep<<",";
    }
    std::cout<<std::endl;
    ict::asio::ioStop();
  });
  ict::asio::ioJoin();
  if (err) return(-1);
  return(0);
}
REGISTER_TEST(resolver,tc3){
  bool err=true;
  ict::asio::ioSignal();
  ict::asio::ioRun();
  ict::asio::resolver::get("/tmp/test.stream",[&](const ict::asio::resolver::stream_endpoint_info_ptr& se,const ict::asio::error_code_t& ec){
    std::cout<<"ict::asio::resolver::get - ";
    if (ec){
      err=true;
      std::cout<<"ERR: "<<ec;
    } else {
      err=false;
      std::cout<<"OK: ("<<se->endpoint.size()<<") ";
      for (const auto & ep : se->endpoint) std::cout<<ep<<",";
    }
    std::cout<<std::endl;
    ict::asio::ioStop();
  });
  ict::asio::ioJoin();
  if (err) return(-1); 
  return(0);
}
#endif
//===========================================
