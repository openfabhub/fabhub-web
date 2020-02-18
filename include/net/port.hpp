#ifndef MP_NET_PORT
#define MP_NET_PORT

#include <newtype/new_type.hpp>

#include <cstdint>

namespace fhw
{
  namespace net
  {

    using port = nt::new_type<std::uint16_t, struct PortTag, deriving(nt::Show, nt::Read, nt::Hash)>;

    inline namespace literals
    {

      auto constexpr inline operator""_port(unsigned long long number) -> port
      {
        return port{static_cast<std::uint16_t>(number)};
      }

    }  // namespace literals
  }    // namespace net

  inline namespace literals
  {
    using namespace net::literals;
  }

}  // namespace fhw

#endif