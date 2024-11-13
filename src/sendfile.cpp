//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/sendfile.hpp>
#include <cobalt/io/file.hpp>
#include <cobalt/io/socket.hpp>

#include <boost/cobalt/experimental/composition.hpp>

#include <boost/asio/windows/overlapped_ptr.hpp>
#if defined(BOOST_WINDOWS_API)
#else
#include <sys/sendfile.h>
#endif

namespace cobalt::io
{

void sendfile_op::initiate(completion_handler<error_code, std::size_t> handler)
{
#if defined(BOOST_WINDOWS_API)
  boost::asio::windows::overlapped_ptr overlapped(handler.get_executor(), std::move(handler));
  BOOL ok = ::TransmitFile(sock.native_handle(), file.native_handle(), 0, 0, overlapped.get(), 0, 0);
  DWORD last_error = ::GetLastError();

  if (!ok && last_error != ERROR_IO_PENDING)
  {
    constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION};
    error_code ec(static_cast<int>(last_error), boost::system::system_category(), &loc);
    overlapped.complete(ec, 0);
  }
  else
    overlapped.release();
#elif __linux__ || defined(__FreeBSD__) || defined(__APPLE__) && defined(__MACH__)
  std::size_t res;
  off_t offset = 0u;
  auto [ec] = co_await sock.wait(socket::wait_type::wait_write);

  while (!ec)
  {
#if __linux__
    const std::size_t n = ::sendfile(sock.native_handle(), file.native_handle(), &offset, 65536);
    const bool failed = n == -1;
#elif defined (__FreeBSD__)
    off_t n = 65536;
    const bool failed =  ::sendfile(file.native_handle(), sock.native_handle(), &offset, &n, nullptr, 0) != 0;
    if (!failed)
      offset += n;

#elif defined(__APPLE__) && defined(__MACH__)
    off_t n = 65536;
    const bool failed =  ::sendfile(file.native_handle(), sock.native_handle(), offset, 0, nullptr, &n, 0) != 0;
    if (!failed)
      offset += n;
#endif
    if (failed)
    {
      const int err = errno;
      if (err == EWOULDBLOCK || err == EAGAIN)
        std::tie(ec) = co_await sock.wait(socket::wait_type::wait_write);
      else if (err == EINTR)
        continue;
      else
      {
        constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION};
        ec.assign(err, boost::system::system_category(), &loc);
        break;
      }
    }

    res += n;
  }

#else
  constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION};
  ec.assign(ENOTSUP, boost::system::system_category(), &loc);
  handler(ec, 0u);
#endif

}

}