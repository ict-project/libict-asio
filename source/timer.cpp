//! @file
//! @brief ASIO timer module - Source file.
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
#include <time.h>
#include <functional>
#include <queue>
#include <asio/strand.hpp>
#include <asio/system_timer.hpp>
#include <asio/steady_timer.hpp>
#include "service.h"
#include "timer.hpp"
//============================================
namespace ict { namespace asio { namespace timer {
//============================================
const static std::string _timer_type_("timer_status");
const static std::string _none_("none");
const static std::string _system_("system");
const static std::string _steady_("steady");
const static std::string _both_("both");
//============================================
class Timer: public interface{
private:
    ::asio::io_context::strand strand;
    ::asio::system_timer system_timer;
    ::asio::steady_timer steady_timer;
    duration_t duration;
    std::queue<handler_t> handlers;
    bool expired=true;
    void exec(const error_code_t& ec);
    time_point_t get_system() const{
        return system_timer.expiry();
    }
    steady_point_t get_steady() const{
        return steady_timer.expiry();
    }
    duration_t get_duration() const{
        return duration;
    }
    void reset(status_t s);
    void set(const time_point_t & tp);
    void set(const date_time_t & dt);
    void set(const duration_t & du);
    void set(const time_point_t & tp,const duration_t & du);
    void set(const date_time_t & dt,const duration_t & du);
    void set(const interface_ptr & ref,const duration_t & du);
    void async_wait(const handler_t & h);
    void cancel();
    void cancel(error_code_t& ec);
public:
    Timer():strand(ict::asio::ioService()),system_timer(ict::asio::ioService()),steady_timer(ict::asio::ioService()){
        reset(none);
    }
    ~Timer(){}
};
//============================================
std::chrono::system_clock::time_point convert(const date_time_t & dt,bool local){
    std::tm t{0};
    t.tm_year=dt.year-1900;
    t.tm_mon=dt.month-1;
    t.tm_mday=dt.day;
    t.tm_hour=dt.hour;
    t.tm_min=dt.minute;
    t.tm_sec=dt.second;
    t.tm_isdst=dt.is_dst?1:0;
    return std::chrono::system_clock::from_time_t(local?mktime(&t):timegm(&t));
}
date_time_t convert(const std::chrono::system_clock::time_point & tp,bool local){
    std::time_t tt=std::chrono::system_clock::to_time_t(tp);
    std::tm * t;
    if (local){
        t=std::localtime(&tt);
    } else {
        t=std::gmtime(&tt);
    }
    return date_time_t{
        (int)(t->tm_year+1900),
        (unsigned char)(t->tm_mon+1),
        (unsigned char)(t->tm_mday),
        (unsigned char)(t->tm_hour),
        (unsigned char)(t->tm_min),
        (unsigned char)(t->tm_sec),
        0<(t->tm_isdst)
    };
}
//============================================
interface_ptr get(){
    return std::make_shared<Timer>();
}
interface_ptr get(const time_point_t & tp){
    interface_ptr ptr=get();
    ptr->set(tp);
    return ptr;
}
interface_ptr get(const date_time_t & dt){
    interface_ptr ptr=get();
    ptr->set(dt);
    return ptr;
}
interface_ptr get(const duration_t & du){
    interface_ptr ptr=get();
    ptr->set(du);
    return ptr;
}
interface_ptr get(const interface_ptr & ref,const duration_t & du){
    interface_ptr ptr=get();
    ptr->set(ref,du);
    return ptr;
}
interface_ptr get(const time_point_t & tp,const duration_t & du){
    interface_ptr ptr=get();
    ptr->set(tp,du);
    return ptr;
}
interface_ptr get(const date_time_t & dt,const duration_t & du){
    interface_ptr ptr=get();
    ptr->set(dt,du);
    return ptr;
}
//============================================
#define TIMER_WAIT(timer) timer.async_wait([self,this](const error_code_t& ec){exec(ec);})
void Timer::exec(const error_code_t& ec){
    auto self(enable_shared_t::shared_from_this());
    strand.post([self,this,ec](){
        while(!handlers.empty()) {
            handlers.front()(ec);
            handlers.pop();
        }
        if (!ec) expired=true;
    });
}
void Timer::reset(status_t s){
    system_timer.expires_at(std::chrono::system_clock::now());
    steady_timer.expires_at(std::chrono::steady_clock::now());
    status=s;
    switch (status){
        default:info[_timer_type_]=_none_;break;
        case system:info[_timer_type_]=_system_;break;
        case steady:info[_timer_type_]=_steady_;break;
        case both:info[_timer_type_]=_both_;break;
    }
    expired=(status==none)?true:false;
}
void Timer::cancel(){
    system_timer.cancel();
    steady_timer.cancel();
}
void Timer::cancel(error_code_t& ec){
    system_timer.cancel(ec);
    steady_timer.cancel(ec);
}
void Timer::set(const time_point_t & tp){
    auto self(enable_shared_t::shared_from_this());
    strand.post([self,this,tp](){
        time_point_t now_system(std::chrono::system_clock::now());
        if ((now_system+std::chrono::seconds(1))<tp){
            reset(system);
            duration=duration_t::zero();
            system_timer.expires_at(tp);
            TIMER_WAIT(system_timer);
        } else {
            reset(steady);
            duration=tp-now_system;
            steady_timer.expires_at(std::chrono::steady_clock::now()+duration);
            TIMER_WAIT(steady_timer);
        }
    });
}
void Timer::set(const date_time_t & dt){
    set(convert(dt));
}
void Timer::set(const duration_t & du){
    auto self(enable_shared_t::shared_from_this());
    strand.post([self,this,du](){
        reset(steady);
        duration=du;
        steady_timer.expires_after(du);
        TIMER_WAIT(steady_timer);
    });
}
void Timer::set(const time_point_t & tp,const duration_t & du){
    auto self(enable_shared_t::shared_from_this());
    strand.post([self,this,tp,du](){
        time_point_t now_system(std::chrono::system_clock::now());
        duration=du;
        if ((now_system+std::chrono::seconds(1))<tp){
            reset(both);
            system_timer.expires_at(tp);
            system_timer.async_wait([self,this,du](const error_code_t& ec){
                strand.post([self,this,ec](){
                    if (ec){
                        exec(ec);
                    } else {
                        steady_timer.expires_after(duration);
                        TIMER_WAIT(steady_timer);
                    }
                });
            });
        } else {
            reset(steady);
            steady_timer.expires_at(std::chrono::steady_clock::now()+du-(now_system-tp));
            TIMER_WAIT(steady_timer);
        }
    });
}
void Timer::set(const date_time_t & dt,const duration_t & du){
    set(convert(dt),du);
}
void Timer::set(const interface_ptr & ref,const duration_t & du){
    switch (ref->get_status()){
        default:{//none
            set(du);
        } break;
        case system:{
            const time_point_t tp(ref->get_system());
            set(tp,du);
        } break;
        case steady:{
            auto self(enable_shared_t::shared_from_this());
            steady_point_t tp(ref->get_steady());
            strand.post([self,this,tp,du](){
                reset(steady);
                duration=du;
                steady_timer.expires_at(tp+du);
                TIMER_WAIT(steady_timer);
            });
        } break;
        case both:{
            time_point_t tp(ref->get_system()+ref->get_duration());
            set(tp,du);
        } break; 
    }
}
void Timer::async_wait(const handler_t & h){
    auto self(enable_shared_t::shared_from_this());
    strand.post([self,this,h](){
        handlers.push(h);
        if (expired) {
            error_code_t ec;
            exec(ec);
        }
    });
}
#undef TIMER_WAIT
//============================================
}}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include "asio.hpp"
#include <atomic>
REGISTER_TEST(timer,tc1){
    ict::asio::timer::time_point_t tp;
    ict::asio::timer::date_time_t dt_in={2022,11,02,15,03},dt_out;
    tp=ict::asio::timer::convert(dt_in);
    dt_out=ict::asio::timer::convert(tp);
    if (dt_in.year!=dt_out.year) return 1;
    if (dt_in.month!=dt_out.month) return 2;
    if (dt_in.day!=dt_out.day) return 3;
    if (dt_in.hour!=dt_out.hour) return 4;
    if (dt_in.minute!=dt_out.minute) return 5;
    if (dt_in.second!=dt_out.second) return 6;
    return 0;
}
REGISTER_TEST(timer,tc2){
    ict::asio::timer::time_point_t tp;
    ict::asio::timer::date_time_t dt_in={2022,11,02,15,03},dt_out;
    tp=ict::asio::timer::convert(dt_in,false);
    dt_out=ict::asio::timer::convert(tp,false);
    if (dt_in.year!=dt_out.year) return 1;
    if (dt_in.month!=dt_out.month) return 2;
    if (dt_in.day!=dt_out.day) return 3;
    if (dt_in.hour!=dt_out.hour) return 4;
    if (dt_in.minute!=dt_out.minute) return 5;
    if (dt_in.second!=dt_out.second) return 6;
    return 0;
}
REGISTER_TEST(timer,tc3){
    ict::asio::timer::time_point_t tp_1,tp_2;
    ict::asio::timer::date_time_t dt_1={2022,11,02,15,03},dt_2;
    tp_1=ict::asio::timer::convert(dt_1);
    dt_2=dt_1;
    dt_2.second++;
    tp_2=ict::asio::timer::convert(dt_2);
    if ((tp_2-tp_1)!=std::chrono::seconds(1)) return 1;
    dt_2=dt_1;
    dt_2.minute++;
    tp_2=ict::asio::timer::convert(dt_2);
    if ((tp_2-tp_1)!=std::chrono::minutes(1)) return 2;
    dt_2=dt_1;
    dt_2.hour++;
    tp_2=ict::asio::timer::convert(dt_2);
    if ((tp_2-tp_1)!=std::chrono::hours(1)) return 3;
    return 0;
}
REGISTER_TEST(timer,tc4){
    ict::asio::timer::time_point_t tp_1,tp_2;
    ict::asio::timer::date_time_t dt_1={2022,11,02,15,03},dt_2;
    tp_1=ict::asio::timer::convert(dt_1,false);
    dt_2=dt_1;
    dt_2.second++;
    tp_2=ict::asio::timer::convert(dt_2,false);
    if ((tp_2-tp_1)!=std::chrono::seconds(1)) return 1;
    dt_2=dt_1;
    dt_2.minute++;
    tp_2=ict::asio::timer::convert(dt_2,false);
    if ((tp_2-tp_1)!=std::chrono::minutes(1)) return 2;
    dt_2=dt_1;
    dt_2.hour++;
    tp_2=ict::asio::timer::convert(dt_2,false);
    if ((tp_2-tp_1)!=std::chrono::hours(1)) return 3;
    return 0;
}
REGISTER_TEST(timer,tc5){
    std::atomic<int> out=-1;
    int dur=2;
    ict::asio::timer::time_point_t tp_start=std::chrono::system_clock::now(),tp_stop;
    ict::asio::ioSignal();
    ict::asio::ioRun();
    {
        ict::asio::timer::interface_ptr ptr=ict::asio::timer::get(std::chrono::seconds(2));
        ptr->async_wait([&](const ict::asio::error_code_t& ec){
            if (ec){
                out=1;
            } else {
                out=0;
            }
            ict::asio::ioStop();
        });
    }
    ict::asio::ioJoin();
    tp_stop=std::chrono::system_clock::now();
    if ((tp_stop-tp_start)<(std::chrono::seconds(dur-1))) out=-2;
    if ((std::chrono::seconds(dur+1))<(tp_stop-tp_start)) out=-3;
    return out;
}
REGISTER_TEST(timer,tc6){
    std::atomic<int> out=-1;
    int dur=2;
    ict::asio::timer::time_point_t tp_start=std::chrono::system_clock::now(),tp_stop;
    ict::asio::ioSignal();
    ict::asio::ioRun();
    {
        ict::asio::timer::interface_ptr ptr=ict::asio::timer::get(std::chrono::system_clock::now()+std::chrono::seconds(2));
        ptr->async_wait([&](const ict::asio::error_code_t& ec){
            if (ec){
                out=1;
            } else {
                out=0;
            }
            ict::asio::ioStop();
        });
    }
    ict::asio::ioJoin();
    tp_stop=std::chrono::system_clock::now();
    if ((tp_stop-tp_start)<(std::chrono::seconds(dur-1))) out=-2;
    if ((std::chrono::seconds(dur+1))<(tp_stop-tp_start)) out=-3;
    return out;
}
REGISTER_TEST(timer,tc7){
    std::atomic<int> out=-1;
    int dur=3;
    ict::asio::timer::time_point_t tp_start=std::chrono::system_clock::now(),tp_stop;
    ict::asio::ioSignal();
    ict::asio::ioRun();
    {
        ict::asio::timer::interface_ptr ptr=ict::asio::timer::get(std::chrono::system_clock::now()+std::chrono::seconds(2),std::chrono::seconds(1));
        ptr->async_wait([&](const ict::asio::error_code_t& ec){
            if (ec){
                out=1;
            } else {
                out=0;
            }
            ict::asio::ioStop();
        });
    }
    ict::asio::ioJoin();
    tp_stop=std::chrono::system_clock::now();
    if ((tp_stop-tp_start)<(std::chrono::seconds(dur-1))) out=-2;
    if ((std::chrono::seconds(dur+1))<(tp_stop-tp_start)) out=-3;
    return out;
}
REGISTER_TEST(timer,tc8){
    std::atomic<int> out=-1;
    int dur=5;
    ict::asio::timer::time_point_t tp_start=std::chrono::system_clock::now(),tp_stop;
    ict::asio::ioSignal();
    ict::asio::ioRun();
    {
        ict::asio::timer::interface_ptr ptr=ict::asio::timer::get(std::chrono::seconds(3));
        usleep(100000);
        ptr->set(ptr,std::chrono::seconds(2));
        usleep(100000);
        ptr->async_wait([&](const ict::asio::error_code_t& ec){
            if (ec){
                out=1;
            } else {
                out=0;
            }
            ict::asio::ioStop();
        });
    }
    ict::asio::ioJoin();
    tp_stop=std::chrono::system_clock::now();
    if ((tp_stop-tp_start)<(std::chrono::seconds(dur-1))) out=-2;
    if ((std::chrono::seconds(dur+1))<(tp_stop-tp_start)) out=-3;
    return out;
}
REGISTER_TEST(timer,tc9){
    std::atomic<int> out=-1;
    int dur=4;
    ict::asio::timer::time_point_t tp_start=std::chrono::system_clock::now(),tp_stop;
    ict::asio::ioSignal();
    ict::asio::ioRun();
    {
        ict::asio::timer::interface_ptr ptr=ict::asio::timer::get(std::chrono::system_clock::now()+std::chrono::seconds(2));
        usleep(100000);
        ptr->set(ptr,std::chrono::seconds(2));
        usleep(100000);
        ptr->async_wait([&](const ict::asio::error_code_t& ec){
            if (ec){
                out=1;
            } else {
                out=0;
            }
            ict::asio::ioStop();
        });
    }
    ict::asio::ioJoin();
    tp_stop=std::chrono::system_clock::now();
    if ((tp_stop-tp_start)<(std::chrono::seconds(dur-1))) out=-2;
    if ((std::chrono::seconds(dur+1))<(tp_stop-tp_start)) out=-3;
    return out;
}
REGISTER_TEST(timer,tc10){
    std::atomic<int> out=-1;
    int dur=5;
    ict::asio::timer::time_point_t tp_start=std::chrono::system_clock::now(),tp_stop;
    ict::asio::ioSignal();
    ict::asio::ioRun();
    {
        ict::asio::timer::interface_ptr ptr=ict::asio::timer::get(std::chrono::system_clock::now()+std::chrono::seconds(2),std::chrono::seconds(1));
        usleep(100000);
        ptr->set(ptr,std::chrono::seconds(2));
        usleep(100000);
        ptr->async_wait([&](const ict::asio::error_code_t& ec){
            if (ec){
                out=1;
            } else {
                out=0;
            }
            ict::asio::ioStop();
        });
    }
    ict::asio::ioJoin();
    tp_stop=std::chrono::system_clock::now();
    if ((tp_stop-tp_start)<(std::chrono::seconds(dur-1))) out=-2;
    if ((std::chrono::seconds(dur+1))<(tp_stop-tp_start)) out=-3;
    return out;
}
#endif