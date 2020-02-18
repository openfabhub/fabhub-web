#ifndef MP_NET_SERVER_HPP
#define MP_NET_SERVER_HPP

#include "net/connection.hpp"
#include "net/hostname.hpp"
#include "net/ip_address.hpp"
#include "net/port.hpp"
#include "support/logger_mixin.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http.hpp>

#include <memory>
#include <set>
#include <system_error>

namespace mp::net
{

  using server_ptr = std::shared_ptr<struct server>;

  struct server
      : std::enable_shared_from_this<server>
      , logger_mixin
      , connection::event_subscriber
  {

    auto static create(boost::asio::io_context & io_context, port port, logger logger) -> server_ptr;
    auto static create(boost::asio::io_context & io_context, ip_address ip, port port, logger logger) -> server_ptr;
    auto static create(boost::asio::io_context & io_context, hostname host, port port, logger logger) -> server_ptr;

    auto start() noexcept -> std::error_code;
    auto stop() -> void;

    auto on_close(connection_ptr connection) -> void override;
    auto on_error(connection_ptr connection, std::error_code error) -> void override;

  protected:
    server(boost::asio::io_context & io_context, port port, logger logger);
    server(boost::asio::io_context & io_context, ip_address ip, port port, logger logger);
    server(boost::asio::io_context & io_context, hostname host, port port, logger logger);

  private:
    server(boost::asio::io_context & io_context, logger logger);

    auto do_accept() -> void;
    auto on_accept(boost::beast::error_code error, boost::asio::ip::tcp::socket socket) -> void;

    boost::asio::io_context & m_io_context;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::endpoint m_endpoint{};

    std::set<connection_ptr> m_connections{};
  };

}  // namespace mp::net

#endif