#include "net/connection.hpp"

#include "support/asio_log_support.hpp"
#include "support/logger_mixin.hpp"

#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/read.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iterator>
#include <utility>

using namespace std::literals;

namespace mp::net
{

  struct connection_ctor_access : connection
  {
    template<typename... ParameterTypes>
    connection_ctor_access(ParameterTypes &&... parameters)
        : connection(std::forward<ParameterTypes>(parameters)...)
    {
    }
  };

  connection::connection(boost::asio::ip::tcp::socket socket, logger logger)
      : logger_mixin{logger, "connection"}
      , m_remote{socket.remote_endpoint()}
      , m_stream{std::move(socket)}
      , m_buffer{}
      , m_request{}
      , m_subscribers{}
  {
  }

  auto connection::create(boost::asio::ip::tcp::socket socket, logger logger) -> connection_ptr
  {
    return std::make_shared<connection_ctor_access>(std::move(socket), logger);
  }

  auto connection::close() -> void
  {
    log_info("close", "closing connection to '{}'", m_remote);
    auto ignored = boost::beast::error_code{};
    m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
    m_stream.close();
    notify_subscribers(close_event{});
  }

  auto connection::start() -> void
  {
    log_info("start", "starting conversation with '{}'", m_remote);
    boost::asio::dispatch(m_stream.get_executor(),
                          boost::beast::bind_front_handler(&connection::do_read, shared_from_this()));
  }

  auto connection::subscribe(subscriber_ptr subscriber) -> bool
  {
    auto [_, inserted] = m_subscribers.insert(subscriber);
    return inserted;
  }

  auto connection::unsubscribe(subscriber_ptr subscriber) -> bool
  {
    return m_subscribers.erase(subscriber);
  }

  auto connection::do_read() -> void
  {
    m_request.clear();
    m_stream.expires_after(30s);
    boost::beast::http::async_read(m_stream,
                                   m_buffer,
                                   m_request,
                                   boost::beast::bind_front_handler(&connection::on_read, shared_from_this()));
  }

  auto connection::on_read(boost::beast::error_code error, std::size_t bytes) -> void
  {
    log_debug("on_read", "read {} bytes from '{}'", bytes, m_remote);

    if (error == boost::beast::http::error::end_of_stream)
    {
      close();
    }
    else if (error)
    {
      log_error("on_read", "failed to read a request from '{}'. reason: {}", m_remote, error.message());
      close();
    }
    else
    {
      do_read();
    }
  }

  auto connection::notify_subscribers(request_type request) -> void
  {
    log_trace("notify_subscribers", "notifying {} subscribers about an incoming request", m_subscribers.size());
    for_each(begin(m_subscribers), end(m_subscribers), [this, &request](auto subscriber) {
      subscriber->on_request(shared_from_this(), request);
    });
  }

  auto connection::notify_subscribers(error_type error) -> void
  {
    log_trace("notify_subscribers",
              "notifying {} subscribers about the following error: ",
              m_subscribers.size(),
              error.message());
    for_each(begin(m_subscribers), end(m_subscribers), [this, &error](auto subscriber) {
      subscriber->on_error(shared_from_this(), error);
    });
  }

  auto connection::notify_subscribers(close_event) -> void
  {
    log_trace("notify_subscribers", "notifying {} subscribers about connection closure", m_subscribers.size());
    for_each(begin(m_subscribers), end(m_subscribers), [this](auto subscriber) {
      subscriber->on_close(shared_from_this());
    });
    m_subscribers.clear();
  }

}  // namespace mp::net