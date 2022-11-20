//! @file
//! @brief Broker module - source file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @date 2022
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2022, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#include "broker.hpp"
#include "service.h"
#include <string>
#include <queue>
#include <map>
#include <set>
#include <chrono>
#include <asio.hpp>
#include "connector.hpp"
//============================================
namespace ict { namespace asio { namespace broker {
//============================================
const static std::string _colon_(":");
const static std::string _empty_("");
const static std::string _client_("client");
const static std::string _server_("server");
//============================================
class implementation : public interface{
private:
    ict::asio::connection::message_ptr message;
    ict::asio::connection::interface_ptr connection;
    std::string key;
    typedef std::queue<broker_handler_t> users_t;
    typedef std::queue<ict::asio::connection::message_ptr> connections_t;
    struct pool1_t {
        users_t users;
        connections_t connections;
        std::chrono::steady_clock::time_point lastUsage=std::chrono::steady_clock::now();
    };
    typedef  std::map<std::string,pool1_t> map1_t;
    struct pool2_t {
        ict::asio::connector::interface_ptr connector;
        map1_t pool;
    };
    typedef  std::map<std::string,pool2_t> map2_t;
    static map2_t & map(){
        static map2_t m;
        return m;
    }
    static void post2(asio_handler_t handler){
        static ::asio::io_service::strand strand(ioService());
        strand.post(handler);
    }
    static void put(const ict::asio::connection::message_ptr & message,const std::string & key,const std::string & sni){
        post2([message,key,sni](){
            map2_t & map(implementation::map());
            if (map.count(key)){
                std::string _sni(map.at(key).pool.count(sni)?sni:_empty_);
                if (map.at(key).pool.count(_sni)){
                    pool1_t & p1 (map[key].pool[_sni]);
                    p1.lastUsage=std::chrono::steady_clock::now();
                    if (p1.users.empty()){
                        p1.connections.push(message);
                    } else {
                        error_code_t ok;
                        p1.users.front()(ok,interface_ptr{new implementation(key,message)});
                        p1.users.pop();
                    }
                }
            }
        });     
    }
    static void get(const broker_handler_t & handler,const std::string & key,const std::string & sni){
        pool2_t & p2(map()[key]);
        pool1_t & p1(p2.pool[sni]);
        p1.lastUsage=std::chrono::steady_clock::now();
        if (p1.connections.empty()){
            if (p2.connector){
                p1.users.push(handler);
                p2.connector->async_connection([key,sni](const error_code_t& ec,const ict::asio::connection::message_ptr & message){
                    if (!ec){
                        implementation::put(message,key,sni);
                    }
                });
            } else {
                interface_ptr ptr;
                error_code_t e(ENOMEDIUM,std::generic_category());
                handler(e,ptr);
            }
        } else {
            error_code_t ok;
            handler(ok,interface_ptr{new implementation(key,p1.connections.front())});
            p1.connections.pop();
        }
    }
    static void timerClean(){
        post2([](){
            std::set<std::string> keys2;//Adresy
            for (map2_t::iterator it2=implementation::map().begin();it2!=implementation::map().end();++it2){//Przejście po wszystkich adresach
                bool erase2=false;
                if (it2->second.pool.empty()){//Nie ma nazw SNI
                    erase2=true;
                } else { //Jest co najmniej jedna nazwa SNI
                    std::set<std::string> keys1;//Nazwy SNI
                    erase2=true;
                    for (map1_t::iterator it1=it2->second.pool.begin();it1!=it2->second.pool.end();++it1){//Przejście po wszystkich nazwach SNI
                        bool erase1=true;
                        if (it1->second.users.empty()){//Nikt nie czeka na połącznie
                            if ((std::chrono::steady_clock::now()-it1->second.lastUsage)<std::chrono::seconds(120)){//Brak aktywności w ostatnim czasie
                                if (!it1->second.connections.empty()){//Jeśli są jakieś dostępne połączenia
                                    it1->second.connections.pop();//Zdejmij jedno połączenie
                                    erase2=false;
                                    erase1=false;    
                                }
                            } else {//Aktywność w ostatnim czasie
                                erase2=false;
                                erase1=false;
                            }
                        } else {//Ktoś czeka na połączenie
                            erase2=false;
                            erase1=false;
                        }
                        if (erase1) keys1.emplace(it1->first);//Zapisz nazwę SNI do skasowania
                    }
                    if ((1<keys1.size())&&(keys1.count(_empty_))) keys1.erase(_empty_);//Nazwa SNI pusta jest kasowana na końcu
                    for (const std::string & k : keys1) it2->second.pool.erase(k);//Kasowanie nazw SNI
                }
                if (erase2) keys2.emplace(it2->first);//Zapisz adres do skasowania
            }
            for (const std::string & k : keys2) implementation::map().erase(k);//Kasowanie adresów
        });
    }
    struct timer_t {
        static ::asio::steady_timer & timer(){
            static ::asio::steady_timer t(ict::asio::ioService());
            return t;
        }
        static void timerRun(){
            timer().expires_from_now(std::chrono::seconds(2));
            timer().async_wait([](const error_code_t& ec){
                if (!ec){
                    implementation::timerClean();
                    implementation::timer_t::timerRun();
                }
            });
        }
        static void timerCancel(){
            timer().cancel();
        }
        timer_t(){
            timerRun();
        }
        ~timer_t(){
            timerCancel();
        }
    };
    implementation(const std::string & k,const ict::asio::connection::message_ptr & c):key(k),message(c){
        static timer_t t;
        if (message&&message->connection&&message->connection->connection) {
            connection=message->connection->connection;
        }
    }
public:
    ~implementation() override {
        if (connection){
            put(message,key,connection->getSNI());
        }
    }
    static void get(const broker_handler_t & handler,const std::string & host,const std::string & port,bool server,const ict::asio::context_ptr & context,const std::string & sni){
        post2([handler,host,port,server,context,sni](){
            std::string k(host+_colon_+port+_colon_+(server?_server_:_client_));
            if (!map()[k].connector) map()[k].connector=ict::asio::connector::get(host,port,server,context,sni);
            get(handler,k,sni);
        });
    }
    static void get(const broker_handler_t & handler,const std::string & path,bool server,const ict::asio::context_ptr & context,const std::string & sni){
        post2([handler,path,server,context,sni](){
            std::string k(path+_colon_+(server?_server_:_client_));
            if (!map()[k].connector) map()[k].connector=ict::asio::connector::get(path,server,context,sni);
            get(handler,k,sni);
        });
    }
    map_info_t & info(){
        auto self(interface::enable_shared_t::shared_from_this());
        static map_info_t bogus;
        if (connection){
            return connection->info;
        }
        return bogus;
    }
    void close() override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->close();
        }
    }
    bool is_open() const override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            return connection->is_open();
        }
        return false;
    }
    std::size_t available() const override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            return connection->available();
        }
        return 0;
    }
    void cancel() override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->cancel();
        }
    }
    void cancel(error_code_t& ec) override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->cancel(ec);
        }
    }
    void post(const asio_handler_t &handler) override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->post(handler);
        }
    }
    const std::string & getSNI() override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            return connection->getSNI();
        }
        return _empty_;
    }
    void async_write_body(const handler_t &handler) override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->post([this,self,handler](){
                switch (status){
                    case request_headers:
                        status=request_body;
                    case request_body:
                        message->async_write_body(request.body,request.bytesLeft,handler);
                        break;
                    case response_headers:
                        status=response_body;
                    case response_body:
                        message->async_write_body(response.body,response.bytesLeft,handler);
                        break;
                    default:{
                        error_code_t e(EBADE,std::generic_category());
                        handler(e);
                    }
                }
            });
        } else {
            error_code_t e(ENOMEDIUM,std::generic_category());
            handler(e);
        }
    }
    void async_read_body(const handler_t &handler) override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->post([this,self,handler](){
                switch (status){
                    case request_headers:
                        status=request_body;
                    case request_body:
                        message->async_read_body(request.body,request.bytesLeft,handler);
                        break;
                    case response_headers:
                        status=response_body;
                    case response_body:
                        message->async_read_body(response.body,response.bytesLeft,handler);
                        break;
                    default:{
                        error_code_t e(EBADE,std::generic_category());
                        handler(e);
                    }
                }
            });
        } else {
            error_code_t e(ENOMEDIUM,std::generic_category());
            handler(e);
        }
    }
    void async_write_request_headers(const handler_t &handler) override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->post([this,self,handler](){
                status=request_headers;
                message->async_write_request_headers(request.headers,handler);
            });
        } else {
            error_code_t e(ENOMEDIUM,std::generic_category());
            handler(e);
        }
    }
    void async_read_request_headers(const handler_t &handler) override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->post([this,self,handler](){
                status=request_headers;
                message->async_read_request_headers(request.headers,handler);
            });
        } else {
            error_code_t e(ENOMEDIUM,std::generic_category());
            handler(e);
        }
    }
    void async_write_response_headers(const handler_t &handler) override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->post([this,self,handler](){
                status=response_headers;
                message->async_write_response_headers(response.headers,handler);
            });
        } else {
            error_code_t e(ENOMEDIUM,std::generic_category());
            handler(e);
        }
    }
    void async_read_response_headers(const handler_t &handler) override {
        auto self(interface::enable_shared_t::shared_from_this());
        if (connection){
            connection->post([this,self,handler](){
                status=response_headers;
                message->async_read_response_headers(response.headers,handler);
            });
        } else {
            error_code_t e(ENOMEDIUM,std::generic_category());
            handler(e);
        }
    }
};
//============================================
void get(const broker_handler_t & handler,const std::string & host,const std::string & port,bool server,const ict::asio::context_ptr & context,const std::string & sni){
    implementation::get(handler,host,port,server,context,sni);
}
void get(const broker_handler_t & handler,const std::string & path,bool server,const ict::asio::context_ptr & context,const std::string & sni){
    implementation::get(handler,path,server,context,sni);
}
//============================================
//============================================
}}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include "asio.hpp"
#include <atomic>

