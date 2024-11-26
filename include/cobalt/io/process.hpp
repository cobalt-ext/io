//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_PROCESS_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_PROCESS_HPP


#include <cobalt/io/config.hpp>
#include <cobalt/io/ops.hpp>
#include <cobalt/io/pipe.hpp>

#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <boost/process/v2/start_dir.hpp>
#include <boost/process/v2/environment.hpp>
#include <boost/core/detail/string_view.hpp>
#include <boost/system/result.hpp>

#include <initializer_list>

namespace cobalt::io
{

namespace detail
{

template<int Target>
struct process_io_binding_ : boost::process::v2::detail::process_io_binding<Target>
{
  using boost::process::v2::detail::process_io_binding<Target>::process_io_binding;

  process_io_binding_(readable_pipe & rp) : boost::process::v2::detail::process_io_binding<Target>(rp.implementation_) {}
  process_io_binding_(writable_pipe & wp) : boost::process::v2::detail::process_io_binding<Target>(wp.implementation_) {}
};

}

using boost::process::v2::pid_type;

struct process_initializer
{
  struct stdio_
  {
    detail::process_io_binding_<STDIN_FILENO>  in;
    detail::process_io_binding_<STDOUT_FILENO> out;
    detail::process_io_binding_<STDERR_FILENO> err;

  };
  stdio_                                  stdio;
  boost::process::v2::process_start_dir   start_dir{boost::process::v2::filesystem::current_path()};
  boost::process::v2::process_environment env{boost::process::v2::environment::current()};
};

struct process
{
  using wait_result = result<int>;
  using handle_type = typename boost::process::v2::basic_process<executor>::handle_type;
  using native_handle_type = typename boost::process::v2::basic_process<executor>::native_handle_type;

  COBALT_IO_DECL process(boost::process::v2::filesystem::path executable,
                           std::initializer_list<boost::core::string_view> args,
                           process_initializer initializer = {},
                           const cobalt::executor & executor = this_thread::get_executor());


  COBALT_IO_DECL process(boost::process::v2::filesystem::path executable,
                           std::span<boost::core::string_view> args,
                           process_initializer initializer = {},
                           const cobalt::executor & executor = this_thread::get_executor());

  COBALT_IO_DECL process(pid_type pid, const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL process(pid_type pid, native_handle_type native_handle,
                           const cobalt::executor & executor = this_thread::get_executor());

  [[nodiscard]] COBALT_IO_DECL result<void> interrupt();
  [[nodiscard]] COBALT_IO_DECL result<void> request_exit();
  [[nodiscard]] COBALT_IO_DECL result<void> suspend();
  [[nodiscard]] COBALT_IO_DECL result<void> resume();
  [[nodiscard]] COBALT_IO_DECL result<void> terminate();
  [[nodiscard]] COBALT_IO_DECL handle_type detach();
  [[nodiscard]] COBALT_IO_DECL result<bool> running();


  [[nodiscard]] pid_type id() const;

  struct [[nodiscard]] wait_op
  {
    void *this_;
    void (*implementation)(void * this_,
                           boost::cobalt::completion_handler<error_code, int>);
    void (*try_implementation)(void * this_, boost::cobalt::handler<error_code, int>);

    op_awaitable<wait_op, std::tuple<>, error_code, int>
        operator co_await()
    {
      return {this};
    }
  };


  [[nodiscard]]  auto wait() { return wait_op{this, initiate_wait_}; }
 private:

  COBALT_IO_DECL static void initiate_wait_(void *, boost::cobalt::completion_handler<error_code, int>);

  boost::process::v2::basic_process<executor> process_;
  friend struct popen;
};


}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_PROCESS_HPP
