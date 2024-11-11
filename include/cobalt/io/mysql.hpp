//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COBALT_IO_MYSQL_HPP
#define COBALT_IO_MYSQL_HPP

#include <cobalt/io/config.hpp>
#include <cobalt/io/ssl.hpp>

#include <boost/cobalt/op.hpp>
#include <boost/cobalt/this_thread.hpp>
#include <boost/mysql/any_connection.hpp>

namespace cobalt::io
{

struct mysql_error : error_code, boost::mysql::diagnostics
{
  mysql_error(error_code ec,
              boost::mysql::diagnostics diag) : error_code(ec), boost::mysql::diagnostics(std::move(diag))
  {}

  friend BOOST_NORETURN inline
  void throw_exception_from_error( const mysql_error & e, boost::source_location const & loc )
  {
    boost::throw_exception(
        boost::mysql::error_with_diagnostics(e, e), loc);
  }
};

struct mysql_client
{
  template<typename AlgoParams>
  struct awaitable_base
  {
    mysql_client * client_;
    AlgoParams algo;
    boost::mysql::diagnostics diag;

    std::optional<std::tuple<error_code>> result;

#if !defined(BOOST_COBALT_NO_PMR)
    using resource_type = pmr::memory_resource;
#else
    using resource_type = detail::sbo_resource;
#endif

    template<typename ... Args_>
    awaitable_base(resource_type *resource, mysql_client * client_, AlgoParams algo)
        : client_(client_), algo(std::move(algo)), resource(resource) {}
    awaitable_base(awaitable_base && lhs)
        : client_(lhs.client_), algo(std::move(lhs.algo)), result(std::move(lhs.result)), resource(lhs.resource)
    {
    }

    bool await_ready() { return false; }

    cobalt::detail::completed_immediately_t completed_immediately = cobalt::detail::completed_immediately_t::no;
    std::exception_ptr init_ep;

    resource_type *resource;

    template<typename Promise>
    bool await_suspend(std::coroutine_handle<Promise> h
#if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
        , const boost::source_location & loc = BOOST_CURRENT_LOCATION
#endif
    ) noexcept
    {
      BOOST_TRY
        {
          completed_immediately = cobalt::detail::completed_immediately_t::initiating;

#if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
          completion_handler<error_code> ch{h, result, resource, &completed_immediately, loc};
#else
          completion_handler<error_code> ch{h, result, resource, &completed_immediately};
#endif
          client_->run_(diag, std::move(algo), std::move(ch));

          if (completed_immediately == cobalt::detail::completed_immediately_t::initiating)
            completed_immediately = cobalt::detail::completed_immediately_t::no;
          return completed_immediately != cobalt::detail::completed_immediately_t::yes;
        }
        BOOST_CATCH(...)
        {
          init_ep = std::current_exception();
          return false;
        }
      BOOST_CATCH_END
    }

    auto await_resume(const struct as_tuple_tag &)
    {
      if (init_ep)
        std::rethrow_exception(init_ep);

      if constexpr (std::is_void_v<typename AlgoParams::result_type>)
        return std::make_tuple(std::get<0>(*result));
      else
        return std::make_tuple(std::get<0>(*result), client_->st_->result<AlgoParams>());
    }

    auto await_resume(const struct as_result_tag &)
    {
      if (init_ep)
        std::rethrow_exception(init_ep);

      mysql_error err{std::get<0>(*result), std::move(diag)};
      if constexpr (std::is_void_v<typename AlgoParams::result_type>)
      {
        if (err)
          return boost::system::result<void, mysql_error>(std::move(err));
        else
          return boost::system::result<void, mysql_error>();
      }
      else
      {
        using type = typename AlgoParams::result_type;
        if (err)
          return boost::system::result<type, mysql_error>(std::move(err));
        else
          return boost::system::result<type, mysql_error>(client_->st_->result<AlgoParams>());
      }
    }

    auto await_resume(const boost::source_location & loc = BOOST_CURRENT_LOCATION)
    {
      if (init_ep)
        std::rethrow_exception(init_ep);
      return await_resume(as_result_tag{}).value(loc);
    }

  };

  template<typename AlgoParams>
  struct awaitable : awaitable_base<AlgoParams>
  {
    char buffer[BOOST_COBALT_SBO_BUFFER_SIZE];
    cobalt::detail::sbo_resource resource{buffer, sizeof(buffer)};

    template<typename ... Args_>
    awaitable(mysql_client * client_, AlgoParams algo) : awaitable_base<AlgoParams>(&resource, client_, std::move(algo)) {}
    awaitable(awaitable && lhs) : awaitable_base<AlgoParams>(std::move(lhs))
    {
      this->awaitable<AlgoParams>::resource = &resource;
    }

