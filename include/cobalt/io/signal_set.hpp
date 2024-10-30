//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_SIGNAL_SET_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_SIGNAL_SET_HPP

#include <boost/cobalt/op.hpp>
#include <cobalt/io/ops.hpp>

#include <boost/asio/basic_signal_set.hpp>
#include <boost/system/result.hpp>

#include <initializer_list>

namespace cobalt::io
{

struct signal_set
{
  signal_set(const cobalt::executor & executor = this_thread::get_executor());
  signal_set(std::initializer_list<int> sigs, const cobalt::executor & executor = this_thread::get_executor());

  [[nodiscard]] result<void> cancel();
  [[nodiscard]] result<void> clear();
  [[nodiscard]] result<void> add(int signal_number);
  [[nodiscard]] result<void> remove(int signal_number);


 private:
  struct wait_op_ final : cobalt::op<error_code, int>
  {
    COBALT_IO_DECL
    void initiate(completion_handler<error_code, int> h) final;
    wait_op_(net::basic_signal_set<cobalt::executor> & signal_set) : signal_set_(signal_set) {}
   private:
    net::basic_signal_set<cobalt::executor> & signal_set_;
  };
 public:
  [[nodiscard]] auto wait() { return wait_op_{signal_set_}; }
 private:
  net::basic_signal_set<cobalt::executor> signal_set_;
};


}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_SIGNAL_SET_HPP
