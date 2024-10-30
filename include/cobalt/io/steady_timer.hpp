//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_IO_STEADY_TIMER_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_IO_STEADY_TIMER_HPP

#include <boost/cobalt/op.hpp>
#include <cobalt/io/ops.hpp>
#include <boost/asio/basic_waitable_timer.hpp>

#include <boost/system/result.hpp>

namespace cobalt::io
{

struct steady_timer
{
  using wait_result = result<void>;

  /// The clock type.
  typedef std::chrono::steady_clock clock_type;

  /// The duration type of the clock.
  typedef typename clock_type::duration duration;

  /// The time point type of the clock.
  typedef typename clock_type::time_point time_point;

  steady_timer(const cobalt::executor & executor = this_thread::get_executor());
  steady_timer(const time_point& expiry_time, const cobalt::executor & executor = this_thread::get_executor());
  steady_timer(const duration& expiry_time,   const cobalt::executor & executor = this_thread::get_executor());

  void cancel();

  time_point expiry() const;
  void reset(const time_point& expiry_time);
  void reset(const duration& expiry_time);
  bool expired() const;

  [[nodiscard]] wait_op wait() { return {this, initiate_wait_}; }
 private:

  COBALT_IO_DECL static void initiate_wait_(void *, boost::cobalt::completion_handler<error_code>);

  net::basic_waitable_timer<std::chrono::steady_clock,
                                    net::wait_traits<std::chrono::steady_clock>,
                                    executor> timer_;
};

}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_IO_STEADY_TIMER_HPP
