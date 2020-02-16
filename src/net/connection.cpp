#include "net/connection.hpp"

#include "support/asio_log_support.hpp"
#include "support/logger_mixin.hpp"

#include <boost/asio/dispatch.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/read.hpp>

#include <chrono>
#include <cstddef>
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
  }

  auto connection::start() -> void
  {
    log_info("start", "starting conversation with '{}'", m_remote);
    boost::asio::dispatch(m_stream.get_executor(),
                          boost::beast::bind_front_handler(&connection::do_read, shared_from_this()));
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

}  // namespace mp::net