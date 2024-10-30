//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/file.hpp>

#if defined(BOOST_ASIO_HAS_FILE)


namespace cobalt::io
{

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

}

#endif
