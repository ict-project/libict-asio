//! @file
//! @brief Connection (message) module - Source file.
//! @author Mariusz Ornowski (mariusz.ornowski@ict-project.pl)
//! @version 1.0
//! @date 2021
//! @copyright ICT-Project Mariusz Ornowski (ict-project.pl)
/* **************************************************************
Copyright (c) 2021, ICT-Project Mariusz Ornowski (ict-project.pl)
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
#include "connection-message.hpp"
//============================================
namespace ict { namespace asio { namespace connection {
//============================================
const static std::string _ENDL_("\r\n");
const static std::string _SPACE_(" ");
const static std::string _COLON_(":");
const static std::size_t min(0x100);
const static std::size_t max(0x1000);
//============================================
std::size_t getTokenSize(const std::string & input){
    std::size_t k=0;
    for (std::size_t i=0;i<input.size();i++) 
        if (std::isgraph(input.at(i))){
            k++;
        } else {
            return(k);            
        }
    return(-1);
}
std::size_t getPhraseSize(const std::string & input){
    std::size_t k=0;
    for (std::size_t i=0;i<input.size();i++) 
        if ((input.at(i)=='\n')||(input.at(i)=='\r')){
            return(k);
        } else {
            k++;
        }
    return(-1);
}
std::size_t getNameSize(const std::string & input){
    std::size_t k=0;
    for (std::size_t i=0;i<input.size();i++) 
        if (std::iscntrl(input.at(i))||(input.at(i)==' ')||(input.at(i)==':')){
            return(k);
        } else {
            k++;
        }
    return(-1);
}
std::size_t getSpaceSize(const std::string & input){
    std::size_t k=0;
    for (std::size_t i=0;i<input.size();i++) 
        if (std::iscntrl(input.at(i))||(input.at(i)==' ')){
            k++;
        } else {
            return(k);
        }
    return(-1);
}
std::size_t getSpaceColonSize(const std::string & input){
    std::size_t k=0;
    for (std::size_t i=0;i<input.size();i++) 
        if (std::iscntrl(input.at(i))||(input.at(i)==' ')||(input.at(i)==':')){
            k++;
        } else {
            return(k);
        }
    return(-1);
}
std::size_t getLineSize(const std::string & input){
    bool nl=false;
    bool cr=false;
    std::size_t k=0;
    for (std::size_t i=0;i<input.size();i++) {
        if (input.at(i)=='\r'){
            if (cr){
                return(k);
            } else {
                k++;
                cr=true;
            }
        } else if (input.at(i)=='\n'){
            if (nl){
                return(k);
            } else {
                k++;
                nl=true;
            }
        } else if (std::isblank(input.at(i))) {
            if (nl||cr){
                nl=false;
                cr=false;
                k++;
            } else {
                k++;   
            }
        } else {
            if (nl||cr){
                return(k);
            } else {
                k++;   
            }
        }
    }
    switch (k){
        case 1:
            if (nl||cr) return(k);
            break;
        case 2:
            if (nl&&cr) return(k);
            break;
        default:break;
    }
    return(-1);
}
void message::async_write_request(request_t & request,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    if (!request.method.empty()){
        std::size_t size=0;
        size=getSpaceSize(request.method);
        if (size!=-1){
            request.method.erase(0,size);
        }
        size=getTokenSize(request.method);
        if (size!=-1){
            write.append(request.method.c_str(),size);
        } else {
            write.append(request.method);
        }
        request.method.clear();
        write.append(_SPACE_);
        size=getSpaceSize(request.uri);
        if (size!=-1){
            request.uri.erase(0,size);
        }
        size=getTokenSize(request.uri);
        if (size!=-1){
            write.append(request.uri.c_str(),size);
        } else {
            write.append(request.uri);
        }
        request.uri.clear();
        write.append(_SPACE_);
        size=getSpaceSize(request.version);
        if (size!=-1){
            request.version.erase(0,size);
        }
        size=getTokenSize(request.version);
        if (size!=-1){
            write.append(request.version.c_str(),size);
        } else {
            write.append(request.version);
        }
        request.version.clear();
        write.append(_ENDL_);
        minWrite=min;
    }
    if (minWrite<write.size()){
        if (connection){
            connection->async_write_string(write,[this,self,handler,&request](const ict::asio::error_code_t & ec){
                if (ec){
                    handler(ec);
                } else {
                    async_write_request(request,handler);
                }
            });   
        } else {
            ict::asio::ioService().post([handler](){
                ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
                handler(ec);
            });
        }
    } else {
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ok;
            handler(ok);
        });
    }
}
void message::async_read_request(request_t & request,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    std::size_t size=getLineSize(read);
    if (size!=-1){
        std::string line(read.c_str(),size);
        read.erase(0,size);
        size=getSpaceSize(line);
        if (size!=-1){
            line.erase(0,size);
        }
        size=getTokenSize(line);
        if (size!=-1){
            request.method.assign(line.c_str(),size);
            line.erase(0,size);
        } else {
            request.method.clear();
        }
        if (request.method.empty()){
            async_read_request(request,handler);
        } else {
            size=getSpaceSize(line);
            if (size!=-1){
                line.erase(0,size);
            }
            size=getTokenSize(line);
            if (size!=-1){
                request.uri.assign(line.c_str(),size);
                line.erase(0,size);
            } else {
                request.uri.clear();
            }
            size=getSpaceSize(line);
            if (size!=-1){
                line.erase(0,size);
            }
            size=getTokenSize(line);
            if (size!=-1){
                request.version.assign(line.c_str(),size);
                line.erase(0,size);
            } else {
                request.version.clear();
            }
            ict::asio::ioService().post([handler](){
                ict::asio::error_code_t ok;
                handler(ok);
            });
        }
    } else if (maxRead<read.size()){
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ec(EMSGSIZE,std::generic_category());
            handler(ec);
        });
    } else if (connection){
        connection->async_read_string(read,[this,self,handler,&request](const ict::asio::error_code_t & ec){
            if (ec){
               handler(ec);
            } else {
                async_read_request(request,handler);
            }
        });
    } else {
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
            handler(ec);
        });
    }
}
void message::async_write_response(response_t & response,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    if (!response.version.empty()){
        std::size_t size=0;
        size=getSpaceSize(response.version);
        if (size!=-1){
            response.version.erase(0,size);
        }
        size=getTokenSize(response.version);
        if (size!=-1){
            write.append(response.version.c_str(),size);
        } else {
            write.append(response.version);
        }
        response.version.clear();
        write.append(_SPACE_);
        size=getSpaceSize(response.code);
        if (size!=-1){
            response.code.erase(0,size);
        }
        size=getTokenSize(response.code);
        if (size!=-1){
            write.append(response.code.c_str(),size);
        } else {
            write.append(response.code);
        }
        response.code.clear();
        write.append(_SPACE_);
        size=getSpaceSize(response.explanation);
        if (size!=-1){
            response.explanation.erase(0,size);
        }
        size=getPhraseSize(response.explanation);
        if (size!=-1){
            write.append(response.explanation.c_str(),size);
        } else {
            write.append(response.explanation);
        }
        response.explanation.clear();
        write.append(_ENDL_);
        minWrite=min;
    }
    if (minWrite<write.size()){
        if (connection){
            connection->async_write_string(write,[this,self,handler,&response](const ict::asio::error_code_t & ec){
                if (ec){
                    handler(ec);
                } else {
                    async_write_response(response,handler);
                }
            });   
        } else {
            ict::asio::ioService().post([handler](){
                ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
                handler(ec);
            });
        }
    } else {
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ok;
            handler(ok);
        });
    }
}
void message::async_read_response(response_t & response,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    std::size_t size=getLineSize(read);
    if (size!=-1){
        std::string line(read.c_str(),size);
        read.erase(0,size);
        size=getSpaceSize(line);
        if (size!=-1){
            line.erase(0,size);
        }
        size=getTokenSize(line);
        if (size!=-1){
            response.version.assign(line.c_str(),size);
            line.erase(0,size);
        } else {
            response.version.clear();
        }
        if (response.version.empty()){
            async_read_response(response,handler);
        } else {
            size=getSpaceSize(line);
            if (size!=-1){
                line.erase(0,size);
            }
            size=getTokenSize(line);
            if (size!=-1){
                response.code.assign(line.c_str(),size);
                line.erase(0,size);
            } else {
                response.code.clear();
            }
            size=getSpaceSize(line);
            if (size!=-1){
                line.erase(0,size);
            }
            size=getPhraseSize(line);
            if (size!=-1){
                response.explanation.assign(line.c_str(),size);
                line.erase(0,size);
            } else {
                response.explanation.clear();
            }
            ict::asio::ioService().post([handler](){
                ict::asio::error_code_t ok;
                handler(ok);
            });
        }
    } else if (maxRead<read.size()){
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ec(EMSGSIZE,std::generic_category());
            handler(ec);
        });
    } else if (connection){
        connection->async_read_string(read,[this,self,handler,&response](const ict::asio::error_code_t & ec){
            if (ec){
                handler(ec);
            } else {
                async_read_response(response,handler);
            }
        });
    } else {
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
            handler(ec);
        });
    }
}
void message::async_write_header(header_t & header,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    if (!header.name.empty()){
        std::size_t size=0;
        size=getSpaceSize(header.name);
        if (size!=-1){
            header.name.erase(0,size);
        }
        size=getNameSize(header.name);
        if (size!=-1){
            write.append(header.name.c_str(),size);
        } else {
            write.append(header.name);
        }
        header.name.clear();
        if (size) {
            bool first=true;
            write.append(_COLON_);
            write.append(_SPACE_);
            size=getSpaceSize(header.value);
            if (size!=-1){
                header.value.erase(0,size);
            }
            while (size=getPhraseSize(header.value)){
                if (first){
                    first=false;
                } else {
                    write.append(_ENDL_);
                    write.append(_SPACE_);
                }
                if (size!=-1) {
                    write.append(header.value.c_str(),size);
                    header.value.erase(0,size);
                } else {
                    write.append(header.value);
                    header.value.clear();
                    break;
                }
                size=getSpaceSize(header.value);
                if (size!=-1){
                    header.value.erase(0,size);
                }
            }
            minWrite=min;
        } else {
            minWrite=0;
        }
        header.value.clear();
        write.append(_ENDL_);
    }
    if (minWrite<write.size()){
        if (connection){
            connection->async_write_string(write,[this,self,handler,&header](const ict::asio::error_code_t & ec){
                if (ec){
                    handler(ec);
                } else {
                    async_write_header(header,handler);
                }
            });   
        } else {
            ict::asio::ioService().post([handler](){
                ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
                handler(ec);
            });
        }
    } else {
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ok;
            handler(ok);
        });
    }
}
void message::async_read_header(header_t & header,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    std::size_t size=getLineSize(read);
    if (size!=-1){
        std::string line(read.c_str(),size);
        read.erase(0,size);
        size=getSpaceSize(line);
        if (size!=-1){
            line.erase(0,size);
        }
        size=getNameSize(line);
        if (size!=-1){
            header.name.assign(line.c_str(),size);
            line.erase(0,size);
        } else {
            header.name.clear();
        }
        header.value.clear();
        if (header.name.empty()){
            header.name.assign(_COLON_);
        } else {
            size=getSpaceColonSize(line);
            if (size!=-1){
                line.erase(0,size);
            }
            while (size=getPhraseSize(line)){
                if (size!=-1){
                    if (!header.value.empty()) header.value.append(1,'\n');
                    header.value.append(line.c_str(),size);
                    line.erase(0,size);
                    size=getSpaceSize(line);
                    if (size!=-1){
                        line.erase(0,size);
                    }
                } else {
                    break;
                }
            }
        }
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ok;
            handler(ok);
        });
    } else if (maxRead<read.size()){
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ec(EMSGSIZE,std::generic_category());
            handler(ec);
        });
    } else if (connection){
        connection->async_read_string(read,[this,self,handler,&header](const ict::asio::error_code_t & ec){
            if (ec){
                handler(ec);    
            } else {
                async_read_header(header,handler);
            }
        });
    } else {
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
            handler(ec);
        });
    }
}
void message::async_write_body(const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    if (write.empty()){
        ict::asio::error_code_t ok;
        handler(ok);
    } else {
        if (connection){
            connection->async_write_string(write,[this,self,handler](const ict::asio::error_code_t & ec){
                if (ec){
                    handler(ec);
                } else {
                    async_write_body(handler);
                }
            });
        } else {
            ict::asio::ioService().post([handler](){
                ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
                handler(ec);
            });
        }
    }
}
void message::async_write_body(std::string & data,std::size_t & bytesLeft,const handler_t &handler){
    std::size_t size=(bytesLeft<data.size())?bytesLeft:data.size();
    bytesLeft-=size;
    write.append(data.c_str(),size);
    data.erase(0,size);
    async_write_body(handler);
}
void message::async_read_body(const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    if (connection){
        connection->async_read_string(read,[this,self,handler](const ict::asio::error_code_t & ec){
            handler(ec);
        });
    } else {
        ict::asio::ioService().post([handler](){
            ict::asio::error_code_t ec(ENOTCONN,std::generic_category());
            handler(ec);
        });
    }
}
void message::async_read_body(std::string & data,std::size_t & bytesLeft,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    async_read_body([this,self,handler,&data,&bytesLeft](const ict::asio::error_code_t & ec){
        if (ec){
            handler(ec);
        } else {
            ict::asio::error_code_t ok;
            std::size_t size=(bytesLeft<read.size())?bytesLeft:read.size();
            bytesLeft-=size;
            data.append(read.c_str(),size);
            read.erase(0,size);
            handler(ok);
        }
    });
}

void message::async_write_headers(headers_t & headers,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    if (headers.empty()){
        ict::asio::error_code_t ok;
        handler(ok);
    } else {
        async_write_header(headers[0],[this,self,&headers,handler](const ict::asio::error_code_t & ec){
            if (ec){
                handler(ec);
            } else {
                headers.erase(headers.begin());
                async_write_headers(headers,handler);
            }
        });
    }
}
void message::async_read_headers(headers_t & headers,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    headers.emplace_back();
    async_read_header(headers[headers.size()-1],[this,self,&headers,handler](const ict::asio::error_code_t & ec){
        if (ec){
            handler(ec);
        } else {
            if (headers.back().name.empty()||headers.back().name==":") {
                ict::asio::error_code_t ok;
                handler(ok);
            } else {
                async_read_headers(headers,handler);
            }
        }
    });
}
void message::async_write_request_headers(request_headers_t & request,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    async_write_request(request.request,[this,self,&request,handler](const ict::asio::error_code_t & ec){
        if (ec){
            handler(ec);
        } else {
            async_write_headers(request.headers,handler);
        }
    });
}
void message::async_read_request_headers(request_headers_t & request,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    async_read_request(request.request,[this,self,&request,handler](const ict::asio::error_code_t & ec){
        if (ec){
            handler(ec);
        } else {
            async_read_headers(request.headers,handler);
        }
    });
}
void message::async_write_response_headers(response_headers_t & response,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    async_write_response(response.response,[this,self,&response,handler](const ict::asio::error_code_t & ec){
        if (ec){
            handler(ec);
        } else {
            async_write_headers(response.headers,handler);
        }
    });
}
void message::async_read_response_headers(response_headers_t & response,const handler_t &handler){
    auto self(enable_shared_t::shared_from_this());
    async_read_response(response.response,[this,self,&response,handler](const ict::asio::error_code_t & ec){
        if (ec){
            handler(ec);
        } else {
            async_read_headers(response.headers,handler);
        }
    });
}
message_ptr get(string_ptr iface){
    return message_ptr(new message(iface));
}
message_ptr getMessage(interface_ptr iface){
    return get(get(iface));
}
message_ptr getMessage(string_ptr iface){
    return get(iface);
}
message_ptr getMessage(::asio::ip::tcp::socket & socket){
    return get(get(get(socket)));
}
message_ptr getMessage(::asio::local::stream_protocol::socket & socket){
    return get(get(get(socket)));
}
message_ptr getMessage(::asio::ip::tcp::socket & socket,context_ptr & context,const std::string & setSNI){
    return get(get(get(socket,context,setSNI)));
}
message_ptr getMessage(::asio::local::stream_protocol::socket & socket,context_ptr & context,const std::string & setSNI){
    return get(get(get(socket,context,setSNI)));
}
//============================================
}}}
//============================================
#ifdef ENABLE_TESTING
#include "test.hpp"
#include "connector.hpp"

static const ict::asio::connection::message::response_headers_t server_example={
    .response={
        .version="HTTP/1.1",
        .code="200",
        .explanation="OK",
    },
    .headers={
        {
            .name="Date",
            .value="Thu, 20 Dec 2021 12:04:30 GMT",
        },
        {
            .name=":",
            .value="",
        },
    }
};
static const ict::asio::connection::message::request_headers_t client_example={
    .request={
        .method="POST",
        .uri="/",
        .version="HTTP/1.1",
    },
    .headers={
        {
            .name="Host",
            .value="example.com",
        },
        {
            .name=":",
            .value="",
        },
    }
};

static int test__connection(ict::asio::connection::context_ptr & s_ctx,ict::asio::connection::context_ptr & c_ctx){
  ict::asio::ioSignal();
  ict::asio::ioRun();
  {
    std::atomic<int> k=4;
    std::string port;
    ::asio::steady_timer t(ict::asio::ioService());
    ict::asio::connection::message::response_headers_t s_write_buffer=server_example;
    ict::asio::connection::message::request_headers_t s_read_buffer;
    ict::asio::connection::message::request_headers_t c_write_buffer=client_example;
    ict::asio::connection::message::response_headers_t c_read_buffer;
    srand(time(NULL));

    t.expires_from_now(std::chrono::seconds(60));
    t.async_wait(
      [](const ict::asio::error_code_t& ec){
        ict::asio::ioService().stop();
      }
    );

    port="300"+std::to_string(rand()%90+10);
    std::cout<<port<<std::endl;
    ict::asio::connector::interface_ptr s1(ict::asio::connector::get("localhost",port,s_ctx,true));
    ict::asio::connector::interface_ptr c1(ict::asio::connector::get("localhost",port,c_ctx,false,"c1"));
    ict::asio::connection::message_ptr s1c;
    ict::asio::connection::message_ptr c1c;
    
    s1->async_connection([&](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-100;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      }
      if (ptr) {
        std::cout<<"s1 "<<k<<" "<<ptr->getInfo()<<" "<<ptr->getSNI()<<std::endl;
        s1c=ict::asio::connection::getMessage(ptr);
        s1c->async_write_response_headers(s_write_buffer,[&](const ict::asio::error_code_t& ec){
          if (ec){
            k=-200;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
          } else {
            k--;
          }
          if (k<=0) ict::asio::ioService().stop();
        });
        s1c->async_read_request_headers(s_read_buffer,[&](const ict::asio::error_code_t& ec){
          if (ec){
            k=-400;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
          } else {
            k--;
            if (
                s_read_buffer.request.method!=client_example.request.method ||
                s_read_buffer.request.uri!=client_example.request.uri ||
                s_read_buffer.request.version!=client_example.request.version ||
                s_read_buffer.headers.size()!=client_example.headers.size()  ||
                s_read_buffer.headers.at(0).name!=client_example.headers.at(0).name ||
                s_read_buffer.headers.at(0).value!=client_example.headers.at(0).value
            ){
              k=-500;
              std::cerr<<__LINE__<<"|"<<s_read_buffer.request.method<<"|"<<s_read_buffer.request.uri<<"|"<<s_read_buffer.request.version<<std::endl;
            }
          }
          if (k<=0) ict::asio::ioService().stop();
        });
      }
    });

    usleep(5000);
    c1->async_connection([&](const ict::asio::error_code_t& ec,ict::asio::connection::interface_ptr ptr){
      if (ec){
        k=-700;
        std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
      }
      if (ptr) {
        c1c=ict::asio::connection::getMessage(ptr);
        c1c->async_write_request_headers(c_write_buffer,[&](const ict::asio::error_code_t& ec){
          if (ec){
            k=-800;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
          } else {
            k--;
          }
          if (k<=0) ict::asio::ioService().stop();
        });
        c1c->async_read_response_headers(c_read_buffer,[&](const ict::asio::error_code_t& ec){
          if (ec){
            k=-1000;
            std::cerr<<__LINE__<<"|"<<ec<<"|"<<ec.message()<<std::endl;
          } else {
            k--;
            if (
                c_read_buffer.response.version!=server_example.response.version ||
                c_read_buffer.response.code!=server_example.response.code ||
                c_read_buffer.response.explanation!=server_example.response.explanation ||
                c_read_buffer.headers.size()!=server_example.headers.size()  ||
                c_read_buffer.headers.at(0).name!=server_example.headers.at(0).name ||
                c_read_buffer.headers.at(0).value!=server_example.headers.at(0).value
            ){
              k=-1100;
              std::cerr<<__LINE__<<"|"<<c_read_buffer.response.version<<"|"<<c_read_buffer.response.code<<"|"<<c_read_buffer.response.explanation<<std::endl;
            }
          }
          if (k<=0) ict::asio::ioService().stop();
        });
      }
    });

    ict::asio::ioJoin();
    if (k) return(k);
  }
  return(0);
}
REGISTER_TEST(connection_message,tc1){
  ict::asio::connection::context_ptr ctx;
  return(test__connection(ctx,ctx));
}
#endif
//===========================================
