#ifndef MP_NET_HOSTNAME
#define MP_NET_HOSTNAME

#include <newtype/new_type.hpp>

#include <cstddef>
#include <string>

namespace fhw
{
  namespace net
  {
    using hostname = nt::new_type<std::string, struct HostnameTag, deriving(nt::Show, nt::Read, nt::Hash)>;

    inline namespace literals
    {

      auto inline operator""_host(char const * name, std::size_t) -> hostname
      {
        return hostname{name};
      }

    }  // namespace literals
  }    // namespace net

  inline namespace literals
  {
    using namespace net::literals;
  }

}  // namespace fhw

#endif