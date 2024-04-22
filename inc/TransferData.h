#pragma once

#include <tuple>
#include <type_traits>

template<typename... Params>
struct Data {
public:
// Special Member Functions
  constexpr Data(Params... params) : values(std::make_tuple(params...)) {}

  /*
  * Get Function to Fetch Data from the Tuple
  */
  template<size_t N>
  inline decltype(auto) get() const {
    return std::get<N>(values);
  }

  /*
  * Set Function to Update Data in the Tuple
  */
  template<typename Type, size_t N>
  inline void set(Type value) {
    static_assert(std::is_same_v<decltype(value), decltype(std::get<N>(values))>,
                  "Type mismatch: The provided value does not match the expected type in the tuple.");
    std::get<N>(values) = value;
  }
private:
  std::tuple<Params...> values;
};
