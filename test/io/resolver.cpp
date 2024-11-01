//
// Copyright (c) 2023 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "../test.hpp"
#include <cobalt/io/resolver.hpp>
#include <boost/asio.hpp>

CO_TEST_CASE(resolver_)
{

  auto t = co_await cobalt::io::lookup("boost.org", "http");

  BOOST_REQUIRE(t.size() > 0u);
  for (auto & ep : t)
    BOOST_CHECK(ep.protocol() == cobalt::io::ip_v4);


}
