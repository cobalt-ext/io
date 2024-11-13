//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COBALT_IO_SENDFILE_HPP
#define COBALT_IO_SENDFILE_HPP

#include <cobalt/io/config.hpp>

#include <boost/cobalt/op.hpp>

namespace cobalt::io
{

struct socket;
struct file;

struct sendfile_op final : op<error_code, std::size_t>
{
  struct socket & sock;
  struct file & file;

  COBALT_IO_DECL
  void initiate(completion_handler<error_code, std::size_t> handler) final;
};

}

#endif //COBALT_IO_SENDFILE_HPP
