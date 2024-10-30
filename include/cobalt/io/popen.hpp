//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_EXPERIMENTAL_IO_COBALT_POPEN_HPP
#define BOOST_EXPERIMENTAL_IO_COBALT_POPEN_HPP

#include <cobalt/io/config.hpp>
#include <cobalt/io/process.hpp>

#include <boost/process/v2/popen.hpp>

namespace cobalt::io
{

struct popen
{
  using wait_result = result<int>;
  using handle_type = typename boost::process::v2::basic_process<executor>::handle_type;
  using native_handle_type = typename boost::process::v2::basic_process<executor>::native_handle_type;

  COBALT_IO_DECL popen(boost::process::v2::filesystem::path executable,
                         std::initializer_list<boost::core::string_view> args,
                         process_initializer initializer = {},
                         const cobalt::executor & executor = this_thread::get_executor());


  COBALT_IO_DECL popen(boost::process::v2::filesystem::path executable,
                         std::span<boost::core::string_view> args,
                         process_initializer initializer = {},
                         const cobalt::executor & executor = this_thread::get_executor());

  [[nodiscard]] COBALT_IO_DECL result<void> interrupt();
  [[nodiscard]] COBALT_IO_DECL result<void> request_exit();
  [[nodiscard]] COBALT_IO_DECL result<void> suspend();
  [[nodiscard]] COBALT_IO_DECL result<void> resume();
  [[nodiscard]] COBALT_IO_DECL result<void> terminate();
  [[nodiscard]] COBALT_IO_DECL handle_type detach();
  [[nodiscard]] COBALT_IO_DECL result<bool> running();


  [[nodiscard]] pid_type id() const;

  [[nodiscard]] result<void> close();
  [[nodiscard]] result<void> cancel();
  [[nodiscard]] bool is_open() const;

  process::wait_op wait()
  {
    return process::wait_op{this, initiate_wait_};
  }

  write_op write_some(const_buffer_sequence buffer)
  {
    return {buffer, this, initiate_write_some_};
  }
  read_op read_some(mutable_buffer_sequence buffer)
  {
    return {buffer, this, initiate_read_some_};
  }


 private:
  COBALT_IO_DECL static void initiate_wait_      (void *,                          completion_handler<error_code, int>);
  COBALT_IO_DECL static void initiate_read_some_ (void *, mutable_buffer_sequence, completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_write_some_(void *, const_buffer_sequence,   completion_handler<error_code, std::size_t>);

  boost::process::v2::basic_popen<executor> popen_;
};

}

#endif //BOOST_EXPERIMENTAL_IO_COBALT_POPEN_HPP
