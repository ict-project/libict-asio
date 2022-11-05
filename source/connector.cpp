//! @file
//! @brief Connector module - source file.
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
#include <asio.hpp>
#include <asio/ssl.hpp>
#include "connector.hpp"
#include "resolver.hpp"
//============================================
namespace ict { namespace asio { namespace connector {
//============================================
const static std::string _connector_type_("connector_type");
const static std::string _connector_host_("connector_host");
const static std::string _connector_port_("connector_port");
const static std::string _connector_path_("connector_path");
const static std::string _connector_server_("connector_server");
const static std::string _tcp_("tcp");
const static std::string _local_("local");
const static std::string _1_("1");
const static std::string _0_("0");
const static std::string _empty_("");
const static std::string _connector_sni_("connector_sni");
//============================================
namespace server {
//============================================
template<class Endpoint,class Acceptor> bool doBindOne(const Endpoint & ep,Acceptor & a,error_code_t & ec){
  a.close();
  a.open(ep.protocol(),ec);
  if (ec) {
    return(false);
  } else {
    a.bind(ep,ec);
    if (ec) {
      return(false);
    } else {
      a.listen(::asio::socket_base::max_connections,ec);
      if (ec) {
        return(false);
      }
    }
  }
  return(true);
}
template<class Endpoints,class Acceptor> bool doBind(const Endpoints & eps,Acceptor & a,error_code_t & ec){
  if (eps) for (const auto & ep : eps->endpoint){
    if (doBindOne(ep,a,ec)) return(true);
  }
  return(false);
}
//============================================
}
//============================================
template <class Socket> class BasicConnector: public interface {
protected:
  bool ready=false;
  bool error=false;
  ict::asio::connection::context_ptr context;
  ::asio::io_service::strand strand;
public:
  BasicConnector(const ict::asio::connection::context_ptr & c):context(c),strand(ict::asio::ioService()){
  }
  bool is_error() const{
    return(error);
  }
};
template <class Socket,class Acceptor> class ServerConnector: public BasicConnector<Socket> {
private:
  Acceptor a;
  Socket s;
private:
  void accept(const ict::asio::connection::connection_handler_t & handler){
    auto self(interface::enable_shared_t::shared_from_this());
    a.async_accept(
      s,
      [this,self,handler](error_code_t ec){
        if (ec) {
          ict::asio::connection::interface_ptr empty;
          handler(ec,empty);
        } else {
          ict::asio::connection::interface_ptr ptr(
            BasicConnector<Socket>::context?
              ict::asio::connection::get(s,BasicConnector<Socket>::context,interface::info.at(_connector_sni_)):
              ict::asio::connection::get(s)
          );
          for (ict::asio::map_info_t::const_iterator it=interface::info.begin();it!=interface::info.end();++it){
            ptr->info[it->first]=it->second;
          }
          handler(ec,ptr);
        }
      }
    );
  }
  void prepare(::asio::ip::tcp::socket & socket,const ict::asio::connection::connection_handler_t & handler){
    auto self(interface::enable_shared_t::shared_from_this());
    ict::asio::resolver::get(
      interface::info.at(_connector_host_),
      interface::info.at(_connector_port_),
      [this,self,handler](ict::asio::resolver::tcp_endpoint_info_ptr & ep,const error_code_t&ec){
        if (ec){
          BasicConnector<Socket>::error=true;
        } else {
          error_code_t ec;
          server::doBind(ep,a,ec);
          if (ec){
            BasicConnector<Socket>::error=true;
          } else {
            accept(handler);
          }
        }
      }
    );
  }
  void prepare(::asio::local::stream_protocol::socket & socket,const ict::asio::connection::connection_handler_t & handler){
    auto self(interface::enable_shared_t::shared_from_this());
    ict::asio::resolver::get(
      interface::info.at(_connector_path_),
      [this,self,handler](ict::asio::resolver::stream_endpoint_info_ptr & ep,const error_code_t&ec){
        if (ec){
          BasicConnector<Socket>::error=true;          
        } else {
          error_code_t ec;
          server::doBind(ep,a,ec);
          if (ec){
            BasicConnector<Socket>::error=true;
          } else {
            accept(handler);
          }
        }
      }
    );
  }
  void unlink_path(){
    if ((interface::info.at(_connector_type_)==_local_)&&(interface::info.at(_connector_server_)==_1_)) {
      if (interface::info.at(_connector_path_).size()) {
        unlink(interface::info.at(_connector_path_).c_str());
      }
    }
  }
public:
  ServerConnector(const ict::asio::connection::context_ptr & c):BasicConnector<Socket>(c),a(ict::asio::ioService()),s(ict::asio::ioService()){}
  ~ServerConnector(){
    unlink_path();
  }
  void close(){
    auto self(interface::enable_shared_t::shared_from_this());
    BasicConnector<Socket>::strand.post([this,self](){
      a.close();
      s.close();
      unlink_path();
    });
  }
  bool is_open() const{
    return(BasicConnector<Socket>::error?false:a.is_open());
  }
  void cancel(){
    auto self(interface::enable_shared_t::shared_from_this());
    BasicConnector<Socket>::strand.post([this,self](){
      a.cancel();
    });
  }
  void cancel(error_code_t& ec){
    auto self(interface::enable_shared_t::shared_from_this());
    BasicConnector<Socket>::strand.post([this,self,ec](){
      error_code_t e(ec);
      a.cancel(e);
    });
  }
  void async_connection(const ict::asio::connection::connection_handler_t & handler){
    auto self(interface::enable_shared_t::shared_from_this());
    BasicConnector<Socket>::strand.post([this,self,handler](){
      if (BasicConnector<Socket>::error) return;
      if (BasicConnector<Socket>::ready){
        accept(handler);
      } else {
        prepare(s,handler);
        BasicConnector<Socket>::ready=true;
      }
    });
  }
};
template <class Socket,class Endpoint> class ClientConnector: public BasicConnector<Socket> {
private:
  Endpoint e;
  Socket s;
  std::size_t i=0;
  ::asio::steady_timer t;
  error_code_t le;
private:
  void connect(const ict::asio::connection::connection_handler_t & handler){
    auto self(interface::enable_shared_t::shared_from_this());
    if (e) if (i<e->endpoint.size()){
      t.expires_from_now(std::chrono::seconds(60));
      t.async_wait(
        [this,self](const error_code_t& ec){
          if (!ec){
            error_code_t e(ETIMEDOUT,std::generic_category());
            le=e;
            s.close();
            t.cancel();
          }
        }
      );
      s.async_connect(
        e->endpoint.at(i),
        [this,self,handler](const error_code_t & ec){
          t.cancel();
          if (ec){
            le=ec;
            i++;
            connect(handler);
          } else {
            ict::asio::connection::interface_ptr ptr(
              BasicConnector<Socket>::context?
                ict::asio::connection::get(s,BasicConnector<Socket>::context,interface::info.at(_connector_sni_)):
                ict::asio::connection::get(s)
            );
            for (ict::asio::map_info_t::const_iterator it=interface::info.begin();it!=interface::info.end();++it){
              ptr->info[it->first]=it->second;
            }
            handler(ec,ptr);
          }
        }
      );
      return;
    }
    ict::asio::ioService().post([this,self,handler](){
      ict::asio::connection::interface_ptr empty;
      BasicConnector<Socket>::error=true;
      handler(le,empty);
    });
  }
  void prepare(::asio::ip::tcp::socket & socket,const ict::asio::connection::connection_handler_t & handler){
    auto self(interface::enable_shared_t::shared_from_this());
    ict::asio::resolver::get(
      interface::info.at(_connector_host_),
      interface::info.at(_connector_port_),
      [this,self,handler](ict::asio::resolver::tcp_endpoint_info_ptr & ep,const error_code_t&ec){
        if (ec){
          BasicConnector<Socket>::error=true;
          le=ec;
        } else {
          e=ep;
          i=0;
          connect(handler);
        }
      }
    );
  }
  void prepare(::asio::local::stream_protocol::socket & socket,const ict::asio::connection::connection_handler_t & handler){
    auto self(interface::enable_shared_t::shared_from_this());
    ict::asio::resolver::get(
      interface::info.at(_connector_path_),
      [this,self,handler](ict::asio::resolver::stream_endpoint_info_ptr & ep,const error_code_t&ec){
        if (ec){
          BasicConnector<Socket>::error=true;
          le=ec;          
        } else {
          e=ep;
          i=0;
          connect(handler);
        }
      }
    );
  }
public:
  ClientConnector(const ict::asio::connection::context_ptr & c):BasicConnector<Socket>(c),s(ict::asio::ioService()),t(ict::asio::ioService()){}
  ~ClientConnector(){}
  void close(){
    auto self(interface::enable_shared_t::shared_from_this());
    BasicConnector<Socket>::strand.post([this,self](){
      s.close();
    });
  }
  bool is_open() const{
    return(BasicConnector<Socket>::error?false:true);
  }
  void cancel(){
    auto self(interface::enable_shared_t::shared_from_this());
      BasicConnector<Socket>::strand.post([this,self](){
      s.cancel();
    });
  }
  void cancel(error_code_t& ec){
    auto self(interface::enable_shared_t::shared_from_this());
    BasicConnector<Socket>::strand.post([this,self,ec](){
      error_code_t e(ec);
      s.cancel(e);
    });
  }
  void async_connection(const ict::asio::connection::connection_handler_t &handler){
    auto self(interface::enable_shared_t::shared_from_this());
    BasicConnector<Socket>::strand.post([this,self,handler](){
      BasicConnector<Socket>::error=false;
      prepare(s,handler);
      BasicConnector<Socket>::ready=true;
    });
  }
};
//============================================
interface_ptr get(const std::string & host,const std::string & port,bool server){
  ict::asio::connection::context_ptr empty;
  return(get(host,port,empty,server));
}
interface_ptr get(const std::string & host,const std::string & port,const ict::asio::connection::context_ptr & context,bool server,const std::string & setSNI){
  interface_ptr ptr;
  if (server){
    ptr=std::make_shared<ServerConnector<::asio::ip::tcp::socket,::asio::ip::tcp::acceptor>>(context);
  } else {
    ptr=std::make_shared<ClientConnector<::asio::ip::tcp::socket,ict::asio::resolver::tcp_endpoint_info_ptr>>(context);
  }
  ptr->info[_connector_type_]=_tcp_;
  ptr->info[_connector_host_]=host;
  ptr->info[_connector_port_]=port;
  ptr->info[_connector_path_]=_empty_;
  ptr->info[_connector_server_]=server?_1_:_0_;
  ptr->info[_connector_sni_]=setSNI;
  return(ptr);
}
interface_ptr get(const std::string & path,bool server){
  ict::asio::connection::context_ptr empty;
  return(get(path,empty,server));
}
interface_ptr get(const std::string & path,const ict::asio::connection::context_ptr & context,bool server,const std::string & setSNI){
  interface_ptr ptr;
  if (server){
    ptr=std::make_shared<ServerConnector<::asio::local::stream_protocol::socket,::asio::local::stream_protocol::acceptor>>(context);
  } else {
    ptr=std::make_shared<ClientConnector<::asio::local::stream_protocol::socket,ict::asio::resolver::stream_endpoint_info_ptr>>(context);
  }
  ptr->info[_connector_type_]=_local_;
  ptr->info[_connector_host_]=_empty_;
  ptr->info[_connector_port_]=_empty_;
  ptr->info[_connector_path_]=path;
  ptr->info[_connector_server_]=server?_1_:_0_;
  ptr->info[_connector_sni_]=setSNI;
  return(ptr);
}
//============================================
}}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include <atomic>
REGISTER_TEST(connector,tc1){
  ict::asio::ioSignal();
  ict::asio::ioRun();
  {
    std::atomic<int> k=8;
    std::string port;
    ict::asio::connection::context_ptr ctx(new ::asio::ssl::context(::asio::ssl::context::tls));
    ::asio::steady_timer t(ict::asio::ioService());
    srand(time(NULL));

    t.expires_from_now(std::chrono::seconds(60));
    t.async_wait(
      [](const ict::asio::error_code_t& ec){
        ict::asio::ioService().stop();
      }
    );

    port="300"+std::to_string(rand()%90+10);
    std::cout<<port<<std::endl;
    ict::asio::connector::interface_ptr s1(ict::asio::connector::get("localhost",port,true));
    ict::asio::connector::interface_ptr c1(ict::asio::connector::get("localhost",port,false));

    port="300"+std::to_string(rand()%90+10);
    std::cout<<port<<std::endl;
    ict::asio::connector::interface_ptr s2(ict::asio::connector::get("/tmp/test-connector-"+port,true));
    ict::asio::connector::interface_ptr c2(ict::asio::connector::get("/tmp/test-connector-"+port,false));

    port="300"+std::to_string(rand()%90+10);
    std::cout<<port<<std::endl;
    ict::asio::connector::interface_ptr s3(ict::asio::connector::get("localhost",port,ctx,true));
    ict::asio::connector::interface_ptr c3(ict::asio::connector::get("localhost",port,ctx,false,"c3"));
    
    port="300"+std::to_string(rand()%90+10);
    std::cout<<port<<std::endl;
    ict::asio::connector::interface_ptr s4(ict::asio::connector::get("/tmp/test-connector-"+port,ctx,true));
    ict::asio::connector::interface_ptr c4(ict::asio::connector::get("/tmp/test-connector-"+port,ctx,false,"c4"));
    
    s1->async_connection([&k](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-100;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      } else {
        k--;
      }
      if (ptr) std::cout<<"s1 "<<k<<" "<<ptr->getInfo()<<std::endl;
      if (k<=0) ict::asio::ioService().stop();
    });
    s2->async_connection([&k](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-200;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      } else {
        k--;
      }
      if (ptr) std::cout<<"s2 "<<k<<" "<<ptr->getInfo()<<std::endl;
      if (k<=0) ict::asio::ioService().stop();
    });
    s3->async_connection([&k](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-300;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      } else {
        k--;
      }
      if (ptr) std::cout<<"s3 "<<k<<" "<<ptr->getInfo()<<" "<<ptr->getSNI()<<std::endl;
      if (k<=0) ict::asio::ioService().stop();
    });
    s4->async_connection([&k](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-400;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      } else {
        k--;
      }
      if (ptr) std::cout<<"s4 "<<k<<" "<<ptr->getInfo()<<" "<<ptr->getSNI()<<std::endl;
      if (k<=0) ict::asio::ioService().stop();
    });
    usleep(5000);
    c1->async_connection([&k](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-500;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      } else {
        k--;
      }
    });
    usleep(5000);
    c2->async_connection([&k](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-600;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      } else {
        k--;
      }
    });
    usleep(5000);
    c3->async_connection([&k](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-700;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      } else {
        k--;
      }
    });
    usleep(5000);
    c4->async_connection([&k](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-800;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      } else {
        k--;
      }
    });

    ict::asio::ioJoin();
    if (k) return(k);
  }
  return(0);
}
#endif
//===========================================
