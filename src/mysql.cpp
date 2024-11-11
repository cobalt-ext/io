//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/config.hpp>
#include <cobalt/io/mysql.hpp>

#include <boost/cobalt/experimental/composition.hpp>

#include <boost/mysql/connection.hpp>
#include <cobalt/io/resolver.hpp>

namespace cobalt::io
{

template<typename AlgoParams>
void mysql_client::run_(
        boost::mysql::diagnostics & diag,
        AlgoParams params,
        completion_handler<error_code>)
{
  boost::mysql::detail::any_resumable_ref resumable = st_->setup(diag, params);

  using next_action_type = boost::mysql::detail::next_action_type;
  while (true)
  {
    error_code io_ec;
    std::size_t bytes_transferred = 0;
    // Run the op
    auto act = resumable.resume(io_ec, bytes_transferred);

    // Apply the next action
    bytes_transferred = 0;
    if (act.is_done())
    {
      co_return {act.error()};
    }
    else if (act.type() == next_action_type::read)
    {
      std::tie(io_ec, bytes_transferred) = co_await stream_.read_some(
          boost::mysql::detail::to_buffer(act.read_args().buffer));
    }
    else if (act.type() == next_action_type::write)
    {
      std::tie(io_ec, bytes_transferred) = co_await stream_.write_some(
          boost::asio::buffer(act.write_args().buffer));
    }
    else if (act.type() == next_action_type::ssl_handshake)
    {
      std::tie(io_ec) = co_await stream_.handshake(ssl_stream::handshake_type::client);
    }
    else if (act.type() == next_action_type::ssl_shutdown)
    {
      std::tie(io_ec) = co_await stream_.shutdown();
    }
    else if (act.type() == next_action_type::connect)
    {
      if (server_address_.type() == boost::mysql::address_type::host_and_port)
      {
        resolver res{co_await this_coro::executor};
        pmr::vector<endpoint> rn;
        std::tie(io_ec, endpoints_) = co_await res.resolve(server_address_.hostname(),
                                            std::to_string(server_address_.port()));
        if (io_ec)
          co_return {io_ec};

        endpoint _;
        std::tie(io_ec, _) = co_await stream_.connect(endpoints_);
        if (!io_ec)
          io_ec = stream_.set_no_delay(true).error();
      }
      else
        std::tie(io_ec) = co_await stream_.
            connect(endpoints_.emplace_back(local_stream, server_address_.unix_socket_path()));
    }
    else
    {
      BOOST_ASSERT(act.type() == next_action_type::close);
      io_ec  = stream_.close().error();
    }
  }
}

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::connect_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::execute_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::start_execution_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::prepare_statement_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::close_statement_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::read_some_rows_dynamic_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::read_some_rows_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::read_resultset_head_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::set_character_set_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::ping_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::reset_connection_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::close_connection_algo_params params,
    completion_handler<error_code>);

template
void mysql_client::run_(
    boost::mysql::diagnostics & diag,
    boost::mysql::detail::run_pipeline_algo_params params,
    completion_handler<error_code>);




}