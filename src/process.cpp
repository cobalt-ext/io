//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/process.hpp>

namespace cobalt::io
{

process::process(boost::process::v2::filesystem::path executable,
                 std::initializer_list<boost::core::string_view> args,
                 process_initializer initializer,
                 const cobalt::executor & exec)
    : process_(exec,
               executable,
               args,
               initializer.stdio,
               initializer.start_dir,
               initializer.env) {}


process::process(boost::process::v2::filesystem::path executable,
                 std::span<boost::core::string_view> args,
                 process_initializer initializer,
                 const cobalt::executor & exec)
    : process_(exec,
               executable,
               args,
               initializer.stdio,
               initializer.start_dir,
               initializer.env) {}


process::process(pid_type pid, const cobalt::executor & exec) : process_(exec, pid) {}
process::process(pid_type pid, native_handle_type native_handle, const cobalt::executor & exec)
    : process_(exec, pid, native_handle) {}



pid_type process::id() const {return process_.id();}

result<void> process::interrupt()
{
  error_code ec;
  process_.interrupt(ec);
  return ec ? ec : result<void>();
}
result<void> process::request_exit()
{
  error_code ec;
  process_.request_exit(ec);
  return ec ? ec : result<void>();
}
result<void> process::suspend()
{
  error_code ec;
  process_.suspend(ec);
  return ec ? ec : result<void>();
}
result<void> process::resume()
{
  error_code ec;
  process_.resume(ec);
  return ec ? ec : result<void>();
}
result<void> process::terminate()
{
  error_code ec;
  process_.terminate(ec);
  return ec ? ec : result<void>();
}
process::handle_type process::detach()
{
  return process_.detach();
}
result<bool> process::running()
{
  error_code ec;
  auto res = process_.running(ec);
  return ec ? result<bool>(boost::system::in_place_error, ec) : result<bool>(res);
}

void process::initiate_wait_(void * this_, completion_handler<error_code, int> handler)
{
  static_cast<process*>(this_)->process_.async_wait(std::move(handler));
}


}