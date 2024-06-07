# `ict::asio` module

This module provides several functions for to basic Asio functions and objects:
* `ict::asio::ioService()` - Provides access to `asio::io_service`.
* `ict::asio::ioSignal(const signal_handler_t & handler)` - Sets handler for SIGINT and SIGTERM signals.
* `ict::asio::ioSignal()` - Sets basic handler for SIGINT and SIGTERM signals (it only stops io processing).
* `ict::asio::ioRun()` - Starts `asio::io_service::run()` in several threads (number of threads is equal to `std::thread::hardware_concurrency()`).
* `ict::asio::ioJoin()` - Waits for end of `asio::io_service::run()`.
* `ict::asio::ioRunJoin()` - Starts `asio::io_service::run()` in several threads (number of threads is equal to `std::thread::hardware_concurrency()`) and waits for end of `asio::io_service::run()`.
* `void ict::asio::ioStop()` - Executes `asio::io_service::stop()`.

Example of usage:
```c
int main(){
  ict::asio::ioSignal();//Sets signal handler.
  ict::asio::ioRun();//Runs io service.
  ict::asio::ioServicePost([](){//Ads task.
    ict::asio::ioStop()//Stops io service
  });
  ict::asio::ioJoin();//Waits for threads end.
  return(0);
}
```
