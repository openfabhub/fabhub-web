#ifndef MP_TEST_SUPPORT_LOGGER_MIXIN_SUITE_HPP
#define MP_TEST_SUPPERT_LOGGER_MIXIN_SUITE_HPP

#include <cute/cute_suite.h>

#include <string>
#include <utility>

auto logger_mixin_suite() -> std::pair<cute::suite, std::string>;

#endif