#ifndef MP_NET_IP_ADDRESS
#define MP_NET_IP_ADDRESS

#include <newtype/new_type.hpp>

#include <cstddef>
#include <string>

namespace mp
{
  namespace net
  {
    using ip_address = nt::new_type<std::string, struct IpAddressTag, deriving(nt::Show, nt::Read, nt::Hash)>;

    inline namespace literals
    {

      auto inline operator""_ip(char const * address, std::size_t) -> ip_address
      {
        return ip_address{address};
      }

    }  // namespace literals
  }    // namespace net

  inline namespace literals
  {
    using namespace net::literals;
  }

}  // namespace mp

#endif