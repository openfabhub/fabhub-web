#ifndef MP_SUPPORT_LOGGER_MIXIN_HPP
#define MP_SUPPORT_LOGGER_MIXIN_HPP

#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/logger.h>

#include <memory>
#include <string_view>
#include <utility>

namespace mp
{

  using logger = std::shared_ptr<spdlog::logger>;

  struct logger_mixin
  {
    logger_mixin(logger logger, std::string_view class_name)
        : m_logger{std::move(logger)}
        , m_class_name{class_name}
    {
    }

  protected:
    template<typename... ParameterTypes>
    auto log_trace(std::string function, std::string format, ParameterTypes &&... parameters) const
    {
      auto prefixed = make_prefixed_format(function, format);
      m_logger->trace(prefixed, std::forward<ParameterTypes>(parameters)...);
    }

    template<typename... ParameterTypes>
    auto log_debug(std::string function, std::string format, ParameterTypes &&... parameters) const
    {
      auto prefixed = make_prefixed_format(function, format);
      m_logger->debug(prefixed, std::forward<ParameterTypes>(parameters)...);
    }

    template<typename... ParameterTypes>
    auto log_info(std::string function, std::string format, ParameterTypes &&... parameters) const
    {
      auto prefixed = make_prefixed_format(function, format);
      m_logger->info(prefixed, std::forward<ParameterTypes>(parameters)...);
    }

    template<typename... ParameterTypes>
    auto log_warn(std::string function, std::string format, ParameterTypes &&... parameters) const
    {
      auto prefixed = make_prefixed_format(function, format);
      m_logger->warn(prefixed, std::forward<ParameterTypes>(parameters)...);
    }

    template<typename... ParameterTypes>
    auto log_error(std::string function, std::string format, ParameterTypes &&... parameters) const
    {
      auto prefixed = make_prefixed_format(function, format);
      m_logger->error(prefixed, std::forward<ParameterTypes>(parameters)...);
    }

    template<typename... ParameterTypes>
    auto log_critical(std::string function, std::string format, ParameterTypes &&... parameters) const
    {
      auto prefixed = make_prefixed_format(function, format);
      m_logger->critical(prefixed, std::forward<ParameterTypes>(parameters)...);
    }

  private:
    auto make_prefixed_format(std::string const & function, std::string const & format) const -> std::string
    {
      return fmt::format("{}::{}: {}", m_class_name, function, format);
    }

    logger m_logger;
    std::string_view m_class_name;
  };

}  // namespace mp

#endif