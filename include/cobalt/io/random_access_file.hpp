//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_RANDOM_ACCESS_FILE_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_RANDOM_ACCESS_FILE_HPP

#include <boost/asio/basic_random_access_file.hpp>

#include <cobalt/io/config.hpp>
#include <cobalt/io/buffer.hpp>
#include <cobalt/io/file.hpp>
#include <cobalt/io/ops.hpp>
#include <boost/cobalt/noop.hpp>
#include <boost/cobalt/op.hpp>

namespace cobalt::io
{


struct random_access_file : file
{
  using native_handle_type = file::native_handle_type;

  COBALT_IO_DECL random_access_file(const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL random_access_file(const char * path, file::flags open_flags,
                                       const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL random_access_file(const std::string & path, file::flags open_flags,
                                       const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL random_access_file(const native_handle_type & native_file,
                                       const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL random_access_file(random_access_file && sf) noexcept;

  write_at_op write_some_at(std::uint64_t offset, const_buffer_sequence buffer)
  {
    return { offset, buffer, this, initiate_write_some_at_};
  }

  read_at_op read_some_at(std::uint64_t offset, mutable_buffer_sequence buffer)
  {
    return { offset, buffer, this, initiate_read_some_at_};
  }


 private:
  COBALT_IO_DECL static void initiate_read_some_at_(void *, std::uint64_t,  mutable_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_write_some_at_(void *, std::uint64_t, const_buffer_sequence,   boost::cobalt::completion_handler<error_code, std::size_t>);
#if defined(BOOST_ASIO_HAS_FILE)
  net::basic_random_access_file<executor> implementation_;
#endif
};


}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_RANDOM_ACCESS_FILE_HPP
