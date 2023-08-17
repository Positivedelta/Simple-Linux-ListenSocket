//
// (c) Bit Parallel Ltd, March 2023
//

#include "copyable_atomic.hpp"

//
// the std::atomic<T> implementation is not trivially copyable which prevents it from being used in conjunction
// with std::optional<T>, this template adds the ability to make copies, but does not provide any src/dst synchronisation
//

template<class T>
bpl::CopyableAtomic<T>::CopyableAtomic() = default;

template<class T>
bpl::CopyableAtomic<T>::CopyableAtomic(T value):
    std::atomic<T>(value) {
}

template<class T>
bpl::CopyableAtomic<T>::CopyableAtomic(const bpl::CopyableAtomic<T>& value):
    bpl::CopyableAtomic<T>(value.load(std::memory_order_acquire)) {
}

template<class T>
bpl::CopyableAtomic<T>& bpl::CopyableAtomic<T>::operator=(const bpl::CopyableAtomic<T>& value)
{
    this->store(value.load(std::memory_order_acquire), std::memory_order_release);
    return *this;
}

template class bpl::CopyableAtomic<bool>;
