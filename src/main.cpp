#include "net/hostname.hpp"
#include "net/ip_address.hpp"
#include "net/server.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <csignal>
#include <cstdlib>
#include <exception>
#include <memory>
#include <string>
#include <utility>

auto make_logger(std::string name) -> std::shared_ptr<spdlog::logger>
{
  auto stdio_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  return std::make_shared<spdlog::logger>(std::move(name), stdio_sink);
}

auto make_server(boost::asio::io_context & io_context, mp::net::ip_address ip, mp::net::port port, mp::logger logger)
{
  try
  {
    return mp::net::server::create(io_context, ip, port, logger);
  }
  catch (std::exception const & e)
  {
    logger->critical("unhandled exception during startup: {}", e.what());
  }
  return mp::net::server_ptr{};
}

auto main() -> int
{
  using namespace mp::literals;

  auto logger = make_logger("mofprint");
  logger->info("starting up");
  auto io_context = boost::asio::io_context{};

  auto server = make_server(io_context, "0.0.0.0"_ip, 51717_port, logger);
  if (!server)
  {
    return EXIT_FAILURE;
  }
  server->start();

  auto signals = boost::asio::signal_set{io_context, SIGINT, SIGTERM};
  signals.async_wait([&](auto error, auto) {
    if (!error)
    {
      server->stop();
    }
  });

  io_context.run();
}
