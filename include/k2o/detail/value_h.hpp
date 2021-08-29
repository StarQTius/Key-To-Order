//! \file
//! \brief Value representation through type

#pragma once

namespace k2o {
namespace detail {

//! \brief Holds a value
//! \tparam T Type of the held value
//! \tparam Value value to be held
template<typename T, T Value>
struct value_h {
  constexpr static auto value = Value;
};

} // namespace detail
} // namespace k2o