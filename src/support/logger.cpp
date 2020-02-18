#include "support/logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>
#include <string>
#include <utility>

namespace fhw
{

  auto make_logger(std::string name) -> std::shared_ptr<spdlog::logger>
  {
    auto stdio_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    return std::make_shared<spdlog::logger>(std::move(name), stdio_sink);
  }

}  // namespace fhw