//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cobalt/io/resolver.hpp>

#include <boost/asio/deferred.hpp>

namespace cobalt::io
{


resolver::resolver(const cobalt::executor & executor) : resolver_(executor) {}
void resolver::cancel() { resolver_.cancel(); }

void resolver::resolve_op_::initiate(completion_handler<error_code, pmr::vector<endpoint>> h)
{
  resolver_.async_resolve(
      cobalt::io::ip, host_, service_,
      net::deferred(
          [](error_code ec, auto rr)
          {
            pmr::vector<endpoint> r{this_thread::get_allocator()};
            r.assign(rr.begin(), rr.end());

            return net::deferred.values(ec, std::move(r));

          }))(std::move(h));
}

}