//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_SSL_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_SSL_HPP

#include <cobalt/io/socket.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/basic_stream_socket.hpp>

namespace cobalt::io
{

namespace detail
{

struct ssl_stream_base
{

  template<typename ... Args>
  ssl_stream_base(Args &&...args) : ssl_stream_(std::forward<Args>(args)...) {}

  net::ssl::stream<net::basic_stream_socket<io::protocol_type, executor>> ssl_stream_;
};

}

 struct ssl_stream : private detail::ssl_stream_base, net::ssl::stream_base, socket
{
  using shutdown_result = result<void>;

  COBALT_IO_DECL ssl_stream(const executor & executor = this_thread::get_executor());
  COBALT_IO_DECL ssl_stream(ssl_stream && steam);
  COBALT_IO_DECL ssl_stream(stream_socket && socket);

  COBALT_IO_DECL ssl_stream(net::ssl::context & ctx, const executor & executor = this_thread::get_executor());
  COBALT_IO_DECL ssl_stream(net::ssl::context & ctx, stream_socket && socket);

  write_op write_some(const_buffer_sequence buffer)
  {
    return {buffer, this, initiate_write_some_};
  }
  read_op read_some(mutable_buffer_sequence buffer)
  {
    return {buffer, this, initiate_read_some_};
  }

   struct handshake_op
   {
     handshake_type ht;

     void *this_;
     void (*implementation)(void * this_, handshake_type,
                            boost::cobalt::completion_handler<error_code>);

     op_awaitable<handshake_op, std::tuple<handshake_type>, error_code>
         operator co_await()
     {
       return {this, ht};
     }
   };

   struct buffered_handshake_op
   {
     handshake_type ht;
     const_buffer_sequence buffer;

     void *this_;
     void (*implementation)(void * this_, handshake_type, const_buffer_sequence,
                            boost::cobalt::completion_handler<error_code, std::size_t>);

     op_awaitable<buffered_handshake_op, std::tuple<handshake_type, const_buffer_sequence>, error_code, std::size_t>
         operator co_await()
     {
       return {this, ht, buffer};
     }
   };

   handshake_op handshake(handshake_type ht)
   {
     return {ht, this,  initiate_handshake_};
   }

   buffered_handshake_op handshake(handshake_type ht, const_buffer_sequence buffer)
   {
     return {ht, buffer, this, initiate_buffered_handshake_};
   }

   io::wait_op shutdown()
   {
     return {this, initiate_shutdown_};
   }


 private:
  COBALT_IO_DECL static void initiate_read_some_ (void *, mutable_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_write_some_(void *, const_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_shutdown_(void *, boost::cobalt::completion_handler<error_code>);
  COBALT_IO_DECL static void initiate_handshake_(void *, handshake_type, boost::cobalt::completion_handler<error_code>);
  COBALT_IO_DECL static void initiate_buffered_handshake_(void *, handshake_type, const_buffer_sequence,
                                                             boost::cobalt::completion_handler<error_code, std::size_t>);

  void adopt_endpoint_(endpoint & ep) override;
};

}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_SSL_HPP
