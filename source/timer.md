# `ict::asio::timer` module

This module provides acces to timers. In order to get a new timer one of following function should be used:
* `ict::asio::timer::get()` - Gets pointer (`std::shared_ptr`) to a timer that is not set.
* `ict::asio::timer::get(tp)` - Gets pointer (`std::shared_ptr`) to a timer that expires at givent time point (`std::chrono::system_clock::time_point`).
* `ict::asio::timer::get(dt)` - Gets pointer (`std::shared_ptr`) to a timer that expires at givent date and time (see struct below).
* `ict::asio::timer::get(du)` - Gets pointer (`std::shared_ptr`) to a timer that expires after given duration (`std::chrono::steady_clock::duration`) from now.
* `ict::asio::timer::get(tp,du)` - Gets pointer (`std::shared_ptr`) to a timer that expires after given duration (`std::chrono::steady_clock::duration`) from given point in time (`std::chrono::system_clock::time_point`).
* `ict::asio::timer::get(dt,du)` - Gets pointer (`std::shared_ptr`) to a timer that expires after given duration (`std::chrono::steady_clock::duration`) from given date and time (see struct below).
* `ict::asio::timer::get(ref,du)` - Gets pointer (`std::shared_ptr`) to a timer that expires after given duration (`std::chrono::steady_clock::duration`) from expiration time of given timer (`std::shared_ptr`).

Date and time is defined by following structure:
```c
struct date_time_t{
    int year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    bool is_dst=false;
};
```

The timer interface:
```c
//! 
//! @brief Sets timer to expire at givent time point.
//! 
//! @param tp Point in time.
//! 
void set(const time_point_t & tp);
//! 
//! @brief Sets timer to expire at givent date and time.
//! 
//! @param dt Date and time.
//! 
void set(const date_time_t & dt);
//! 
//! @brief Sets timer to expire after given duration from now.
//! 
//! @param du Duration.
//! 
void set(const duration_t & du);
//! 
//! @brief Sets timer to expire after given duration from given point in time.
//! 
//! @param tp Point in time.
//! @param du Duration.
//! 
void set(const time_point_t & tp,const duration_t & du);
//! 
//! @brief Sets timer to expire after given duration from given date and time.
//! 
//! @param dt Date and time.
//! @param du Duration.
//! 
void set(const date_time_t & dt,const duration_t & du);
//! 
//! @brief Sets timer to expire after given duration from expiration time of given timer.
//! 
//! @param ref Shared pointer to a timer.
//! @param du Duration.
//! 
void set(const interface_ptr & ref,const duration_t & du);
//! 
//! @brief Adds a new task to execute on timer expiration.
//! 
//! @param h Zadanie do wykonania.
//! 
void async_wait(const handler_t & h);
//! Cancels all async tasks.
void cancel();
void cancel(error_code_t& ec);
//! 
```

The timer handler (`ict::asio::timer::interface::handler_t`) is a function/functor that looks like this:
```c
//! param ec Error code.
std::function<void(const ict::asio::error_code_t&)> 
```