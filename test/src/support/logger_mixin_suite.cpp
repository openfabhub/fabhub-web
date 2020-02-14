#include "support/logger_mixin_suite.hpp"

#include "kawaii.hpp"
#include "support/logger_mixin.hpp"

#include <cute/cute.h>

#include <spdlog/logger.h>
#include <spdlog/sinks/ostream_sink.h>

#include <memory>
#include <sstream>
#include <string>
#include <utility>

struct logging : mp::logger_mixin
{
  explicit logging(mp::logger logger)
      : logger_mixin{logger, "logging"}
  {
  }

  auto perform_trace_log() const -> void
  {
    log_trace("perform_trace_log", "This is a trace log. {}", 42);
  }

  auto perform_debug_log() const -> void
  {
    log_debug("perform_debug_log", "This is a debug log. {}", 42);
  }

  auto perform_info_log() const -> void
  {
    log_info("perform_info_log", "This is a info log. {}", 42);
  }

  auto perform_warn_log() const -> void
  {
    log_warn("perform_warn_log", "This is a warn log. {}", 42);
  }

  auto perform_error_log() const -> void
  {
    log_error("perform_error_log", "This is a error log. {}", 42);
  }

  auto perform_critical_log() const -> void
  {
    log_critical("perform_critical_log", "This is a critical log. {}", 42);
  }
};

auto make_logger(std::string name, std::ostream & ostream) -> mp::logger
{
  auto sink = std::make_shared<spdlog::sinks::ostream_sink_st>(ostream, true);
  auto logger = std::make_shared<spdlog::logger>(name, sink);

  logger->set_pattern("%l: %v");
  logger->set_level(spdlog::level::trace);
  logger->flush_on(spdlog::level::trace);

  return logger;
}

auto a_type_derived_from_mp_logger_mixin_can_write_trace_logs() -> void
{
  auto stream = std::ostringstream{};
  auto logger = make_logger("trace_test_0", stream);

  auto user = logging{logger};
  user.perform_trace_log();

  ASSERT_EQUAL("trace: logging::perform_trace_log: This is a trace log. 42\n", stream.str());
}

auto a_type_derived_from_mp_logger_mixin_can_write_debug_logs() -> void
{
  auto stream = std::ostringstream{};
  auto logger = make_logger("debug_test_0", stream);

  auto user = logging{logger};
  user.perform_debug_log();

  ASSERT_EQUAL("debug: logging::perform_debug_log: This is a debug log. 42\n", stream.str());
}

auto a_type_derived_from_mp_logger_mixin_can_write_info_logs() -> void
{
  auto stream = std::ostringstream{};
  auto logger = make_logger("info_test_0", stream);

  auto user = logging{logger};
  user.perform_info_log();

  ASSERT_EQUAL("info: logging::perform_info_log: This is a info log. 42\n", stream.str());
}

auto a_type_derived_from_mp_logger_mixin_can_write_warn_logs() -> void
{
  auto stream = std::ostringstream{};
  auto logger = make_logger("warn_test_0", stream);

  auto user = logging{logger};
  user.perform_warn_log();

  ASSERT_EQUAL("warning: logging::perform_warn_log: This is a warn log. 42\n", stream.str());
}

auto a_type_derived_from_mp_logger_mixin_can_write_error_logs() -> void
{
  auto stream = std::ostringstream{};
  auto logger = make_logger("error_test_0", stream);

  auto user = logging{logger};
  user.perform_error_log();

  ASSERT_EQUAL("error: logging::perform_error_log: This is a error log. 42\n", stream.str());
}

auto a_type_derived_from_mp_logger_mixin_can_write_critical_logs() -> void
{
  auto stream = std::ostringstream{};
  auto logger = make_logger("critical_test_0", stream);

  auto user = logging{logger};
  user.perform_critical_log();

  ASSERT_EQUAL("critical: logging::perform_critical_log: This is a critical log. 42\n", stream.str());
}

auto logger_mixin_suite() -> std::pair<cute::suite, std::string>
{
  return {
      {
          KAWAII(a_type_derived_from_mp_logger_mixin_can_write_trace_logs),
          KAWAII(a_type_derived_from_mp_logger_mixin_can_write_debug_logs),
          KAWAII(a_type_derived_from_mp_logger_mixin_can_write_info_logs),
          KAWAII(a_type_derived_from_mp_logger_mixin_can_write_warn_logs),
          KAWAII(a_type_derived_from_mp_logger_mixin_can_write_error_logs),
          KAWAII(a_type_derived_from_mp_logger_mixin_can_write_critical_logs),
      },
      "logger_mixin Suite",
  };
}