//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/signal_set.hpp>

namespace cobalt::io
{

signal_set::signal_set(const cobalt::executor & executor) : signal_set_(executor) {}
signal_set::signal_set(std::initializer_list<int> sigs, const cobalt::executor & executor)
  : signal_set_(executor)
{
    for (auto i : sigs)
      add(i).value();
}

result<void> signal_set::cancel()
{
  error_code ec;
  signal_set_.cancel(ec);
  return ec ? ec : result<void>{};
}

result<void> signal_set::clear()
{
  error_code ec;
  signal_set_.clear(ec);
  return ec ? ec : result<void>{};
}

result<void> signal_set::add(int signal_number)
{
  error_code ec;
  signal_set_.add(signal_number, ec);
  return ec ? ec : result<void>{};
}

result<void> signal_set::remove(int signal_number)
{
  error_code ec;
  signal_set_.remove(signal_number, ec);
  return ec ? ec : result<void>{};
}

void signal_set::wait_op_::initiate(completion_handler<error_code, int> h)
{
  signal_set_.async_wait(std::move(h));
}

}