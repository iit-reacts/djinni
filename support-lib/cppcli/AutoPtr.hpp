#pragma once

#include "Assert.hpp"

template <typename T>
ref struct AutoPtr {
    AutoPtr() : _ptr(0) {}
    AutoPtr(T* ptr) : _ptr(ptr) {}
    AutoPtr(AutoPtr<T>% right) : _ptr(right.Release()) {}

    ~AutoPtr() {
        delete _ptr;
        _ptr = 0;
    }
    !AutoPtr() {
        //ASSERT(0 == _ptr);
        delete _ptr;
    }
    T* operator->() {
        if (0 == _ptr) {
            throw gcnew System::ObjectDisposedException(System::String::Empty);
        }

        return _ptr;
    }

    T* GetPointer() {
        return _ptr;
    }
    T& GetRef() {
        if (0 == _ptr) {
            throw gcnew System::ObjectDisposedException(System::String::Empty);
        }

        return *_ptr;
    }
    T* Release() {
        T* released = _ptr;
        _ptr = 0;
        return released;
    }
    void Reset() {
        Reset(0);
    }
    void Reset(T* ptr) {
        if (ptr != _ptr) {
            delete _ptr;
            _ptr = ptr;
        }
    }

private:
    T* _ptr;
};
