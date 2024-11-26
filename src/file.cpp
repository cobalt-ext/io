//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/file.hpp>
#include <sys/stat.h>


namespace cobalt::io
{

#if defined(BOOST_ASIO_HAS_FILE)


result<void> file::assign(const native_handle_type & native_file)
{
  error_code ec;
  file_.assign(native_file, ec);
  return ec ? ec : result<void>{};
}
result<void> file::cancel()
{
  error_code ec;
  file_.cancel(ec);
  return ec ? ec : result<void>{};
}

executor file::get_executor() { return file_.get_executor();}
bool file::is_open() const { return file_.is_open();}

auto file::native_handle() -> native_handle_type {return file_.native_handle();}

result<void> file::open(const char * path, flags open_flags)
{
  error_code ec;
  file_.open(path, open_flags, ec);
  return ec ? ec : result<void>{};
}

result<void> file::open(const std::string & path, flags open_flags)
{
  error_code ec;
  file_.open(path, open_flags, ec);
  return ec ? ec : result<void>{};
}

auto file::release() -> result<native_handle_type>
{
  error_code ec;
  auto r = file_.release(ec);
  return ec ? ec : result<native_handle_type>{r};
}

result<std::uint64_t> file::size() const
{
  error_code ec;
  auto n = file_.size(ec);
  return ec ? ec : result<std::uint64_t>{n};
}


result<void> file::resize(std::uint64_t sz)
{
  error_code ec;
  file_.resize(sz, ec);
  return ec ? ec : result<void>{};
}

result<void> file::sync_all()
{
  error_code ec;
  file_.sync_all(ec);
  return ec ? ec : result<void>{};
}

result<void> file::sync_data()
{
  error_code ec;
  file_.sync_data(ec);
  return ec ? ec : result<void>{};
}

result<void> file::close()
{
  error_code ec;
  file_.close(ec);
  return ec ? ec : result<void>{};
}

#else



result<void> file::assign(const native_handle_type & native_file)
{
  boost::system::error_code ec;
  file_.assign(native_file, ec);
  return ec ? ec : result<void>{};
}
result<void> file::cancel()
{
  return result<void>{};
}

executor file::get_executor() { return file_.get_executor();}
bool file::is_open() const { return file_.is_open();}

auto file::native_handle() -> native_handle_type {return file_.native_handle();}

#if !defined(COBALT_RETURN_ERROR)
#define COBALT_RETURN_ERROR()                                            \
  do {                                                                   \
    constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION}; \
    return error_code{errno, ::boost::system::system_category(), &loc};  \
  }                                                                      \
  while(true)
#endif

result<void> file::open(const char * path, flags open_flags)
{
  int r = ::open(path, static_cast<int>(open_flags), 0777);
  if (r == -1)
    COBALT_RETURN_ERROR();
  boost::system::error_code ec;
  file_.assign(r, ec);
  return ec ? ec : result<void>{};
}

result<void> file::open(const std::string & path, flags open_flags)
{
  return open(path.c_str(), open_flags);
}

auto file::release() -> result<native_handle_type>
{
  auto v = file_.native_handle();
  file_.release();
  return v;
}

result<std::uint64_t> file::size() const
{
  struct stat s;
  if (-1 == ::fstat(
      const_cast<boost::asio::posix::basic_stream_descriptor<executor>&>(file_).native_handle(), & s))
    COBALT_RETURN_ERROR();
  return s.st_size;
}


result<void> file::resize(std::uint64_t sz)
{;
  if (-1 == ::ftruncate(file_.native_handle(), sz))
    COBALT_RETURN_ERROR();
  return {};
}

result<void> file::sync_all()
{
  if (-1 == ::fsync(file_.native_handle()))
    COBALT_RETURN_ERROR();
  return {};
}

result<void> file::sync_data()
{
#if defined(_POSIX_SYNCHRONIZED_IO)
  if (-1 ==  ::fdatasync(file_.native_handle()))
#else // defined(_POSIX_SYNCHRONIZED_IO)
  if (-1 ==  ::fsync(file_.native_handle()))
#endif // defined(_POSIX_SYNCHRONIZED_IO)
    COBALT_RETURN_ERROR();
  return {};
}

result<void> file::close()
{
  boost::system::error_code ec;
  file_.close(ec);
  return ec ? ec : result<void>{};
}

#endif


}

