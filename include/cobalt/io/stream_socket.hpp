//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_STREAM_SOCKET_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_STREAM_SOCKET_HPP

#include <cobalt/io/endpoint.hpp>
#include <cobalt/io/socket.hpp>

#include <boost/asio/generic/datagram_protocol.hpp>
#include <boost/asio/basic_stream_socket.hpp>

namespace cobalt::io
{

struct [[nodiscard]] stream_socket final : socket
{
  COBALT_IO_DECL stream_socket(const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL stream_socket(stream_socket && lhs);
  COBALT_IO_DECL stream_socket(native_handle_type h, protocol_type protocol = protocol_type(),
                  const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL stream_socket(endpoint ep,
                  const cobalt::executor & executor = this_thread::get_executor());

  write_op write_some(const_buffer_sequence buffer)
  {
    return {buffer, this, initiate_write_some_};
  }
  read_op read_some(mutable_buffer_sequence buffer)
  {
    return {buffer, this, initiate_read_some_};
  }

 public:
  COBALT_IO_DECL void adopt_endpoint_(endpoint & ep) override;

  COBALT_IO_DECL static void initiate_read_some_ (void *, mutable_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_write_some_(void *, const_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);

  net::basic_stream_socket<protocol_type, executor> stream_socket_;
};


inline result<std::pair<stream_socket, stream_socket>> make_pair(decltype(local_stream) protocol)
{
  std::pair<stream_socket, stream_socket> res;
  auto c = connect_pair(protocol, res.first, res.second);
  if (c)
    return res;
  else
    return c.error();
}

}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_STREAM_SOCKET_HPP
