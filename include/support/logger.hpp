#ifndef MP_SUPPORT_LOGGER_HPP
#define MP_SUPPORT_LOGGER_HPP

#include <spdlog/logger.h>

#include <memory>
#include <string>

namespace fhw
{

  auto make_logger(std::string name) -> std::shared_ptr<spdlog::logger>;

}  // namespace fhw

#endif