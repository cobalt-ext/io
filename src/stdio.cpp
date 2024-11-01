//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/signal_set.hpp>
#include <cobalt/io/stdio.hpp>

#include <boost/cobalt/experimental/composition.hpp>

#if defined(BOOST_POSIX_API)
#include <boost/asio/posix/basic_stream_descriptor.hpp>
#include <boost/asio/write.hpp>
#include <termios.h>
#else
#include <windows.h>
#endif


namespace cobalt::io
{

#if !defined(COBALT_RETURN_ERROR)
#define COBALT_RETURN_ERROR()                                            \
  do {                                                                   \
    constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION}; \
    return error_code{errno, ::boost::system::system_category(), &loc};  \
  }                                                                      \
  while(true)
#endif

result<void> set_console_echo(bool enable )
{
#if defined(BOOST_POSIX_API)
  struct ::termios t;
  if (tcgetattr(STDIN_FILENO, &t))
    COBALT_RETURN_ERROR();

  if (enable)
    t.c_lflag |=  (ECHO | ECHOE | ECHOK | ECHONL);
  else
    t.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);

  if (tcsetattr(STDIN_FILENO, TCSANOW, &t))
    COBALT_RETURN_ERROR();


#else
  DWORD mode;
  auto h = GetStdHandle(STD_INPUT_HANDLE);
  if (!GetConsoleMode(h, &mode))
    COBALT_RETURN_ERROR();

  if (enable)
    mode |= ENABLE_ECHO_INPUT;
  else
    mode &= ~ENABLE_ECHO_INPUT;

  if (!SetConsoleMode(h, mode))
    COBALT_RETURN_ERROR();
#endif
  return {};
}

result<void> set_console_line(bool enable)
{
#if defined(BOOST_POSIX_API)
  struct ::termios t;
  if (tcgetattr(STDIN_FILENO, &t))
    COBALT_RETURN_ERROR();

  if (enable)
    t.c_lflag |=  ICANON;
  else
    t.c_lflag &= ~ICANON;

  if (tcsetattr(STDIN_FILENO, TCSANOW, &t))
    COBALT_RETURN_ERROR();
#else
  DWORD mode;
  auto h = GetStdHandle(STD_INPUT_HANDLE);

  if (!GetConsoleMode(h, &mode))
    COBALT_RETURN_ERROR();

  if (enable)
    mode |= ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT ;
  else
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT );

  if (!SetConsoleMode(h, mode))
    COBALT_RETURN_ERROR();
#endif
  return {};
}

result<bool> console_echo()
{
#if defined(BOOST_POSIX_API)
  struct ::termios t;
  if (tcgetattr(STDIN_FILENO, &t))
    COBALT_RETURN_ERROR();

  return (t.c_lflag & (ECHO)) == ECHO;
#else
  DWORD mode;
  auto h = GetStdHandle(STD_INPUT_HANDLE);

  if (!GetConsoleMode(h, &mode))
    COBALT_RETURN_ERROR();

  return (mode & ENABLE_ECHO_INPUT) == ENABLE_ECHO_INPUT;
#endif
}

result<bool> console_line()
{
#if defined(BOOST_POSIX_API)
  struct ::termios t;
  if (tcgetattr(STDIN_FILENO, &t))
    COBALT_RETURN_ERROR();

  return (t.c_lflag & ICANON) == ICANON;
#else
  DWORD mode;
  auto h = GetStdHandle(STD_INPUT_HANDLE);

  if (!GetConsoleMode(h, &mode))
    COBALT_RETURN_ERROR();

  return (mode & ENABLE_LINE_INPUT) == ENABLE_LINE_INPUT;
#endif
}


bool is_pty()
{
#if defined(BOOST_POSIX_API)
    struct ::termios t;
    return !tcgetattr(STDIN_FILENO, &t);
#else
    return GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_CHAR;
    return handle_.object.is_open();
#endif
}

struct stdin_service : boost::asio::execution_context::service
{

  static boost::asio::execution_context::id id;
  stdin_service(boost::asio::execution_context & ctx)
      : boost::asio::execution_context::service(ctx) {}

  void shutdown() override {}

#if defined(BOOST_POSIX_API)
  std::optional<signal_set> ss;
  std::optional<boost::asio::posix::basic_stream_descriptor<executor>> handle;

  signal_set & get_signal_set(executor ex)
  {
    if (!ss)
    {
      ss.emplace(ex);
      ss->add(SIGWINCH).value();
    }
    return *ss;
  }

