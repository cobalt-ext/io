//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_IO_SLEEP_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_IO_SLEEP_HPP

#include <cobalt/io/ops.hpp>
#include <cobalt/io/steady_timer.hpp>
#include <cobalt/io/system_timer.hpp>

#include <boost/cobalt/promise.hpp>

namespace cobalt::detail::io
{

struct [[nodiscard]] steady_sleep
{
  steady_sleep(const std::chrono::steady_clock::time_point & tp) : tim{tp} {}
  steady_sleep(const std::chrono::steady_clock::duration & du)   : tim{du} {}

  auto operator co_await() { return std::move(op_.emplace(tim.wait())).operator co_await(); }
 private:
  cobalt::io::steady_timer tim;
  std::optional<decltype(tim.wait())> op_;
};

struct [[nodiscard]] system_sleep
{
  system_sleep(const std::chrono::system_clock::time_point & tp) : tim{tp} {}
  system_sleep(const std::chrono::system_clock::duration & du)   : tim{du} {}

  auto operator co_await() { return std::move(op_.emplace(tim.wait())).operator co_await(); }
 private:
  cobalt::io::system_timer tim;
  std::optional<decltype(tim.wait())> op_;
};

}

namespace cobalt::io
{



// NOTE: these don't need to be coros, we can optimize that out. Not sure that's worth it though
auto sleep(const std::chrono::steady_clock::duration & d)    { return detail::io::steady_sleep{d};}
auto sleep(const std::chrono::steady_clock::time_point & tp) { return detail::io::steady_sleep{tp};}
auto sleep(const std::chrono::system_clock::time_point & tp) { return detail::io::system_sleep{tp};}

template<typename Duration>
auto sleep(const std::chrono::time_point<std::chrono::steady_clock, Duration> & tp)
{
  return sleep(std::chrono::time_point_cast<std::chrono::steady_clock::duration >(tp));
}

template<typename Duration>
auto  sleep(const std::chrono::time_point<std::chrono::system_clock, Duration> & tp)
{
  return sleep(std::chrono::time_point_cast<std::chrono::system_clock::duration >(tp));
}

template<typename Rep, typename Period>
auto sleep(const std::chrono::duration<Rep, Period> & dur)
{
  return sleep(std::chrono::duration_cast<std::chrono::steady_clock::duration >(dur));
}

}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_IO_SLEEP_HPP
