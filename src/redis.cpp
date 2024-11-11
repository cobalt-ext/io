//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/redis.hpp>

namespace cobalt::io
{

COBALT_IO_DECL
void redis_client::run_op::initiate(completion_handler<boost::system::error_code> handler)
{
  conn.async_run(cfg, {}, std::move(handler));
}

void redis_client::receive_op::ready(handler<error_code, std::size_t> h)
{
  error_code ec;
  auto n = conn.receive(ec);
  if (ec != boost::redis::error::sync_receive_push_failed)
    h(ec, n);
}


COBALT_IO_DECL
void redis_client::receive_op::initiate(completion_handler<boost::system::error_code, std::size_t> handler)
{
  conn.async_receive(std::move(handler));
}



COBALT_IO_DECL
void redis_client::exec_op::initiate(completion_handler<boost::system::error_code, std::size_t> handler)
{
  conn.async_exec(req, resp, std::move(handler));
}

}