#pragma once
#include <cstdint>
#include "../config/constants.hpp"

using price_t = uint64_t;
using timestamp_t = uint64_t;


// I have intentionally kept the bid and ask as uint64_t
struct alignas(CACHE_LINE) Message {
  char instrument[MAX_INSTRUMENT_NAME_SIZE];
  price_t bid;
  price_t ask;
  timestamp_t timestamp_ns;
};