  boost::asio::posix::basic_stream_descriptor<executor> & get_stream(executor ex)
  {
    if (!handle)
      handle.emplace(ex, ::dup(STDIN_FILENO));
    return *handle;
  }
#else

#endif

};

boost::asio::execution_context::id stdin_service::id;

void wait_for_input_impl(void *, boost::cobalt::completion_handler<error_code> handler)
{
#if defined(BOOST_POSIX_API)
  auto & is = boost::asio::use_service<stdin_service>((co_await this_coro::executor).context());
  auto & s = is.get_stream(co_await this_coro::executor);
  co_return std::get<0>(co_await s.async_wait(boost::asio::posix::descriptor_base::wait_read));
#else
#error impl this
#endif
}

wait_op  wait_for_input()
{
  return {nullptr, &wait_for_input_impl};
}


void wait_for_console_size_change::initiate(boost::cobalt::completion_handler<error_code, console_size_t>)
{
  if (!has_pty())
  {
    constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION};
    co_return {error_code{boost::asio::error::operation_not_supported, &loc}, {}};
  }

  auto & is = boost::asio::use_service<stdin_service>((co_await this_coro::executor).context());

#if defined(BOOST_POSIX_API)
  auto & signal_set = is.get_signal_set(co_await this_coro::executor);
  auto [ec, _] = co_await signal_set.wait();
  if (ec)
    co_return {ec, {}};

  auto cc = console_size();
  if (cc)
    co_return {{}, *cc};
  else
    co_return {cc.error(), {}};

#else
#error implement - > channel ?
#endif
}

void gets_impl(void *, mutable_buffer_sequence buffer,
               boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
#if defined(BOOST_POSIX_API)
  auto & is = boost::asio::use_service<stdin_service>(handler.get_executor().context());
  auto & s = is.get_stream(handler.get_executor());
  s.async_read_some(buffer, std::move(handler));
#else
#error impl this
#endif
}

read_op gets(mutable_buffer_sequence buffer)
{
  return {buffer, nullptr, &gets_impl};
}

struct stdout_service : boost::asio::execution_context::service
{

  static boost::asio::execution_context::id id;
  stdout_service(boost::asio::execution_context & ctx)
      : boost::asio::execution_context::service(ctx) {}

  void shutdown() override {}

#if defined(BOOST_POSIX_API)
  std::optional<boost::asio::posix::basic_stream_descriptor<executor>> handle;
  boost::asio::posix::basic_stream_descriptor<executor> & get_stream(executor ex)
  {
    if (!handle)
      handle.emplace(ex, ::dup(STDOUT_FILENO));
    return *handle;
  }
#else

#endif

};

boost::asio::execution_context::id stdout_service::id;

void print_impl(void *, const_buffer_sequence buffer,
               boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
#if defined(BOOST_POSIX_API)
  auto & is = boost::asio::use_service<stdout_service>(handler.get_executor().context());
  auto & s = is.get_stream(handler.get_executor());
  boost::asio::async_write(s, buffer, std::move(handler));
#else
#error impl this
#endif
}

write_op print(const_buffer_sequence buffer)
{
  return {buffer, nullptr, &print_impl};
}

struct stderr_service : boost::asio::execution_context::service
{

  static boost::asio::execution_context::id id;
  stderr_service(boost::asio::execution_context & ctx)
      : boost::asio::execution_context::service(ctx) {}

  void shutdown() override {}

#if defined(BOOST_POSIX_API)
  std::optional<boost::asio::posix::basic_stream_descriptor<executor>> handle;
  boost::asio::posix::basic_stream_descriptor<executor> & get_stream(executor ex)
  {
    if (!handle)
      handle.emplace(ex, ::dup(STDERR_FILENO));
    return *handle;
  }
#else

#endif

};

boost::asio::execution_context::id stderr_service::id;

void print_error_impl(void *, const_buffer_sequence buffer,
                boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
#if defined(BOOST_POSIX_API)
  auto & is = boost::asio::use_service<stderr_service>(handler.get_executor().context());
  auto & s = is.get_stream(handler.get_executor());
  boost::asio::async_write(s, buffer, std::move(handler));
#else
#error impl this
#endif
}

write_op print_error(const_buffer_sequence buffer)
{
  return {buffer, nullptr, &print_error_impl};
}









}