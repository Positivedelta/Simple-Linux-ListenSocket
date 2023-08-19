//
// (c) Bit Parallel Ltd, August 2023
//

#ifndef BIT_PARALLEL_READ_LISTENER_HPP
#define BIT_PARALLEL_READ_LISTENER_HPP

#include <cstdint>
#include <functional>

namespace bpl
{
    using ReadListener = std::function<void (const uint8_t bytes[], const int32_t length)>;
}

#endif
