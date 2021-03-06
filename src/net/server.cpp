#include "net/server.hpp"

#include "support/asio_log_support.hpp"
#include "support/logger_mixin.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/system/system_error.hpp>
#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <system_error>

namespace fhw::net
{

  using namespace fhw::literals;

  struct server_ctor_access : server
  {
    template<typename... ParameterTypes>
    server_ctor_access(ParameterTypes &&... parameters)
        : server(std::forward<ParameterTypes>(parameters)...)
    {
    }
  };

  template<typename... ParameterTypes>
  auto create_server(logger logger, ParameterTypes &&... parameters) -> server_ptr
  {
    try
    {
      return std::make_shared<server_ctor_access>(std::forward<ParameterTypes>(parameters)..., logger);
    }
    catch (std::exception const & e)
    {
      logger->error("create", "failed to instantiate server. reason: {}", e.what());
      return {};
    }
  }

  auto server::create(boost::asio::io_context & io_context, port port, logger logger) -> server_ptr
  {
    return create_server(logger, io_context, port);
  }

  auto server::create(boost::asio::io_context & io_context, ip_address ip, port port, logger logger) -> server_ptr
  {
    return create_server(logger, io_context, ip, port);
  }

  auto server::create(boost::asio::io_context & io_context, hostname host, port port, logger logger) -> server_ptr
  {
    return create_server(logger, io_context, host, port);
  }

  server::server(boost::asio::io_context & io_context, logger logger)
      : logger_mixin{logger, "server"}
      , m_io_context{io_context}
      , m_acceptor{boost::asio::make_strand(io_context)}
  {
  }

  server::server(boost::asio::io_context & io_context, port port, logger logger)
      : server{io_context, "0.0.0.0"_ip, port, logger}
  {
  }

  server::server(boost::asio::io_context & io_context, ip_address ip, port port, logger logger)
      : server{io_context, logger}
  {
    auto address = boost::asio::ip::make_address(ip.decay());
    m_endpoint = boost::asio::ip::tcp::endpoint{address, port.decay()};
  }

  server::server(boost::asio::io_context & io_context, hostname host, port port, logger logger)
      : server{io_context, logger}
  {
    auto resolver = boost::asio::ip::tcp::resolver{m_acceptor.get_executor()};
    auto results = resolver.resolve(host.decay(), "");
    if (results.empty())
    {
      throw std::runtime_error{fmt::format("failed to resolve '{}'", host)};
    }
    auto found = results.begin()->endpoint();
    found.port(port.decay());
    m_endpoint = found;
  }

  auto server::start() noexcept -> std::error_code
  {
    try
    {
      m_acceptor.open(m_endpoint.protocol());
      m_acceptor.set_option(boost::asio::socket_base::reuse_address(true));
      m_acceptor.bind(m_endpoint);
      m_acceptor.listen(boost::asio::socket_base::max_listen_connections);
    }
    catch (boost::system::system_error const & e)
    {
      return e.code();
    }

    do_accept();
    return {};
  }

  auto server::stop() -> void
  {
    log_info("close", "gracefully shutting down the server");
    auto ignored = boost::system::error_code{};
    m_acceptor.cancel(ignored);
    m_acceptor.close(ignored);
    for_each(begin(m_connections), end(m_connections), [this](auto connection) {
      connection->unsubscribe(shared_from_this());
      connection->close();
    });
  }

  auto server::on_close(connection_ptr connection) -> void
  {
    auto erased = m_connections.erase(connection);
    log_info("on_close", "erased {} connections from the pool, {} remain", erased, m_connections.size());
  }

  auto server::on_error(connection_ptr connection, std::error_code error) -> void
  {
    log_info("on_error", "dropping connection due to the following error: {}", error.message());
    connection->close();
  }

  auto server::do_accept() -> void
  {
    log_debug("do_accept", "waiting for an incoming connection");
    m_acceptor.async_accept(boost::asio::make_strand(m_io_context),
                            boost::beast::bind_front_handler(&server::on_accept, this));
  }

  auto server::on_accept(boost::beast::error_code error, boost::asio::ip::tcp::socket socket) -> void
  {
    if (error && error != boost::asio::error::operation_aborted)
    {
      log_error("on_accept", "failed to accept connection. reason: {}", error.message());
    }
    else if (!error)
    {
      auto const remote = socket.remote_endpoint();
      log_info("on_accept", "accepted new connection from '{}'", remote);
      auto [connection, inserted] =
          m_connections.insert(connection::create(m_io_context, std::move(socket), logger_handle()));
      if (inserted)
      {
        (*connection)->start();
        (*connection)->subscribe(shared_from_this());
      }
      do_accept();
    }
  }

}  // namespace fhw::net