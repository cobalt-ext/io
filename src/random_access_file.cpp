//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/initiate_templates.hpp>
#include <cobalt/io/random_access_file.hpp>


namespace cobalt::io
{
#if defined(BOOST_ASIO_HAS_FILE)


random_access_file::random_access_file(const cobalt::executor & executor)
    : file(implementation_), implementation_(executor)
{
}

random_access_file::random_access_file(const char * path, file_base::flags open_flags,
                                       const cobalt::executor & executor)
    : file(implementation_), implementation_(executor, path, open_flags)
{
}

random_access_file::random_access_file(const std::string & path, file_base::flags open_flags,
                                       const cobalt::executor & executor)
    : file(implementation_), implementation_(executor, path, open_flags)
{
}
random_access_file::random_access_file(const native_handle_type & native_file,
                                       const cobalt::executor & executor)
    : file(implementation_), implementation_(executor, native_file)
{
}

random_access_file::random_access_file(random_access_file && sf) noexcept = default;

void random_access_file::initiate_read_some_at_(void *this_, std::uint64_t offset,  mutable_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  return initiate_async_read_some_at(static_cast<random_access_file*>(this_)->implementation_, offset, buffer, std::move(handler));
}
void random_access_file::initiate_write_some_at_(void *this_, std::uint64_t offset, const_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  return initiate_async_write_some_at(static_cast<random_access_file*>(this_)->implementation_, offset, buffer, std::move(handler));
}

#else

random_access_file::random_access_file(const cobalt::executor & executor)
    : file(executor)
{

}

random_access_file::random_access_file(const char * path, file::flags open_flags,
                                       const cobalt::executor & executor)
    : file(executor)
{
  open(path, open_flags).value();
}

random_access_file::random_access_file(const std::string & path, file::flags open_flags,
                                       const cobalt::executor & executor)
    : file(executor)
{
  open(path, open_flags).value();
}

random_access_file::random_access_file(const native_handle_type & native_file,
                                       const cobalt::executor & executor)
    : file(executor, native_file)
{
}

random_access_file::random_access_file(random_access_file && sf) noexcept = default;

void initiate_async_read_some_at_helper(boost::asio::posix::basic_stream_descriptor<executor> & str,
                                        std::uint64_t offset, mutable_buffer_sequence seq,
                                        completion_handler<error_code, std::size_t> handler)
{
  if (lseek64(str.native_handle(), SEEK_SET, offset) < 0)
  {
    constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION};
    return handler(error_code{errno, ::boost::system::system_category(), &loc}, 0u);
  }

  if (seq.buffer_count() > 0u)
    str.async_read_some(seq, std::move(handler));
  else if (seq.is_registered())
    str.async_read_some(seq.registered, std::move(handler));
  else
    str.async_read_some(seq.head, std::move(handler));
}

void initiate_async_write_some_at_helper(boost::asio::posix::basic_stream_descriptor<executor> & str,
                                        std::uint64_t offset, const_buffer_sequence seq,
                                        completion_handler<error_code, std::size_t> handler)
{
  if (lseek64(str.native_handle(), SEEK_SET, offset) < 0)
  {
    constexpr static boost::source_location loc{BOOST_CURRENT_LOCATION};
    return handler(error_code{errno, ::boost::system::system_category(), &loc}, 0u);
  }

  if (seq.buffer_count() > 0u)
    str.async_write_some(seq, std::move(handler));
  else if (seq.is_registered())
    str.async_write_some(seq.registered, std::move(handler));
  else
    str.async_write_some(seq.head, std::move(handler));
}

void random_access_file::initiate_read_some_at_(void *this_, std::uint64_t offset,  mutable_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  return initiate_async_read_some_at_helper(static_cast<random_access_file*>(this_)->file_, offset, buffer, std::move(handler));
}
void random_access_file::initiate_write_some_at_(void *this_, std::uint64_t offset, const_buffer_sequence buffer, boost::cobalt::completion_handler<error_code, std::size_t> handler)
{
  return initiate_async_write_some_at_helper(static_cast<random_access_file*>(this_)->file_, offset, buffer, std::move(handler));
}

#endif

}