    awaitable_base<AlgoParams>replace_resource(typename awaitable_base<AlgoParams>::resource_type * resource) &&
    {
      awaitable_base<AlgoParams> nw = std::move(*this);
      nw.resource = resource;
      return nw;
    }
  };
  
  using params = boost::mysql::any_connection_params;

  mysql_client(boost::mysql::any_connection_params params = {},
               executor ex = this_thread::get_executor())
               : stream_(params.ssl_context ?
                         ssl_stream(*params.ssl_context, ex) :
                         ssl_stream(ex)),
                 st_(boost::mysql::detail::new_connection_state(params.initial_buffer_size,params.max_buffer_size, true))
  {
  }

  bool uses_ssl() const noexcept { return stream_.upgraded(); }
  bool backslash_escapes() const noexcept { return st_->data().backslash_escapes; }
  boost::system::result<boost::mysql::character_set> current_character_set() const noexcept
  {
    return st_->data().current_charset;
  }
  boost::system::result<boost::mysql::format_options> format_opts() const noexcept
  {
    auto res = current_character_set();
    if (res.has_error())
      return res.error();
    return boost::mysql::format_options{res.value(), backslash_escapes()};
  }
  boost::mysql::metadata_mode meta_mode() const noexcept { return st_->data().meta_mode; }
  void set_meta_mode(boost::mysql::metadata_mode v) noexcept { st_->data().meta_mode = v; }


  awaitable<boost::mysql::detail::connect_algo_params> connect(
      const boost::mysql::connect_params& params)
  {
    server_address_ = params.server_address;
    return {this, boost::mysql::detail::connect_algo_params{
          boost::mysql::detail::make_hparams(params),
          params.server_address.type() == boost::mysql::address_type::unix_path
      }};
  }

  template <BOOST_MYSQL_EXECUTION_REQUEST ExecutionRequest, BOOST_MYSQL_RESULTS_TYPE ResultsType>
  awaitable<boost::mysql::detail::execute_algo_params> execute(ExecutionRequest&& req, ResultsType& result)
  {
    return {
      this,
      {
        boost::mysql::detail::execution_request_traits<typename std::decay<ExecutionRequest>::type>::
        make_request(std::forward<ExecutionRequest>(req), *st_),
        &boost::mysql::detail::access::get_impl(result).get_interface()
      }
    };
  }

  template <BOOST_MYSQL_EXECUTION_REQUEST ExecutionRequest, BOOST_MYSQL_RESULTS_TYPE ResultsType>
  awaitable<boost::mysql::detail::start_execution_algo_params> start_execution(ExecutionRequest&& req, ResultsType& result)
  {
    return {
        this,
        {
            boost::mysql::detail::execution_request_traits<typename std::decay<ExecutionRequest>::type>::
            make_request(std::forward<ExecutionRequest>(req), *st_),
            &boost::mysql::detail::access::get_impl(result).get_interface()
        }
    };
  }

  awaitable<boost::mysql::detail::prepare_statement_algo_params> prepare_statement(std::string_view stmt)
  {
    return {this, {stmt}};
  }

  awaitable<boost::mysql::detail::close_statement_algo_params> close_statement(const boost::mysql::statement& stmt)
  {
    return {this, {stmt.id()}};
  }

  awaitable<boost::mysql::detail::read_some_rows_dynamic_algo_params> read_some_rows(boost::mysql::execution_state& st)
  {
    return {this, {&boost::mysql::detail::access::get_impl(st).get_interface()}};
  }

  template <class SpanElementType, class... StaticRow>
  awaitable<boost::mysql::detail::read_some_rows_algo_params> read_some_rows(
      boost::mysql::static_execution_state<StaticRow...>& st,
      std::span<SpanElementType> output)
  {
    using boost::mysql::detail::access;
    return {this, {
        &access::get_impl(st).get_interface(),
        access::get_impl(st).make_output_ref(output)}};
  }

  template <BOOST_MYSQL_EXECUTION_STATE_TYPE ExecutionStateType>
  awaitable<boost::mysql::detail::read_resultset_head_algo_params> read_resultset_head(ExecutionStateType& st)
  {
    return {boost::mysql::detail::access::get_impl(st).get_interface()};
  }


 private:

  COBALT_IO_DECL
  template<typename Resumable>
  void run_(boost::mysql::diagnostics & diag,
            Resumable resumable,
            completion_handler<error_code>);


  boost::mysql::any_address server_address_;
  ssl_stream stream_;
  pmr::vector<endpoint> endpoints_;
  std::unique_ptr<boost::mysql::detail::connection_state,
                  boost::mysql::detail::connection_state_deleter> st_;

};


}

#endif //COBALT_IO_MYSQL_HPP
