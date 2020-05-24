// 200211
#ifndef SDRB_HH
#define SDRB_HH

#include <string>
#include <unordered_map>

#include "constants.hh"
#include "types.hh"

size_t*
InitializeDemoDims(std::string const& datum, size_t cap, bool override = false, size_t new_d0 = 1, size_t new_d1 = 1, size_t new_d2 = 1, size_t new_d3 = 1);

#endif
