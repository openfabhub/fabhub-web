#include "net/hostname.hpp"
#include "net/ip_address.hpp"
#include "net/server.hpp"

#include <boost/asio/io_context.hpp>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <exception>
#include <memory>
#include <string>
#include <utility>

auto make_logger(std::string name) -> std::shared_ptr<spdlog::logger>
{
  auto stdio_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  return std::make_shared<spdlog::logger>(std::move(name), stdio_sink);
}

auto main() -> int
{
  using namespace mp::literals;

  auto logger = make_logger("mofprint");
  logger->info("starting up");
  auto io_context = boost::asio::io_context{};

  try
  {
    auto server = mp::net::server::create(io_context, "0.0.0.0"_ip, 51717_port, logger);
    server->start();

    io_context.run();
  }
  catch (std::exception const & e)
  {
    logger->critical("unhandled exception during startup: {}", e.what());
    throw;
  }
}
