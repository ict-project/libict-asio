# Asio C++ Library wrapper

This code implements a simple wrapper for Asio C++ Library.

See:
* [asio](source/asio.md) for more details about basic Asio functions and objects;
* [resolver](source/resolver.md) for more details about async DNS resolving;
* [connection](source/connection.md) for more details about connection interface;
* [connector](source/connector.md) for more details about connection handling (server and client side).

## Building instructions

```sh
make # Build library
make test # Execute all tests
make package # Create library package
make package_source  # Create source package
```
