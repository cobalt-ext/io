//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#include <cobalt/io/popen.hpp>
#include <cobalt/io/initiate_templates.hpp>

namespace cobalt::io
{

popen::popen(boost::process::v2::filesystem::path executable,
             std::initializer_list<boost::core::string_view> args,
             process_initializer initializer,
             const cobalt::executor & exec)
    : popen_(exec,
             executable,
             args,
             initializer.stdio,
             initializer.start_dir,
             initializer.env) {}


popen::popen(boost::process::v2::filesystem::path executable,
             std::span<boost::core::string_view> args,
             process_initializer initializer,
             const cobalt::executor & exec)
    : popen_(exec,
             executable,
             args,
             initializer.stdio,
             initializer.start_dir,
             initializer.env) {}

pid_type popen::id() const {return popen_.id();}

result<void> popen::interrupt()
{
  error_code ec;
  popen_.interrupt(ec);
  return ec ? ec : result<void>();
}
result<void> popen::request_exit()
{
  error_code ec;
  popen_.request_exit(ec);
  return ec ? ec : result<void>();
}
result<void> popen::suspend()
{
  error_code ec;
  popen_.suspend(ec);
  return ec ? ec : result<void>();
}
result<void> popen::resume()
{
  error_code ec;
  popen_.resume(ec);
  return ec ? ec : result<void>();
}
result<void> popen::terminate()
{
  error_code ec;
  popen_.terminate(ec);
  return ec ? ec : result<void>();
}
popen::handle_type popen::detach()
{
  return popen_.detach();
}
result<bool> popen::running()
{
  error_code ec;
  auto res = popen_.running(ec);
  return ec ? result<bool>(boost::system::in_place_error, ec) : result<bool>(res);
}

result<void> popen::close()
{
  return this->terminate();
}

result<void> popen::cancel()
{
  error_code ec;
  popen_.get_stdin().cancel(ec);
  if (ec)
    return ec;
  popen_.get_stdout().cancel(ec);
  if (ec)
    return ec;
  return {};
}

bool popen::is_open() const
{
  return this->popen_.is_open();
}

void popen::initiate_wait_      (void * this_,                          completion_handler<error_code, int> handler)
{
  static_cast<popen*>(this_)->popen_.async_wait(std::move(handler));
}

void popen::initiate_read_some_ (void * this_, mutable_buffer_sequence buffer, completion_handler<error_code, std::size_t> handler)
{
  initiate_async_read_some(static_cast<popen*>(this_)->popen_, buffer, std::move(handler));
}
void popen::initiate_write_some_(void * this_, const_buffer_sequence buffer,   completion_handler<error_code, std::size_t> handler)
{
  initiate_async_write_some(static_cast<popen*>(this_)->popen_, buffer, std::move(handler));
}


}