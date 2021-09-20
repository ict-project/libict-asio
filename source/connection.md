# `ict::asio::connection` module

This module provides the interface that handling read/write on a conection (TCP or local stream type).

## Basic interface (*connection.hpp*)

In order to get a new connection interface one of following function should be used:
* `ict::asio::connection::get(socket)` - Gets pointer (`std::shared_ptr`) to an interface to a connection for given socket (TCP o local stream).
* `ict::asio::connection::get(socket,context,setSNI)` - Gets pointer (`std::shared_ptr`) to an interface to a connection for given socket (TCP o local stream) with SSL (for given context and SNI).

The connection interface:
```c
//! Closes the connection.
void close();
//! Tests if connection is open.
bool is_open() const;
//! Returns the number of bytes waiting to be read.
std::size_t available() const;
//! Writes data to a connection.
//! @param buffer Buffer with data to write (Note: size must be set before use!).
//! @param handler Function executed after write operation.
void async_write_some(buffer_t& buffer,const handler_t &handler);
//! Reads data from a connection
//! @param buffer Buffer for data read (Note: size must be set before use!).
//! @param handler Function executed after read operation.
void async_read_some(buffer_t& buffer,const handler_t &handler;
//! Returns the server name (SNI) - SSL only.
//! @returns The name of the server (SNI).
const std::string & getSNI();
```

When `async_write_some(buffer,handler)` function is used then writing process is started. Once the write is done the handler is executed. In order to repeat the cicle the function `async_write_some(buffer,handler)` must be called again.

When `async_read_some(buffer,handler)` function is used then reading process is started. Once the read is done the handler is executed. In order to repeat the cicle the function `async_read_some(buffer,handler)` must be called again.

The handler (`ict::asio::connection::interface::handler_t`) is a function/functor that looks like this:
```c
//! param ec Error code.
//! param s Size of data that was really read or written.
void(const ict::asio::error_code_t& ec,std::size_t s)
```
The data buffer (`ict::asio::connection::interface::buffer_t`) is defined like this:
```c
typedef std::vector<unsigned char> buffer_t;
``` 

## Interface with `std::string` buffer (*connection-string.hpp*)

More advance version of the basic interface. In order to get a new connection interface (string) one of following function should be used:
* `ict::asio::connection::getString(iface)` - Gets pointer (`std::shared_ptr`) to an interface (string) to a connection for given basic interface.
* `ict::asio::connection::getString(socket)`  - Gets pointer (`std::shared_ptr`) to an interface (string) to a connection for given socket (TCP o local stream).
* `ict::asio::connection::getString(socket,context,setSNI)`- Gets pointer (`std::shared_ptr`) to an interface (string) to a connection for given socket (TCP o local stream) with SSL (for given context and SNI).

The connection interface (string):
```c
//! Pointer to basic interface of the connection.
interface_ptr connection;
//! Writes data to a connection.
//! @param buffer Buffer with data to write (Note: Data is removed from the begining of the string.).
//! @param handler Function executed after write operation.
//! 
void async_write_string(std::string & buffer,const handler_t &handler);
//! Reads data from a connection
//! @param buffer Buffer for data read (Note: New data is added to the end of the string.).
//! @param handler Function executed after read operation.
void async_read_string(std::string & buffer,const handler_t &handler);
```

## Interface with message buffer (*connection-message.hpp*)

More advance version of the string interface. In order to get a new connection interface (message) one of following function should be used:
* `ict::asio::connection::getMessage(iface)` - Gets pointer (`std::shared_ptr`) to an interface (message) to a connection for given basic or string interface.
* `ict::asio::connection::getMessage(socket)`  - Gets pointer (`std::shared_ptr`) to an interface (message) to a connection for given socket (TCP o local stream).
* `ict::asio::connection::getMessage(socket,context,setSNI)`- Gets pointer (`std::shared_ptr`) to an interface (message) to a connection for given socket (TCP o local stream) with SSL (for given context and SNI).

The connection interface (string):
```c
//! Pointer to string interface of the connection.
string_ptr connection;
//! Writes request line to a connection.
//! @param request Request data to write (Note: Data is cleared after write.)
//! @param handler Function executed after write operation.
void async_write_request(request_t & request,const handler_t &handler);
//! Reads request line from a connection.
//! @param request Request data read.
//! @param handler Function executed after read operation.
void async_read_request(request_t & request,const handler_t &handler);
//! Writes response line to a connection.
//! @param response Response data to write (Note: Data is cleared after write.)
//! @param handler Function executed after write operation.
void async_write_response(response_t & response,const handler_t &handler);
//! Reads response line from a connection.
//! @param response Response data read.
//! @param handler Function executed after read operation.
void async_read_response(response_t & response,const handler_t &handler);
//! Writes header to a connection.
//! @param header Header data to write (Note: Data is cleared after write.)
//! @param handler Function executed after write operation.
void async_write_header(header_t & header,const handler_t &handler);
//! Reads header from a connection.
//! @param header Header data read.
//! @param handler Function executed after read operation.
void async_read_header(header_t & header,const handler_t &handler);
//! Writes body data to a connection.
//! @param data Data to write (Note: Data is cleared after write.)
//! @param bytesLeft Controls size of body. It is decreased after each write and stops writing when reaches 0.
//! @param handler Function executed after write operation.
void async_write_body(std::string & data,std::size_t & bytesLeft,const handler_t &handler);
//! Reads body data from a connection.
//! @param data Data read.
//! @param bytesLeft Controls size of body. It is decreased after each read and stops reading when reaches 0.
//! @param handler Function executed after read operation.
void async_read_body(std::string & data,std::size_t & bytesLeft,const handler_t &handler);
//! Writes headers to a connection.
//! @param headers Headers data to write (Note: Data is cleared after write. Note: After last header empty one should be added - that is with name=':' !).
//! @param handler Function executed after write operation.
void async_write_headers(headers_t & headers,const handler_t &handler);
//! Reads headers from a connection.
//! @param headers Headers data read (Note: After last header empty one is added - with name=':' !).
//! @param handler Function executed after read operation.
void async_read_headers(headers_t & headers,const handler_t &handler);
//! Writes request line and headers to a connection.
//! @param request Data to write (Note: Data is cleared after write. Note: After last header empty one should be added - that is with name=':' !).
//! @param handler Function executed after write operation.
void async_write_request_headers(request_headers_t & request,const handler_t &handler);
//! Reads request line and headers from a connection.
//! @param headers Data read (Note: After last header empty one is added - with name=':' !).
//! @param handler Function executed after read operation.
void async_read_request_headers(request_headers_t & request,const handler_t &handler);
//! Writes response line and headers to a connection.
//! @param response Data to write (Note: Data is cleared after write. Note: After last header empty one should be added - that is with name=':' !).
//! @param handler Function executed after write operation.
void async_write_response_headers(response_headers_t & response,const handler_t &handler);
//! Reads response line and headers from a connection.
//! @param response Data read (Note: After last header empty one is added - with name=':' !).
//! @param handler Function executed after read operation.
void async_read_response_headers(response_headers_t & response,const handler_t &handler);
```