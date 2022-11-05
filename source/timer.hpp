//! @file
//! @brief ASIO timer module - header file.
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
#ifndef _ASIO_TIMER_HEADER
#define _ASIO_TIMER_HEADER
//============================================
#include <chrono>
#include <functional>
#include "types.hpp"
//============================================
namespace ict { namespace asio { namespace timer {
//===========================================
//! Typ - punkt w czasie dla zegara systemowego
typedef std::chrono::system_clock::time_point time_point_t;
//! Typ - punkt w czasie dla zegara ciągłego
typedef std::chrono::steady_clock::time_point steady_point_t;
//! Typ - okres czasu dla zegara ciągłego
typedef std::chrono::steady_clock::duration duration_t;
//! Struktura dla daty/czasu kalendarza gregoriańskiego.
struct date_time_t{
    int year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    bool is_dst=false;
};
//===========================================
//! Interfejs do obsługi timera.
class interface;
//! Wskaźnik do interfejsu timera.
typedef std::shared_ptr<interface> interface_ptr;
//! Interfejs do obsługi timera.
class interface : public std::enable_shared_from_this<interface> {
protected:
  //! Status ustawienia timera.
  enum status_t {
      none,   //!> Timer nie jest ustawiony.
      system, //!> Timer jest ustawiony na punkt w czasie wg zegara systemowego.
      steady, //!> Timer jest ustawiony na punkt w czasie wg zegara ciągłego.
      both    //!> Timer jest ustawiony na punkt w czasie wg najpierw zegara systemowego, potem zegara ciągłego.
  } status=none;
public:
  //! Typ pomocniczy do generowania wskaźnika.
  typedef  std::enable_shared_from_this<interface> enable_shared_t;
  //! Typ - Funkcja do obsługi timera.
  typedef std::function<void(const ict::asio::error_code_t&)> handler_t;
  //! Metadane timera
  map_info_t info;
  std::string getInfo(){
    std::string o;
    for (map_info_t::const_iterator it=info.begin();it!=info.end();++it){
      if (it!=info.begin()) o+=",";
      o+=it->first;
      o+="=";
      o+=it->second;
    }
    return(o);
  }
public:
  //! Destruktor
  virtual ~interface(){}
  //! 
  //! @brief Ustawia timer na punkt w czasie wg zegara systemowego.
  //! 
  //! @param tp Punkt w czasie.
  //! 
  virtual void set(const time_point_t & tp)=0;
  //! 
  //! @brief Ustawia timer na punkt w czasie (data/czas) wg zegara systemowego.
  //! 
  //! @param dt Punkt w czasie (data/czas).
  //! 
  virtual void set(const date_time_t & dt)=0;
  //! 
  //! @brief Ustawia timer na punkt w czasie (okres czasu liczony od teraz) wg zegara ciągłego.
  //! 
  //! @param du Okres czasu liczony od teraz.
  //! 
  virtual void set(const duration_t & du)=0;
  //! 
  //! @brief Ustawia timer na punkt w czasie (okres czasu liczony od podanego punktu w czasie) wg zegara systemowego, potem zegara ciągłego.
  //! 
  //! @param tp Punkt w czasie.
  //! @param du Okres czasu liczony od punktu w czasie (patrz wyżej).
  //! 
  virtual void set(const time_point_t & tp,const duration_t & du)=0;
  //! 
  //! @brief Ustawia timer na punkt w czasie (okres czasu liczony od podanego punktu w czasie - data/czas) wg zegara systemowego, potem zegara ciągłego.
  //! 
  //! @param dt Punkt w czasie (data/czas).
  //! @param du Okres czasu liczony od punktu w czasie (patrz wyżej).
  //! 
  virtual void set(const date_time_t & dt,const duration_t & du)=0;
  //! 
  //! @brief Ustawia timer na punkt w czasie (okres czasu liczony od momentu wyzwolenia podanego timera) wg zegara systemowego, potem zegara ciągłego.
  //! 
  //! @param ref 
  //! @param du 
  //! 
  virtual void set(const interface_ptr & ref,const duration_t & du)=0;
  //! 
  //! @brief Dodaje zadanie do wykonania po wyzwoleniu timera (należy wykonać po ustawieniu timera).
  //! 
  //! @param h Zadanie do wykonania.
  //! 
  virtual void async_wait(const handler_t & h)=0;
  //! Anuluje wszystkie asynchroniczne operacje w timerze.
  virtual void cancel()=0;
  virtual void cancel(error_code_t& ec)=0;
  //! 
  //! @brief Zwraca aktualnie ustawiony punkt w czasie timera dla zegara systemowego.
  //! 
  //! @return time_point_t Punkt w czasie dla zegara systemowego.
  //! 
  virtual time_point_t get_system() const=0;
  //! 
  //! @brief Zwraca aktualnie ustawiony punkt w czasie timera dla zegara ciągłego.
  //! 
  //! @return time_point_t Punkt w czasie dla zegara ciągłego.
  //! 
  virtual steady_point_t get_steady() const=0;
  //! 
  //! @brief Zwraca aktualnie ustawiony okres dla timera zegara ciągłego.
  //! 
  //! @return duration_t Ustawiony okres czasu.
  //! 
  virtual duration_t get_duration() const=0;
  //! 
  //! @brief Zwraca status ustawienia timera.
  //! 
  //! @return const status_t& Status.
  //! 
  const status_t & get_status() const {
    return status;
  }
};
//===========================================
//! 
//! @brief Zamienia datę/czas na punkt w czasie.
//! 
//! @param dt Data czas.
//! @param local Informacja, czy data/czas to czas lokalny, czy uniwersalny.
//! @return time_point_t Punkt w czasie.
//! 
time_point_t convert(const date_time_t & dt,bool local=true);
//! 
//! @brief Zamienia punkt w czasie na datę/czas.
//! 
//! @param tp Punkt w czasie.
//! @param local Informacja, czy data/czas to czas lokalny, czy uniwersalny.
//! @return Data/czas.
//! 
date_time_t convert(const time_point_t & tp,bool local=true);
//===========================================
//! Zwraca interfejs do timera.
//! @returns Wskaźnik do interfejsu do timera.
interface_ptr get();
//! 
//! @brief Zwraca interfejs do timera i ustawia go.
//! 
//! @param tp Punkt w czasie.
//! @return interface_ptr Wskaźnik do interfejsu do timera.
//! 
interface_ptr get(const time_point_t & tp);
//! 
//! @brief Zwraca interfejs do timera i ustawia go.
//! 
//! @param dt Punkt w czasie (data/czas).
//! @return interface_ptr Wskaźnik do interfejsu do timera.
//! 
interface_ptr get(const date_time_t & dt);
//! 
//! @brief Zwraca interfejs do timera i ustawia go.
//! 
//! @param du Okres czasu od teraz.
//! @return interface_ptr Wskaźnik do interfejsu do timera.
//! 
interface_ptr get(const duration_t & du);
//! 
//! @brief Zwraca interfejs do timera i ustawia go.
//! 
//! @param tp Punkt w czasie.
//! @param du Okres czasu od punktu w czasie (patrz powyżej).
//! @return interface_ptr Wskaźnik do interfejsu do timera.
//! 
interface_ptr get(const time_point_t & tp,const duration_t & du);
//! 
//! @brief Zwraca interfejs do timera i ustawia go.
//! 
//! @param dt Punkt w czasie (data/czas).
//! @param du Okres czasu od punktu w czasie (patrz powyżej).
//! @return interface_ptr Wskaźnik do interfejsu do timera.
//! 
interface_ptr get(const date_time_t & dt,const duration_t & du);
//! 
//! @brief Zwraca interfejs do timera i ustawia go.
//! 
//! @param ref Referencja do timera, od którego wyzwolenia (punkt w czasie) ma być liczony czas.
//! @param du Okres czasu od punktu w czasie (patrz powyżej).
//! @return interface_ptr Wskaźnik do interfejsu do timera.
//! 
interface_ptr get(const interface_ptr & ref,const duration_t & du);
//============================================
}}}
//===========================================
#endif
