#include "net/connection.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <utility>

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
      , m_socket{std::move(socket)}
  {
  }

  auto connection::create(boost::asio::ip::tcp::socket socket, logger logger) -> connection_ptr
  {
    return std::make_shared<connection_ctor_access>(std::move(socket), logger);
  }

}  // namespace mp::net