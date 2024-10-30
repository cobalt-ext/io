//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef COBALT_IO_CONFIG_HPP
#define COBALT_IO_CONFIG_HPP

#include <boost/cobalt/config.hpp>
#include <boost/system/result.hpp>

namespace boost::cobalt::detail {}
namespace boost::asio {}
namespace cobalt
{

namespace net = boost::asio;

using boost::system::result;
using boost::system::error_code;

using namespace boost::cobalt;

namespace detail
{
using namespace boost::cobalt::detail;
}

#if defined(BOOST_ALL_DYN_LINK) || defined(COBALT_IO_DYN_LINK)
#if defined(COBALT_IO_SOURCE)
#define COBALT_IO_DECL BOOST_SYMBOL_EXPORT
#else
#define COBALT_IO_DECL BOOST_SYMBOL_IMPORT
#endif
#else
#define COBALT_IO_DECL
#endif


}

#endif //COBALT_IO_CONFIG_HPP
