//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_PIPE_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_PIPE_HPP

#include <cobalt/io/config.hpp>
#include <cobalt/io/buffer.hpp>
#include <cobalt/io/ops.hpp>
#include <boost/cobalt/op.hpp>

#include <boost/asio/basic_readable_pipe.hpp>
#include <boost/asio/basic_writable_pipe.hpp>

#include <boost/system/result.hpp>

namespace cobalt::io
{

namespace detail
{

template<int Target>
struct process_io_binding_;

}

COBALT_IO_DECL
result<std::pair<struct readable_pipe, struct  writable_pipe>> pipe(
    const cobalt::executor & executor
    );


struct readable_pipe 
{
  using native_handle_type = net::basic_readable_pipe<executor>::native_handle_type;

  COBALT_IO_DECL readable_pipe(const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL readable_pipe(native_handle_type native_file, const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL readable_pipe(readable_pipe && sf) noexcept;

  COBALT_IO_DECL result<void> assign(native_handle_type native_file);
  COBALT_IO_DECL result<void> cancel();

  COBALT_IO_DECL executor get_executor();
  COBALT_IO_DECL bool is_open() const;

  COBALT_IO_DECL result<void> close();

  COBALT_IO_DECL native_handle_type native_handle();
  COBALT_IO_DECL result<native_handle_type> release();

  read_op read_some(mutable_buffer_sequence buffer)
  {
    return {buffer, this, initiate_read_some_};
  }

 private:

  COBALT_IO_DECL static void initiate_read_some_(void *, mutable_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);

  friend result<std::pair<struct readable_pipe, struct writable_pipe>> pipe(const cobalt::executor & executor);
  net::basic_readable_pipe<executor> implementation_;

  template<int Target>
  friend struct detail::process_io_binding_;
};


struct writable_pipe
{
  using native_handle_type = net::basic_writable_pipe<executor>::native_handle_type;

  COBALT_IO_DECL writable_pipe(const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL writable_pipe(native_handle_type native_file, const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL writable_pipe(writable_pipe && sf) noexcept;

  COBALT_IO_DECL result<void> assign(native_handle_type native_file);
  COBALT_IO_DECL result<void> cancel();

  COBALT_IO_DECL executor get_executor();
  COBALT_IO_DECL bool is_open() const;

  write_op write_some(const_buffer_sequence buffer)
  {
    return {buffer, this, initiate_write_some_};
  }

  COBALT_IO_DECL result<void> close();

  COBALT_IO_DECL native_handle_type native_handle();

  COBALT_IO_DECL result<native_handle_type> release();

 private:
  COBALT_IO_DECL static void initiate_write_some_(void *, const_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);

  friend result<std::pair<struct readable_pipe, struct  writable_pipe>> pipe(const cobalt::executor & executor);
  net::basic_writable_pipe<executor> implementation_;

  template<int Target>
  friend struct detail::process_io_binding_;
};



}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_PIPE_HPP
