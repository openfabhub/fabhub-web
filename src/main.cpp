#include "net/defaults.hpp"
#include "net/hostname.hpp"
#include "net/ip_address.hpp"
#include "net/server.hpp"
#include "support/logger.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include <csignal>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>

auto main() -> int
{
  using namespace mp;

  auto io_context = boost::asio::io_context{};

  auto logger = make_logger("web");
  logger->info("starting up");

  auto server = net::server::create(io_context, net::default_listen_address, net::default_listen_port, logger);
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
