//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/ssl.hpp>

#include <cobalt/io/initiate_templates.hpp>
#include <cobalt/io/socket.hpp>
#include <cobalt/io/stream_socket.hpp>


namespace cobalt::io
{

static net::ssl::context & get_ssl_context()
{
  thread_local static net::ssl::context ctx{net::ssl::context_base::tlsv13};
  return ctx;
}


ssl_stream::ssl_stream(const cobalt::executor & exec)
    : ssl_stream_base(exec, get_ssl_context()), socket(ssl_stream_.next_layer())
{
}

ssl_stream::ssl_stream(ssl_stream && lhs)
    : ssl_stream_base(std::move(lhs.ssl_stream_)), socket(ssl_stream_.next_layer())
{
}

ssl_stream::ssl_stream(stream_socket && socket_)
    : ssl_stream_base(std::move(socket_.stream_socket_), get_ssl_context()), socket(ssl_stream_.next_layer()) {}

ssl_stream::ssl_stream(net::ssl::context & ctx, const cobalt::executor & exec)
    : ssl_stream_base(exec, ctx), socket(ssl_stream_.next_layer()) {}

ssl_stream::ssl_stream(net::ssl::context & ctx, stream_socket && socket_)
    : ssl_stream_base(std::move(socket_.stream_socket_), ctx), socket(ssl_stream_.next_layer()) {}



void ssl_stream::adopt_endpoint_(endpoint & ep)
{

  switch (ep.protocol().family())
  {
    case BOOST_ASIO_OS_DEF(AF_INET): BOOST_FALLTHROUGH;
    case BOOST_ASIO_OS_DEF(AF_INET6):
      if (ep.protocol().protocol() == BOOST_ASIO_OS_DEF(IPPROTO_IP))
        ep.set_protocol(BOOST_ASIO_OS_DEF(IPPROTO_TCP));
    case AF_UNIX:
      if (ep.protocol().type() == 0)
        ep.set_type(BOOST_ASIO_OS_DEF(SOCK_STREAM));
  }
}

void ssl_stream::initiate_read_some_(void *this_ , mutable_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  auto th = static_cast<ssl_stream*>(this_);
  if (th->mode_ == 3)
    initiate_async_read_some(th->ssl_stream_.next_layer(), buffer, std::move(handler));
  else
    initiate_async_read_some(th->ssl_stream_,              buffer, std::move(handler));

}

void ssl_stream::initiate_write_some_   (void * this_, const_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  auto th = static_cast<ssl_stream*>(this_);
  if (th->mode_ == 3)
    initiate_async_write_some(th->ssl_stream_.next_layer(), buffer, std::move(handler));
  else
    initiate_async_write_some(th->ssl_stream_,              buffer, std::move(handler));
}

void ssl_stream::initiate_shutdown_(void * this_, boost::cobalt::completion_handler<error_code> handler)
{
  auto t = static_cast<ssl_stream*>(this_);
  t->ssl_stream_.async_shutdown(
      boost::asio::deferred(
          [t](error_code ec)
          {
            if (!ec)
              t->mode_ &= ~1;
            return boost::asio::deferred.values(ec);
          }))(std::move(handler));
}

void ssl_stream::initiate_handshake_(void *this_, handshake_type ht, boost::cobalt::completion_handler<error_code> handler)
{
  auto t = static_cast<ssl_stream*>(this_);
  t->ssl_stream_.async_handshake(
      ht, boost::asio::deferred(
          [t](error_code ec)
          {
            if (!ec)
              t->mode_ |= 1;
            return boost::asio::deferred.values(ec);
          }))(std::move(handler));
}

void ssl_stream::initiate_buffered_handshake_(void * this_, handshake_type ht, const_buffer_sequence seq,
                                              boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  auto t = static_cast<ssl_stream*>(this_);
  auto & str = t->ssl_stream_;

  auto d = boost::asio::deferred(
              [t](error_code ec, std::size_t n)
              {
                if (!ec)
                  t->mode_ |= 1;
                return boost::asio::deferred.values(ec, n);
              });

  if (seq.buffer_count() > 0u)
    str.async_handshake(ht, seq, d)(std::move(handler));
  else
    str.async_handshake(ht, seq.head, d)(std::move(handler));
}



}
