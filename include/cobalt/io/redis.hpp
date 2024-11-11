//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COBALT_IO_REDIS_HPP
#define COBALT_IO_REDIS_HPP

#include <cobalt/io/config.hpp>

#include <boost/cobalt/detached.hpp>
#include <boost/cobalt/op.hpp>
#include <boost/cobalt/this_thread.hpp>
#include <boost/redis/connection.hpp>
#include "endpoint.hpp"


namespace cobalt::io
{


struct redis_client
{
  redis_client(
      const cobalt::executor & executor = this_thread::get_executor(),
      boost::asio::ssl::context ctx = boost::asio::ssl::context{boost::asio::ssl::context::tlsv12_client},
      std::size_t max_read_size = (std::numeric_limits<std::size_t>::max)())
        : conn_(executor, std::move(ctx), max_read_size)
  {}

  using response_type = boost::redis::generic_response;
  using request_type = boost::redis::request;

  struct run_op final : op<error_code>
  {
    boost::redis::basic_connection<executor> & conn;
    
    const boost::redis::config & cfg;
    run_op(boost::redis::basic_connection<executor> & conn,
           const boost::redis::config & cfg) : conn(conn), cfg(cfg) {}
    COBALT_IO_DECL
    void initiate(completion_handler<error_code> handler) final;

    cobalt::detached detach() // just run, ignore the error
    {
      auto t = std::move(*this);
      co_await cobalt::as_tuple(std::move(t));
    }
  };

  run_op run(const boost::redis::config & cfg = {})
  {
    return run_op{conn_, cfg};
  }

  struct receive_op final : op<error_code, std::size_t>
  {
    boost::redis::basic_connection<executor> & conn;

    receive_op(boost::redis::basic_connection<executor> & conn) : conn(conn) {}

    COBALT_IO_DECL
    void ready(handler<error_code, std::size_t> h);

    COBALT_IO_DECL
    void initiate(completion_handler<error_code, std::size_t> handler) final;

  };

  receive_op receive()
  {
    return receive_op{conn_};
  }

  struct exec_op final : op<error_code, std::size_t>
  {
    boost::redis::basic_connection<executor> & conn;
    request_type const & req;
    response_type & resp;
    exec_op(boost::redis::basic_connection<executor> & conn,
            boost::redis::request const & req,
            boost::redis::generic_response & resp) : conn(conn), req(req), resp(resp) {}

    COBALT_IO_DECL
    void initiate(completion_handler<error_code, std::size_t> handler) final;
  };

  exec_op exec(request_type const & req, response_type  & res)
  {
    return exec_op{conn_, req, res};
  }


 private:
  boost::redis::basic_connection<executor> conn_;
};

}

#endif //COBALT_IO_REDIS_HPP
