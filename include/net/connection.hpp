#ifndef MP_NET_CONNECTION_HPP
#define MP_NET_CONNECTION_HPP

#include "support/logger_mixin.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

#include <cstddef>
#include <memory>

namespace mp::net
{

  using connection_ptr = std::shared_ptr<struct connection>;

  struct connection
      : std::enable_shared_from_this<connection>
      , logger_mixin
  {
    auto static create(boost::asio::ip::tcp::socket socket, logger logger) -> connection_ptr;

    auto close() -> void;
    auto start() -> void;

  protected:
    connection(boost::asio::ip::tcp::socket socket, logger logger);

  private:
    auto do_read() -> void;
    auto on_read(boost::beast::error_code error, std::size_t bytes) -> void;

    boost::asio::ip::tcp::endpoint m_remote;
    boost::beast::tcp_stream m_stream;
    boost::beast::flat_buffer m_buffer;
    boost::beast::http::request<boost::beast::http::string_body> m_request;
  };

}  // namespace mp::net

#endif