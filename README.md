# Asio C++ Library wrapper

This code implements a simple wrapper for Asio C++ Library.

See:
* [asio](source/asio.md) for more details about basic Asio functions and objects;
* [timer](source/timer.md) for more details about basic timer objects;
* [connector](source/connector.md) for more details about connection handling (server and client side).
* [connection](source/connection.md) for more details about connection interface;

## Building instructions

```sh
make # Build library
make test # Execute all tests
make package # Create library package
make package_source  # Create source package
```
