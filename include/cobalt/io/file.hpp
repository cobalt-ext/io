//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_FILE_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_FILE_HPP

#include <boost/asio/basic_file.hpp>
#if defined(BOOST_ASIO_HAS_FILE)

#include <cobalt/io/config.hpp>
#include <boost/system/result.hpp>

namespace cobalt::io
{

struct file : net::file_base
{

  using native_handle_type = net::basic_file<executor>::native_handle_type;

  COBALT_IO_DECL result<void> assign(const native_handle_type & native_file);
  COBALT_IO_DECL result<void> cancel();

  COBALT_IO_DECL executor get_executor();
  COBALT_IO_DECL bool is_open() const;

  COBALT_IO_DECL result<void> close();
  COBALT_IO_DECL native_handle_type native_handle();

  COBALT_IO_DECL result<void> open(const char * path,        flags open_flags);
  COBALT_IO_DECL result<void> open(const std::string & path, flags open_flags);

  COBALT_IO_DECL result<native_handle_type> release();
  COBALT_IO_DECL result<void> resize(std::uint64_t n);

  COBALT_IO_DECL result<std::uint64_t> size() const;
  COBALT_IO_DECL result<void> sync_all();
  COBALT_IO_DECL result<void> sync_data();


  file(net::basic_file<executor> & file) : file_(file) {}
 private:
  net::basic_file<executor> & file_;
};

}

#endif

#endif //BOOST_COBALT_EXPERIMENTAL_IO_FILE_HPP
