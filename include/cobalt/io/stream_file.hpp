//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_STREAM_FILE_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_STREAM_FILE_HPP

#include <boost/asio/basic_stream_file.hpp>

#include <cobalt/io/config.hpp>
#include <cobalt/io/buffer.hpp>
#include <cobalt/io/file.hpp>
#include <cobalt/io/ops.hpp>
#include <boost/cobalt/op.hpp>

#include <boost/system/result.hpp>

namespace cobalt::io
{


struct stream_file : file
{
  COBALT_IO_DECL stream_file(const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL stream_file(const char * path, file::flags open_flags,
                                const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL stream_file(const std::string & path, file::flags open_flags,
                                const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL stream_file(const native_handle_type & native_file,
                                const cobalt::executor & executor = this_thread::get_executor());
  COBALT_IO_DECL stream_file(stream_file && sf) noexcept;

  write_op write_some(const_buffer_sequence buffer)
  {
    return {buffer, this, initiate_write_some_};
  }
  read_op read_some(mutable_buffer_sequence buffer)
  {
    return {buffer, this, initiate_read_some_};
  }
  COBALT_IO_DECL result<void> resize(std::uint64_t n);

  COBALT_IO_DECL result<std::uint64_t> seek(
      std::int64_t offset,
      seek_basis whence);

 private:
  COBALT_IO_DECL static void initiate_read_some_(void *, mutable_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_write_some_(void *, const_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);

#if defined(BOOST_ASIO_HAS_FILE)
  net::basic_stream_file<executor> implementation_;
#endif
};


}
#endif //BOOST_COBALT_EXPERIMENTAL_IO_STREAM_FILE_HPP
