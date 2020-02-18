#ifndef MP_NET_DEFAULTS_HPP
#define MP_NET_DEFAULTS_HPP

#include "net/ip_address.hpp"
#include "net/port.hpp"

namespace fhw::net
{

  auto const default_listen_address = ip_address{"0.0.0.0"};
  auto constexpr default_listen_port = port{51717};

}  // namespace fhw::net

#endif