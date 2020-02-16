#ifndef MP_SUPPORT_ASIO_LOG_SUPPORT
#define MP_SUPPORT_ASIO_LOG_SUPPORT

#include <boost/asio/ip/tcp.hpp>

#include <ostream>

auto inline operator<<(std::ostream & stream, boost::asio::ip::tcp::endpoint const & endpoint) -> std::ostream &
{
  return stream << endpoint.address().to_string() << ':' << endpoint.port();
}

#endif