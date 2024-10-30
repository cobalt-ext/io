//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/initiate_templates.hpp>
#include <cobalt/io/stream_file.hpp>

#if defined(BOOST_ASIO_HAS_FILE)

namespace cobalt::io
{

static_assert(stream<stream_file>);


stream_file::stream_file(const cobalt::executor & executor)
      : file(implementation_), implementation_(executor)
{
}

stream_file::stream_file(const char * path, file_base::flags open_flags,
                         const cobalt::executor & executor)
      : file(implementation_), implementation_(executor, path, open_flags)
{
}

stream_file::stream_file(const std::string & path, file_base::flags open_flags,
                         const cobalt::executor & executor)
      : file(implementation_), implementation_(executor, path, open_flags)
{
}
stream_file::stream_file(const native_handle_type & native_file,
                         const cobalt::executor & executor)
      : file(implementation_), implementation_(executor, native_file)
{
}

stream_file::stream_file(stream_file && sf) noexcept = default;



result<void> stream_file::resize(std::uint64_t n)
{
  error_code ec;
  implementation_.resize(n, ec);
  return ec ? ec : result<void>{};
}

result<std::uint64_t> stream_file::seek(std::int64_t offset, seek_basis whence)
{
  error_code ec;
  auto n = implementation_.seek(offset, whence);
  return ec ? ec : result<std::uint64_t>{n};
}

void stream_file::initiate_read_some_(void * this_, mutable_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  initiate_async_read_some(static_cast<stream_file*>(this_)->implementation_, buffer, std::move(handler));
}
void stream_file::initiate_write_some_(void * this_, const_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  initiate_async_write_some(static_cast<stream_file*>(this_)->implementation_, buffer, std::move(handler));
}


}

#endif