//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_ACCEPTOR_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_ACCEPTOR_HPP


#include <cobalt/io/ops.hpp>
#include <cobalt/io/stream_socket.hpp>
#include <cobalt/io/seq_packet_socket.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>

namespace cobalt::io
{

struct acceptor : net::socket_base
{
  COBALT_IO_DECL acceptor(const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL acceptor(endpoint ep, const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL result<void> bind(endpoint ep);
  COBALT_IO_DECL result<void> listen(int backlog = max_listen_connections); // int backlog = net::max_backlog()
  COBALT_IO_DECL endpoint local_endpoint();

  struct [[nodiscard]]  accept_op
  {
    socket * sock;

    void *this_;
    void (*implementation)(void * this_, socket *,
                           boost::cobalt::completion_handler<error_code>);

    op_awaitable<accept_op, std::tuple<socket *>, error_code>
        operator co_await()
    {
      return {this, sock};
    }
  };

  accept_op accept(socket & sock)
  {
    return {&sock, this, initiate_accept_};
  }

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

 private:
  COBALT_IO_DECL static void initiate_accept_ (void *, socket *, completion_handler<error_code>);
  COBALT_IO_DECL static void initiate_wait_(void *, wait_type, completion_handler<error_code>);

  net::basic_socket_acceptor<protocol_type, executor> acceptor_;
};

}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_ACCEPTOR_HPP
