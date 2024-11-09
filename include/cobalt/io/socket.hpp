//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_SOCKET_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_SOCKET_HPP

#include <cobalt/io/endpoint.hpp>
#include <cobalt/io/ops.hpp>
#include <boost/asio/socket_base.hpp>
#include <boost/asio/basic_socket.hpp>


namespace cobalt::io
{

struct socket
{
  [[nodiscard]] result<void> open(protocol_type prot = protocol_type {});
  [[nodiscard]] result<void> close();
  [[nodiscard]] result<void> cancel();
  [[nodiscard]] bool is_open() const;

  // asio acceptor compatibility
  template<typename T>
  struct rebind_executor {using other = socket;};

  using shutdown_type      = net::socket_base::shutdown_type;
  using wait_type          = net::socket_base::wait_type;
  using message_flags      = net::socket_base::message_flags;
  constexpr static int message_peek          = net::socket_base::message_peek;
  constexpr static int message_out_of_band   = net::socket_base::message_out_of_band;
  constexpr static int message_do_not_route  = net::socket_base::message_do_not_route;
  constexpr static int message_end_of_record = net::socket_base::message_end_of_record;

  using native_handle_type = net::basic_socket<protocol_type, executor>::native_handle_type;
  native_handle_type native_handle();

  [[nodiscard]] COBALT_IO_DECL result<void> shutdown(shutdown_type = shutdown_type::shutdown_both);

  [[nodiscard]] COBALT_IO_DECL result<endpoint> local_endpoint() const;
  [[nodiscard]] COBALT_IO_DECL result<endpoint> remote_endpoint() const;


  COBALT_IO_DECL result<void> assign(protocol_type protocol, native_handle_type native_handle);
  COBALT_IO_DECL result<native_handle_type> release();

  /// copied from what asio does
  [[nodiscard]] COBALT_IO_DECL result<std::size_t> bytes_readable();

  [[nodiscard]] COBALT_IO_DECL result<void> set_debug(bool debug);
  [[nodiscard]] COBALT_IO_DECL result<bool> get_debug() const;

  [[nodiscard]] COBALT_IO_DECL result<void> set_do_not_route(bool do_not_route);
  [[nodiscard]] COBALT_IO_DECL result<bool> get_do_not_route() const;

  [[nodiscard]] COBALT_IO_DECL result<void> set_enable_connection_aborted(bool enable_connection_aborted);
  [[nodiscard]] COBALT_IO_DECL result<bool> get_enable_connection_aborted() const;

  [[nodiscard]] COBALT_IO_DECL result<void> set_keep_alive(bool keep_alive);
  [[nodiscard]] COBALT_IO_DECL result<bool> get_keep_alive() const;

  [[nodiscard]] COBALT_IO_DECL result<void> set_linger(bool linger, int timeout);
  [[nodiscard]] COBALT_IO_DECL result<std::pair<bool, int>> get_linger() const;

  [[nodiscard]] COBALT_IO_DECL result<void>        set_receive_buffer_size(std::size_t receive_buffer_size);
  [[nodiscard]] COBALT_IO_DECL result<std::size_t> get_receive_buffer_size() const;

  [[nodiscard]] COBALT_IO_DECL result<void>        set_send_buffer_size(std::size_t send_buffer_size);
  [[nodiscard]] COBALT_IO_DECL result<std::size_t> get_send_buffer_size() const;

  [[nodiscard]] COBALT_IO_DECL result<void>        set_receive_low_watermark(std::size_t receive_low_watermark);
  [[nodiscard]] COBALT_IO_DECL result<std::size_t> get_receive_low_watermark() const;

  [[nodiscard]] COBALT_IO_DECL result<void>        set_send_low_watermark(std::size_t send_low_watermark);
  [[nodiscard]] COBALT_IO_DECL result<std::size_t> get_send_low_watermark() const;

  [[nodiscard]] COBALT_IO_DECL result<void> set_reuse_address(bool reuse_address);
  [[nodiscard]] COBALT_IO_DECL result<bool> get_reuse_address() const;

  struct [[nodiscard]] wait_op
  {
    wait_type wt;

    void *this_;
    void (*implementation)(void * this_, wait_type wt,
                           boost::cobalt::completion_handler<error_code>);

    op_awaitable<wait_op, std::tuple<wait_type>, error_code>
        operator co_await()
    {
      return {this, wt};
    }
  };
  wait_op     wait(wait_type wt = wait_type::wait_read)
  {
    return {wt, this, initiate_wait_};
  }

  struct [[nodiscard]] connect_op
  {
    struct endpoint endpoint;

    void *this_;
    void (*implementation)(void * this_, struct endpoint,
                           boost::cobalt::completion_handler<error_code>);

    op_awaitable<connect_op, std::tuple<struct endpoint>, error_code>
        operator co_await()
    {
      return {this, endpoint};
    }
  };
  connect_op connect(endpoint ep)
  {
    return {ep, this, initiate_connect_};
  }

  socket(net::basic_socket<protocol_type, executor> & socket) : socket_(socket) {}

 private:
  virtual void adopt_endpoint_(endpoint & ) {}

  friend struct acceptor;
  net::basic_socket<protocol_type, executor> & socket_;
  COBALT_IO_DECL static void initiate_wait_(void *, wait_type, completion_handler<error_code>);
  COBALT_IO_DECL static void initiate_connect_(void *, endpoint, completion_handler<error_code>);
};

COBALT_IO_DECL result<void> connect_pair(protocol_type protocol, socket & socket1, socket & socket2);


}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_SOCKET_HPP
