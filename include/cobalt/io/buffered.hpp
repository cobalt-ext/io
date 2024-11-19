//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef COBALT_IO_BUFFERED_HPP
#define COBALT_IO_BUFFERED_HPP

#include <cobalt/io/ops.hpp>

namespace cobalt::io
{

struct buffered_reader
{
  read_op fill()
  {
    return {{},  this, &initiate_fill_};
  }

  boost::asio::const_registered_buffer available()
  {
    auto b = rbuffer_;
    b += begin_;
    return buffer(b, end_ - begin_);
  }
  read_op read_some(mutable_buffer_sequence buffers)
  {
    return {buffers, this, &initiate_read_some_, try_read_some_};
  }

  std::size_t  capacity() const {return buffer_.size();}
  std::size_t available() const {return end_ - begin_; }

  buffered_reader(read_op op, std::size_t n = 65535)
          : op_(std::move(op)), data_(new std::uint8_t[n]), buffer_(data_.get(), n) {}
  buffered_reader(read_op op, net::mutable_buffer buf)            : op_(std::move(op)), buffer_(buf) {}
  buffered_reader(read_op op, net::mutable_registered_buffer buf) : op_(std::move(op)), rbuffer_(buf) {}

 private:
  read_op op_;
  std::unique_ptr<std::uint8_t[]> data_;
  union
  {
    boost::asio::mutable_buffer buffer_;
    boost::asio::mutable_registered_buffer rbuffer_;
  };

  std::size_t begin_ = 0u, end_ = 0u;

  COBALT_IO_DECL static void try_read_some_      (void *, mutable_buffer_sequence, boost::cobalt::handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_read_some_ (void *, mutable_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_fill_      (void *, mutable_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
};

struct buffered_writer
{
  write_op flush()
  {
    return {{}, this, &initiate_flush_};
  }
  write_op write_some(const_buffer_sequence buffers)
  {
    return {buffers, this, &initiate_write_some_, &try_write_some_};
  }

  std::size_t capacity() const {return buffer_.size();}
  std::size_t available() const {return end_ - begin_;}

  buffered_writer(write_op op, std::size_t n = 65535)
          : op_(std::move(op)), data_(new std::uint8_t[n]), buffer_(data_.get(), n) {}
  buffered_writer(write_op op, net::mutable_buffer buf)            : op_(std::move(op)), buffer_(buf) {}
  buffered_writer(write_op op, net::mutable_registered_buffer buf) : op_(std::move(op)), rbuffer_(buf) {}

 private:
  write_op op_;
  std::unique_ptr<std::uint8_t[]> data_;
  union
  {
    boost::asio::mutable_buffer buffer_;
    boost::asio::mutable_registered_buffer rbuffer_;
  };

  std::size_t begin_ = 0u, end_ = 0u;

  COBALT_IO_DECL static void try_write_some_     (void *, const_buffer_sequence, boost::cobalt::handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_write_some_(void *, const_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
  COBALT_IO_DECL static void initiate_flush_     (void *, const_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>);
};

inline buffered_reader buffered(read_op op, std::size_t n = 65535)              {return {std::move(op), n}; }
inline buffered_reader buffered(read_op op, net::mutable_buffer buf)            {return {std::move(op), buf}; }
inline buffered_reader buffered(read_op op, net::mutable_registered_buffer buf) {return {std::move(op), buf}; }

inline buffered_writer buffered(write_op op, std::size_t n = 65535)              {return {std::move(op), n}; }
inline buffered_writer buffered(write_op op, net::mutable_buffer buf)            {return {std::move(op), buf}; }
inline buffered_writer buffered(write_op op, net::mutable_registered_buffer buf) {return {std::move(op), buf}; }

}

#endif //COBALT_IO_BUFFERED_HPP
