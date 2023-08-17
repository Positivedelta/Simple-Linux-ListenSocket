//
// (c) Bit Parallel Ltd, March 2023
//

#ifndef BIT_PARALLEL_COPYABLE_ATOMIC_HPP
#define BIT_PARALLEL_COPYABLE_ATOMIC_HPP

#include <atomic>

//
// the std::atomic<T> implementation is not trivially copyable which prevents it from being used in conjunction
// with std::optional<T>, this template adds the ability to make copies, but does not provide any src/dst synchronisation
//

namespace bpl
{
    template<class T> class CopyableAtomic : public std::atomic<T>
    {
        public:
            CopyableAtomic();
            CopyableAtomic(T value);
            CopyableAtomic(const CopyableAtomic<T>& value);
            CopyableAtomic& operator=(const CopyableAtomic<T>& value);
    };
}

#endif
