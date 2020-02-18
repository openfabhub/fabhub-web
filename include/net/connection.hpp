#ifndef MP_NET_CONNECTION_HPP
#define MP_NET_CONNECTION_HPP

#include "support/logger_mixin.hpp"

#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

#include <cstddef>
#include <memory>
#include <set>
#include <system_error>

namespace mp::net
{

  using connection_ptr = std::shared_ptr<struct connection>;

  struct connection
      : std::enable_shared_from_this<connection>
      , logger_mixin
  {
    using request_type = boost::beast::http::request<boost::beast::http::string_body>;
    using error_type = std::error_code;

    struct event_subscriber
    {
      // clang-format off
      auto virtual on_request(connection_ptr, request_type) -> void { }
      auto virtual on_error(connection_ptr, error_type) -> void { }
      auto virtual on_close(connection_ptr) -> void { }
      // clang-format off
    };

    using subscriber_ptr = std::shared_ptr<struct event_subscriber>;

    auto static create(boost::asio::io_context & io_context, boost::asio::ip::tcp::socket socket, logger logger) -> connection_ptr;

    auto close() -> void;
    auto start() -> void;

    auto subscribe(subscriber_ptr subscriber) -> void;
    auto unsubscribe(subscriber_ptr subscriber) -> void;

  protected:
    connection(boost::asio::io_context & io_context, boost::asio::ip::tcp::socket socket, logger logger);

  private:
    // clang-format off
    struct close_event { };
    // clang-format on

    auto do_read() -> void;
    auto on_read(boost::beast::error_code error, std::size_t bytes) -> void;

    auto notify_subscribers(request_type request) -> void;
    auto notify_subscribers(error_type error) -> void;
    auto notify_subscribers(close_event) -> void;

    boost::asio::ip::tcp::endpoint m_remote;
    boost::asio::io_context::strand m_notification_strand;
    boost::beast::tcp_stream m_stream;
    boost::beast::flat_buffer m_buffer;
    boost::beast::http::request<boost::beast::http::string_body> m_request;

    std::set<subscriber_ptr> m_subscribers;
  };

}  // namespace mp::net

#endif