//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_READ_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_READ_HPP

#include <cobalt/io/buffer.hpp>
#include <cobalt/io/ops.hpp>

#include <concepts>

namespace cobalt::io
{

struct read_all  final : op<error_code, std::size_t>
{
  read_op step;
  read_all(read_op op) : step(op) {}

  COBALT_IO_DECL void initiate(completion_handler<error_code, std::size_t>) final;
};

template<typename Stream>
  requires requires (Stream & str, mutable_buffer_sequence buffer)
  {
    {str.read_some(buffer)} -> std::same_as<read_op>;
  }
  read_all read(Stream & str, mutable_buffer_sequence buffer)
{
  return read_all{str.read_some(buffer)};
}

}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_READ_HPP
