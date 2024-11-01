//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COBALT_IO_STDIO_HPP
#define COBALT_IO_STDIO_HPP

#include <cobalt/io/config.hpp>
#include <cobalt/io/buffer.hpp>
#include <cobalt/io/ops.hpp>

#include <boost/cobalt/op.hpp>

#include <cstdio>


namespace cobalt::io
{

COBALT_IO_DECL result<void> set_console_echo(bool enable = true);
COBALT_IO_DECL result<void> set_console_line(bool enable = true);

COBALT_IO_DECL result<bool> console_echo();
COBALT_IO_DECL result<bool> console_line();

COBALT_IO_DECL wait_op  wait_for_input();
COBALT_IO_DECL read_op  gets(mutable_buffer_sequence buffer);
COBALT_IO_DECL write_op print(const_buffer_sequence buffer);
COBALT_IO_DECL write_op print_error(const_buffer_sequence buffer);

COBALT_IO_DECL bool has_pty();

struct console_size_t
{
  unsigned short columns, rows;
};

COBALT_IO_DECL result<console_size_t> console_size();

struct wait_for_console_size_change final : boost::cobalt::op<error_code, console_size_t>
{
  COBALT_IO_DECL
  void initiate(boost::cobalt::completion_handler<error_code, console_size_t> h) final;
  wait_for_console_size_change() {}
};



}

#endif //COBALT_IO_STDIO_HPP