struct test_t : public std::enable_shared_from_this<test_t>{
    typedef  std::enable_shared_from_this<test_t> enable_shared_t;
    std::atomic<int> & out;
    std::string host;
    std::string port;
    ict::asio::broker::interface_ptr client;
    ict::asio::broker::interface_ptr server;
    test_t(std::atomic<int> & o):out(o){
        host="localhost";
        port="300"+std::to_string(rand()%90+10);
    }
    void init(){
        auto self(enable_shared_t::shared_from_this());
        ict::asio::broker::get([self,this](const ict::asio::error_code_t& ec,ict::asio::broker::interface_ptr ptr){
            if (ec){
                out=__LINE__;ict::asio::ioStop();
                std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
            } else {
                std::cerr<<__LINE__<<"|"<<"ict::asio::broker::get(server)"<<std::endl;
                server=ptr;
                if (client) run1();
            }
        },host,port,true);
        ict::asio::broker::get([self,this](const ict::asio::error_code_t& ec,ict::asio::broker::interface_ptr ptr){
            if (ec){
                out=__LINE__;ict::asio::ioStop();
                std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
            } else {
                std::cerr<<__LINE__<<"|"<<"ict::asio::broker::get(client)"<<std::endl;
                client=ptr;
                if (server) run1();
            }
        },host,port,false);  
    }
    void run1(){
        auto self(enable_shared_t::shared_from_this());
        client->request.headers.request.method="GET";
        client->request.headers.request.uri="/";
        client->request.headers.request.version="HTTP/1.1";
        client->request.headers.headers.emplace_back(ict::asio::message::header_t{"Host","example.com"});
        client->request.headers.headers.emplace_back(ict::asio::message::header_t{":",""});
        client->async_write_request_headers([self,this](const ict::asio::error_code_t& ec){
            if (ec){
                out=__LINE__;ict::asio::ioStop();
                std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
            } else {
                std::cerr<<__LINE__<<"|"<<"client->async_write_request_headers()"<<std::endl;
                client->async_write_body([self,this](const ict::asio::error_code_t& ec){
                    if (ec) {
                        out=__LINE__;ict::asio::ioStop();
                        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
                    } else {
                        std::cerr<<__LINE__<<"|"<<"client->async_write_body()"<<std::endl;
                        run2();
                    }
                });
            }
        });        
    }
    void run2(){
        auto self(enable_shared_t::shared_from_this());
        server->async_read_request_headers([self,this](const ict::asio::error_code_t& ec){
            if (ec){
                out=__LINE__;ict::asio::ioStop();
                std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
            } else {
                std::cerr<<__LINE__<<"|"<<"server->async_read_request_headers()"<<std::endl;
                server->async_read_body([self,this](const ict::asio::error_code_t& ec){
                    if (ec) {
                        out=__LINE__;ict::asio::ioStop();
                        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
                    } else {
                        std::cerr<<__LINE__<<"|"<<"server->async_read_body()"<<std::endl;
                        run3();
                    }
                });
            }
        });
    }
    void run3(){
        auto self(enable_shared_t::shared_from_this());
        server->response.headers.response.version="HTTP/1.1";
        server->response.headers.response.code="200";
        server->response.headers.response.explanation="OK";
        server->response.headers.headers.emplace_back(ict::asio::message::header_t{"Host","example.com"});
        server->response.headers.headers.emplace_back(ict::asio::message::header_t{":",""});
        server->async_write_response_headers([self,this](const ict::asio::error_code_t& ec){
            if (ec){
                out=__LINE__;ict::asio::ioStop();
                std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
            } else {
                std::cerr<<__LINE__<<"|"<<"server->async_write_response_headers()"<<std::endl;
                server->async_write_body([self,this](const ict::asio::error_code_t& ec){
                    if (ec) {
                        out=__LINE__;ict::asio::ioStop();
                        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
                    } else {
                        std::cerr<<__LINE__<<"|"<<"server->async_write_body()"<<std::endl;
                        run4();
                    }
                });
            }
        });        
    }
    void run4(){
        auto self(enable_shared_t::shared_from_this());
        client->async_read_response_headers([self,this](const ict::asio::error_code_t& ec){
            if (ec){
                out=__LINE__;ict::asio::ioStop();
                std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
            } else {
                std::cerr<<__LINE__<<"|"<<"client->async_read_response_headers()"<<std::endl;
                client->async_read_body([self,this](const ict::asio::error_code_t& ec){
                    if (ec) {
                        out=__LINE__;ict::asio::ioStop();
                        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
                    } else {
                        std::cerr<<__LINE__<<"|"<<"client->async_read_body()"<<std::endl;
                        ict::asio::ioStop();
                    }
                });
            }
        });
    }
};
REGISTER_TEST(broker,tc1){
    std::atomic<int> out=0;
    ict::asio::ioSignal();
    ict::asio::ioRun();
    {
        std::shared_ptr<test_t> t=std::make_shared<test_t>(out);
        t->init();
        sleep(2);
    }
    ict::asio::ioJoin();
    return(out);
}
#endif
//===========================================
