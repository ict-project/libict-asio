# `ict::asio::resolver` module

This module provides function for resolving `asio::ip::tcp::endpoint` and `asio::local::stream_protocol::endpoint` endpoints.

In order get the endpoint list following functions should be used:
* `ict::asio::resolver::get(host,port,handler)` - resolves `asio::ip::tcp::endpoint` for given host and port.
* `ict::asio::resolver::get(path,handler)` - resolves `asio::local::stream_protocol::endpoint` for given path.

Once the endpoint is resolved a handler function is executed that looks like this:
```c
//! @param ep Resolved endpoint.
//! @param ec Error code.
void(tcp_endpoint_info_ptr& ep,const error_code_t& ec);
//! @param ep Resolved endpoint.
//! @param ec Error code.
void(stream_endpoint_info_ptr& ep,const error_code_t& ec);
```