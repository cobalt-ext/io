//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/serial_port.hpp>

#include <cobalt/io/initiate_templates.hpp>

namespace cobalt::io
{

result<void> serial_port::close()
{
  error_code ec;
  serial_port_.close(ec);
  return ec ? ec : result<void>();
}

result<void> serial_port::cancel()
{
  error_code ec;
  serial_port_.cancel(ec);
  return ec ? ec : result<void>();
}

bool serial_port::is_open() const { return serial_port_.is_open(); }

[[nodiscard]] result<void> serial_port::send_break()
{
  error_code ec;
  serial_port_.send_break(ec);
  return ec ? ec : result<void>{};
}

result<void>     serial_port::set_baud_rate(unsigned rate)
{
  error_code ec;
  serial_port_.set_option(net::serial_port_base::baud_rate(rate), ec);
  return ec ? ec : result<void>();
}

result<unsigned> serial_port::get_baud_rate()
{
  error_code ec;
  net::serial_port_base::baud_rate br;
  serial_port_.get_option(br, ec);
  return ec ? ec : result<unsigned>(br.value());
}

result<void>     serial_port::set_character_size(unsigned rate)
{
  error_code ec;
  serial_port_.set_option(net::serial_port_base::character_size(rate), ec);
  return ec ? ec : result<void>();
}

result<unsigned> serial_port::get_character_size()
{
  error_code ec;
  net::serial_port_base::character_size br;
  serial_port_.get_option(br, ec);
  return ec ? ec : result<unsigned>(br.value());
}


result<void>     serial_port::set_flow_control(flow_control rate)
{
  error_code ec;
  serial_port_.set_option(net::serial_port_base::flow_control(rate), ec);
  return ec ? ec : result<void>();
}

auto serial_port::get_flow_control() -> result<flow_control>
{
  error_code ec;
  net::serial_port_base::flow_control br;
  serial_port_.get_option(br, ec);
  return ec ? ec : result<flow_control>(br.value());
}


result<void>     serial_port::set_parity(parity rate)
{
  error_code ec;
  serial_port_.set_option(net::serial_port_base::parity(rate), ec);
  return ec ? ec : result<void>();
}

auto serial_port::get_parity() -> result<parity>
{
  error_code ec;
  net::serial_port_base::parity br;
  serial_port_.get_option(br, ec);
  return ec ? ec : result<parity>(br.value());
}

serial_port::serial_port(const cobalt::executor & executor)
    : serial_port_(executor) {}
serial_port::serial_port(std::string_view device, const cobalt::executor & executor)
    : serial_port_(executor, std::string(device)) {}
serial_port::serial_port(native_handle_type native_handle, const cobalt::executor & executor)
    : serial_port_(executor, native_handle) {}

result<void> serial_port::assign(native_handle_type native_handle)
{
  error_code ec;
  serial_port_.assign(native_handle, ec);
  return ec ? ec : result<void>{};
}

[[nodiscard]] result<void> serial_port::open(std::string_view device)
{
  std::string dev{device};
  error_code ec;
  serial_port_.open(dev, ec);
  if (ec)
    return ec;
  else
    return boost::system::in_place_value;
}

auto serial_port::release() -> result<native_handle_type>
{
  // ain't done in asio, for some reason.
  constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION};
  error_code ec{net::error::operation_not_supported, &loc};
  return ec;
}


void serial_port::initiate_read_some_(void * this_, mutable_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  initiate_async_read_some(static_cast<serial_port*>(this_)->serial_port_, buffer, std::move(handler));
}
void serial_port::initiate_write_some_(void * this_, const_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  initiate_async_write_some(static_cast<serial_port*>(this_)->serial_port_, buffer, std::move(handler));
}


}