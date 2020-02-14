#ifndef MP_NET_CONNECTION_HPP
#define MP_NET_CONNECTION_HPP

#include "support/logger_mixin.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace mp::net
{

  using connection_ptr = std::shared_ptr<struct connection>;

  struct connection
      : std::enable_shared_from_this<connection>
      , logger_mixin
  {
    auto static create(boost::asio::ip::tcp::socket socket, logger logger) -> connection_ptr;

  protected:
    connection(boost::asio::ip::tcp::socket socket, logger logger);

  private:
    boost::asio::ip::tcp::socket m_socket;
  };

}  // namespace mp::net

#endif