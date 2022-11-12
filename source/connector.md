# `ict::asio::connector` module

This module provides connectors that handling incoming (server) or outgoing (client) connections. In order to get a new connector one of following function should be used:
* `ict::asio::connector::get(host,port,server)` - Gets pointer (`std::shared_ptr`) to a connector to host:port (TCP socket).
* `ict::asio::connector::get(path,server)` - Gets pointer (`std::shared_ptr`) to a connector to path (local socket).
* `ict::asio::connector::get(host,port,server,context,setSNI)` - Gets pointer (`std::shared_ptr`) to a connector to host:port (TCP socket) with SSL (defined by context).
* `ict::asio::connector::get(path,server,context,setSNI)` - Gets pointer (`std::shared_ptr`) to a connector to path (local socket) with SSL (defined by context).

The param `server` determines if connector is a server ('true') or a client ('false').

The connector interface:
```c
//! Closes the connector.
void close();
//! Tests if connector is open.
bool is_open() const;
//! Tests if error occured.
bool is_error() const;
//! Cancels all asynchronous operations associated with the connector.
void cancel();
void cancel(error_code_t& ec); 
//! Handler for a new connection.
void async_connection(const ict::asio::connection::connection_handler_t &handler);
```

When `async_connection(handler)` function is used then:
* Establishing of the connection is started - if connector is a client;
* Waiting for the connection is started - if connector is a server.

Once the connection is establised (client) or accepted (server) the handler is executed. In order to repeat the cicle the function `async_connection(handler)` must be called again.

The connection handler (`ict::asio::connection::connection_handler_t`) is a function/functor that looks like this:
```c
//! param ec Error code.
//! param ptr Pointer (std::shared_ptr) to connection interface
void(const error_code_t& ec,interface_ptr ptr)
```