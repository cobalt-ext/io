//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_SERIAL_PORT_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_SERIAL_PORT_HPP

#include <boost/cobalt/op.hpp>
#include <cobalt/io/ops.hpp>

#include <boost/asio/basic_serial_port.hpp>
#include <boost/core/detail/string_view.hpp>
#include <boost/system/result.hpp>

namespace cobalt::io
{


struct [[nodiscard]] serial_port final 
{
  COBALT_IO_DECL result<void> close();
  COBALT_IO_DECL result<void> cancel();
  COBALT_IO_DECL bool is_open() const;

  [[nodiscard]] COBALT_IO_DECL result<void> send_break();

  [[nodiscard]] COBALT_IO_DECL result<void>     set_baud_rate(unsigned rate);
  [[nodiscard]] COBALT_IO_DECL result<unsigned> get_baud_rate();

  [[nodiscard]] COBALT_IO_DECL result<void>     set_character_size(unsigned rate);
  [[nodiscard]] COBALT_IO_DECL result<unsigned> get_character_size();

  using flow_control = net::serial_port_base::flow_control::type;

  [[nodiscard]] COBALT_IO_DECL result<void>         set_flow_control(flow_control rate);
  [[nodiscard]] COBALT_IO_DECL result<flow_control> get_flow_control();

  using parity = net::serial_port_base::parity::type;

  [[nodiscard]] COBALT_IO_DECL result<void>   set_parity(parity rate);
  [[nodiscard]] COBALT_IO_DECL result<parity> get_parity();

  using native_handle_type = typename net::basic_serial_port<executor>::native_handle_type;
  native_handle_type native_handle() {return serial_port_.native_handle();}

  COBALT_IO_DECL serial_port(const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL serial_port(serial_port && lhs) = default;
  COBALT_IO_DECL serial_port(std::string_view device,         const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL serial_port(native_handle_type native_handle, const cobalt::executor & executor = this_thread::get_executor());

  [[nodiscard]] COBALT_IO_DECL result<void> assign(native_handle_type native_handle);
  [[nodiscard]] COBALT_IO_DECL result<native_handle_type> release();

  COBALT_IO_DECL [[nodiscard]] result<void> open(std::string_view device);

  write_op write_some(const_buffer_sequence buffer)
  {
    return {buffer, this, initiate_write_some_};
  }
  read_op read_some(mutable_buffer_sequence buffer)
  {
    return {buffer, this, initiate_read_some_};
  }


 private:
  COBALT_IO_DECL static void initiate_read_some_(void *, mutable_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_write_some_(void *, const_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);

  net::basic_serial_port<executor> serial_port_;
};


}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_SERIAL_PORT_HPP
