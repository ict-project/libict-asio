//! @file
//! @brief Lock module - source file.
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
#include "lock.hpp"
#include "service.h"
#include <string>
#include <queue>
#include <map>
#include <asio.hpp>
//============================================
namespace ict { namespace asio { namespace lock {
//============================================
const static std::string _key_("key");
//============================================
class implementation : public interface{
private:
    typedef std::queue<lock_handler_t> queue_t;
    typedef std::map<std::string,queue_t> map_t;
    std::string key;
    static map_t & map(){
        static map_t m;
        return m;
    }
    static void post(asio_handler_t handler){
        static ::asio::io_service::strand strand(ioService());
        strand.post(handler);
    }
    implementation(const std::string & k):key(k){
        info[_key_]=k;
    }
public:
    static void get(const std::string & key, const lock_handler_t & handler){
        post([key,handler](){
            if (implementation::map().count(key)){
                implementation::map()[key].push(handler);
            } else {
                implementation::map()[key];
                handler(interface_ptr{new implementation(key)});
            }
        });
    }
    //! Destruktor
    ~implementation() override {
        const std::string k(key);
        post([k](){
            if (implementation::map().count(k)){
                if (implementation::map().at(k).empty()){
                    implementation::map().erase(k);
                } else {
                    implementation::map()[k].front()(interface_ptr{new implementation(k)});
                    implementation::map()[k].pop();
                }

            }
        });
    }
};
//============================================
void get(const std::string & key, const lock_handler_t & handler){
    implementation::get(key,handler);
}
//============================================
}}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include "asio.hpp"
#include <atomic>
REGISTER_TEST(lock,tc1){
    std::atomic<int> out=3;
    ict::asio::lock::interface_ptr p1,p2,p3;
    ict::asio::ioSignal();
    ict::asio::ioRun();
    {
        ict::asio::lock::get("test",[&p1,&out](ict::asio::lock::interface_ptr p){
            p1=p;
            if (0<out){
                out=(out==3)?(out-1):-1;
            }
        });
        ict::asio::lock::get("test",[&p2,&out](ict::asio::lock::interface_ptr p){
            p2=p;
            if (0<out){
                out=(out==2)?(out-1):-2;
            }
        });
        ict::asio::lock::get("test",[&p3,&out](ict::asio::lock::interface_ptr p){
            p3=p;
            if (0<out){
                out=(out==1)?(out-1):-3;
            }
        });
        usleep(10000);
        p1=nullptr;
        usleep(10000);
        p2=nullptr;
        usleep(10000);
        p3=nullptr;
        usleep(10000);
        ict::asio::ioService().stop();
    }
    ict::asio::ioJoin();
    return(out);
}
#endif
//===========================================
