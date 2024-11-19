//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/buffered.hpp>

#include <boost/cobalt/experimental/composition.hpp>
#include <cobalt/io/write.hpp>

namespace cobalt::io
{


void buffered_reader::initiate_fill_(void * this_, mutable_buffer_sequence,
                                     boost::cobalt::completion_handler<error_code, std::size_t>)
{
  //
  auto bf = static_cast<buffered_reader*>(this_);

  auto sz = bf->capacity() - bf->end_;

  constexpr std::size_t chunk_size = 8196;
  if (sz < chunk_size && bf->begin_ > 0u)
  {
    auto orig = bf->buffer_;
    orig += bf->begin_;
    std::memmove(bf->buffer_.data(), orig.data(), bf->end_ - bf->begin_);
    bf->end_ -= bf->begin_;
    bf->begin_ = 0u;
  }

  auto bb = bf->rbuffer_;
  bb += bf->begin_;
  bf->op_.buffer = buffer(bb, bf->end_ - bf->begin_);

  auto [ec, n] = co_await bf->op_;

  bf->end_ += n;
  co_return {ec, n};
}


void buffered_reader::try_read_some_      (void * this_, mutable_buffer_sequence buffer,
                                           boost::cobalt::handler<error_code, std::size_t> h)
{
  auto bf = static_cast<buffered_reader*>(this_);
  if (bf->begin_ != bf->end_)
  {
    net::const_registered_buffer bb = bf->rbuffer_;
    bb += bf->begin_;
    const auto n = net::buffer_copy(buffer, bb.buffer(), bf->end_ - bf->begin_);
    bf->begin_ += n;
    if (bf ->begin_ == bf->end_)
      bf ->begin_ = bf->end_ = 0u;

    h({}, n);
  }
}

void buffered_reader::initiate_read_some_ (void * this_, mutable_buffer_sequence mbs, boost::cobalt::completion_handler<error_code, std::size_t>)
{
  auto bf = static_cast<buffered_reader*>(this_);

  error_code ec;
  std::size_t _
  ;

  while (bf->begin_ == bf->end_ && !ec)
    std::tie(ec, _) = co_await bf->fill();


  net::const_registered_buffer bb = bf->rbuffer_;
  bb += bf->begin_;
  const auto n = net::buffer_copy(mbs, bb.buffer(), bf->end_ - bf->begin_);
  bf->begin_ += n;
  co_return {ec, n};
}

void buffered_writer::try_write_some_     (void * this_, const_buffer_sequence buffer, boost::cobalt::handler<error_code, std::size_t> h)
{
  auto bw = static_cast<buffered_writer*>(this_);
  constexpr std::size_t chunk_size = 8196;
  if ((bw->buffer_.size() - bw->end_) >= chunk_size)
  {
    auto b = bw->buffer_;
    b += bw->begin_;
    auto n = net::buffer_copy(net::buffer(b, bw->end_ - bw->begin_), buffer);
    h({}, n);
  }
}


void buffered_writer::initiate_write_some_(void * this_, const_buffer_sequence buf,
                                           boost::cobalt::completion_handler<error_code, std::size_t>)
{
  // write some and move the buffer.
  auto bw = static_cast<buffered_writer*>(this_);
  net::const_registered_buffer cc = bw->rbuffer_;
  cc += bw->begin_;
  cc = buffer(cc, bw->end_ - bw->begin_);
  bw->op_.buffer = cc;
  auto [ec, n] = co_await bw->op_;

  cc += n;
  std::memmove(bw->buffer_.data(), cc.data(), cc.size());

  n = net::buffer_copy(buffer(bw->buffer_, cc.size()), buf);
  co_return {ec, n};
}

void buffered_writer::initiate_flush_     (void * this_, const_buffer_sequence, boost::cobalt::completion_handler<error_code, std::size_t>)
{
  auto bw = static_cast<buffered_writer*>(this_);
  net::const_registered_buffer cc = bw->rbuffer_;
  cc += bw->begin_;

  bw->op_.buffer = net::buffer(cc, bw->end_ - bw->begin_);

  const auto res = co_await write_all(bw->op_);
  if (bw->begin_ == bw->end_)
    bw->begin_ = bw->end_ = 0u;
  co_return res;
}

}
