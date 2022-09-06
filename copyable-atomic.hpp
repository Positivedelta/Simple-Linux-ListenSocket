//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#ifndef H_COPYABLE_ATOMIC
#define H_COPYABLE_ATOMIC

#include <atomic>

//
// the std::atomic<T> implementation is not trivially copyable which prevents it from being used in conjunction
// with std::optional<T>, this template adds the ability to make copies, but does not provide any src/dst synchronisation
//

template<class T> class CopyableAtomic : public std::atomic<T>
{
    public:
        CopyableAtomic() = default;

        constexpr CopyableAtomic(T value):
            std::atomic<T>(value) {
        }

        constexpr CopyableAtomic(const CopyableAtomic<T>& value):
            CopyableAtomic(value.load(std::memory_order_acquire)) {
        }

        CopyableAtomic& operator=(const CopyableAtomic<T>& value)
        {
            this->store(value.load(std::memory_order_acquire), std::memory_order_release);
            return *this;
        }
};

#endif
