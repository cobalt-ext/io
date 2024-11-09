//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_COBALT_EXPERIMENTAL_IO_RESOLVER_HPP
#define BOOST_COBALT_EXPERIMENTAL_IO_RESOLVER_HPP

#include <cobalt/io/endpoint.hpp>
#include <cobalt/io/ops.hpp>
#include <boost/cobalt/op.hpp>
#include <boost/cobalt/promise.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/container/pmr/vector.hpp>
#include <boost/system/result.hpp>
#include <boost/url/url_view.hpp>

namespace cobalt::io
{

struct resolver
{
  resolver(const cobalt::executor & executor = this_thread::get_executor());
  resolver(resolver && ) = delete;

  void cancel();

 private:

  struct [[nodiscard]] resolve_op_ final : op<error_code, pmr::vector<endpoint>>
  {
    COBALT_IO_DECL
    void initiate(completion_handler<error_code, pmr::vector<endpoint>> h) override;

    resolve_op_(net::ip::basic_resolver<protocol_type, executor> & resolver,
                std::string_view host, std::string_view service)
                : resolver_(resolver), host_(host), service_(service) {}
   private:
    net::ip::basic_resolver<protocol_type, executor> & resolver_;
    std::string_view host_;
    std::string_view service_;

  };

 public:

  [[nodiscard]] auto resolve(std::string_view host, std::string_view service)
  {
    return resolve_op_{resolver_, host, service};
  }


 private:
  net::ip::basic_resolver<protocol_type, executor> resolver_;
};

// NOTE: Doesn't need to be a promise, can be optimized.
struct lookup
{
  lookup(std::string_view host, std::string_view service)
        : host_(host), service_(service) {}
  auto operator co_await() && {return std::move(op_.emplace(resolver_.resolve(host_, service_))).operator co_await();}
 private:
  std::string_view host_;
  std::string_view service_;
  resolver resolver_;
  std::optional<decltype(resolver_.resolve(host_, service_))> op_;
};

}

#endif //BOOST_COBALT_EXPERIMENTAL_IO_RESOLVER_HPP
