#ifndef WEBRTC_BASE_SCOPED_PTR_H__
#define WEBRTC_BASE_SCOPED_PTR_H__

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include <algorithm>  // For std::swap().

#include "constructormagic.h"
#include "template_util.h"
#include "typedefs.h"
#include "ObjectStatic.h"
namespace rtc {

template <class T>
struct DefaultDeleter : public smf::ObjectStatic{
  DefaultDeleter() {}
  template <typename U> DefaultDeleter(const DefaultDeleter<U>& other) {
    enum { T_must_be_complete = sizeof(T) };
    enum { U_must_be_complete = sizeof(U) };
    static_assert(rtc::is_convertible<U*, T*>::value,
                  "U* must implicitly convert to T*");
  }
  inline void operator()(T* ptr) const {
    enum { type_must_be_complete = sizeof(T) };
    // delete ptr;
    using namespace smf;
    Free(ptr);
  }
};

// Specialization of DefaultDeleter for array types.
template <class T>
struct DefaultDeleter<T[]> : public smf::ObjectStatic{
  inline void operator()(T* ptr) const {
    enum { type_must_be_complete = sizeof(T) };
    using namespace smf;
    Free(ptr);
  }

 private:
  template <typename U> void operator()(U* array) const;
};

template <class T, int n>
struct DefaultDeleter<T[n]> : public smf::ObjectStatic{
  // Never allow someone to declare something like scoped_ptr<int[10]>.
  static_assert(sizeof(T) == -1, "do not use array with size as type");
};

namespace internal {

    template <typename T>
    struct ShouldAbortOnSelfReset {
        template <typename U>
        static rtc::internal::NoType Test(const typename U::AllowSelfReset*);

        template <typename U>
        static rtc::internal::YesType Test(...);

        static const bool value =
            sizeof(Test<T>(0)) == sizeof(rtc::internal::YesType);
    };

    template <class T, class D>
    class scoped_ptr_impl {
    public:
        explicit scoped_ptr_impl(T* p) : data_(p) {}

        // Initializer for deleters that have data parameters.
        scoped_ptr_impl(T* p, const D& d) : data_(p, d) {}

        // Templated constructor that destructively takes the value from another
        // scoped_ptr_impl.
        template <typename U, typename V>
        scoped_ptr_impl(scoped_ptr_impl<U, V>* other)
            : data_(other->release(), other->get_deleter()) {
        }

        template <typename U, typename V>
        void TakeState(scoped_ptr_impl<U, V>* other) {
            // See comment in templated constructor above regarding lack of support
            // for move-only deleters.
            reset(other->release());
            get_deleter() = other->get_deleter();
        }

        ~scoped_ptr_impl() {
            if (data_.ptr != nullptr) {
                // Not using get_deleter() saves one function call in non-optimized
                // builds.
                static_cast<D&>(data_)(data_.ptr);
            }
        }

        void reset(T* p) {
            assert(!ShouldAbortOnSelfReset<D>::value || p == nullptr || p != data_.ptr);
            T* old = data_.ptr;
            data_.ptr = nullptr;
            if (old != nullptr)
                static_cast<D&>(data_)(old);
            data_.ptr = p;
        }

        T* get() const { return data_.ptr; }

        D& get_deleter() { return data_; }
        const D& get_deleter() const { return data_; }

        void swap(scoped_ptr_impl& p2) {
            using std::swap;
            swap(static_cast<D&>(data_), static_cast<D&>(p2.data_));
            swap(data_.ptr, p2.data_.ptr);
        }

        T* release() {
            T* old_ptr = data_.ptr;
            data_.ptr = nullptr;
            return old_ptr;
        }

        T** accept() {
            reset(nullptr);
            return &(data_.ptr);
        }

        T** use() {
            return &(data_.ptr);
        }

    private:
        template <typename U, typename V> friend class scoped_ptr_impl;
        struct Data : public D {
            explicit Data(T* ptr_in) : ptr(ptr_in) {}
            Data(T* ptr_in, const D& other) : D(other), ptr(ptr_in) {}
            T* ptr;
        };

        Data data_;

        RTC_DISALLOW_COPY_AND_ASSIGN(scoped_ptr_impl);
    };

}  // namespace internal
template <class T, class D = rtc::DefaultDeleter<T> >
class scoped_ptr {

  // TODO(ajm): If we ever import RefCountedBase, this check needs to be
  // enabled.
  //static_assert(rtc::internal::IsNotRefCounted<T>::value,
  //              "T is refcounted type and needs scoped refptr");

 public:
  // The element and deleter types.
  typedef T element_type;
  typedef D deleter_type;

  scoped_ptr() : impl_(nullptr) {}

  explicit scoped_ptr(element_type* p) : impl_(p) {}
  scoped_ptr(element_type* p, const D& d) : impl_(p, d) {}
  scoped_ptr(decltype(nullptr)) : impl_(nullptr) {}
  template <typename U, typename V>
  scoped_ptr(scoped_ptr<U, V>&& other)
      : impl_(&other.impl_) {
    static_assert(!rtc::is_array<U>::value, "U cannot be an array");
  }
  template <typename U, typename V>
  scoped_ptr& operator=(scoped_ptr<U, V>&& rhs) {
    static_assert(!rtc::is_array<U>::value, "U cannot be an array");
    impl_.TakeState(&rhs.impl_);
    return *this;
  }

  // operator=.  Allows assignment from a nullptr. Deletes the currently owned
  // object, if any.
  scoped_ptr& operator=(decltype(nullptr)) {
    reset();
    return *this;
  }

  // Deleted copy constructor and copy assignment, to make the type move-only.
  scoped_ptr(const scoped_ptr& other) = delete;
  scoped_ptr& operator=(const scoped_ptr& other) = delete;

  // Get an rvalue reference. (sp.Pass() does the same thing as std::move(sp).)
  scoped_ptr&& Pass() { return static_cast<scoped_ptr&&>(*this); }


  void reset(element_type* p = nullptr) { impl_.reset(p); }

  element_type& operator*() const {
    assert(impl_.get() != nullptr);
    return *impl_.get();
  }
  element_type* operator->() const  {
    assert(impl_.get() != nullptr);
    return impl_.get();
  }
  element_type* get() const { return impl_.get(); }

  // Access to the deleter.
  deleter_type& get_deleter() { return impl_.get_deleter(); }
  const deleter_type& get_deleter() const { return impl_.get_deleter(); }

 private:
  typedef rtc::internal::scoped_ptr_impl<element_type, deleter_type>
      scoped_ptr::*Testable;

 public:
  operator Testable() const {
    return impl_.get() ? &scoped_ptr::impl_ : nullptr;
  }

  bool operator==(const element_type* p) const { return impl_.get() == p; }
  bool operator!=(const element_type* p) const { return impl_.get() != p; }

  // Swap two scoped pointers.
  void swap(scoped_ptr& p2) {
    impl_.swap(p2.impl_);
  }

  element_type* release() WARN_UNUSED_RESULT {
    return impl_.release();
  }

  // Delete the currently held pointer and return a pointer
  // to allow overwriting of the current pointer address.
  element_type** accept() WARN_UNUSED_RESULT {
    return impl_.accept();
  }

  // Return a pointer to the current pointer address.
  element_type** use() WARN_UNUSED_RESULT {
    return impl_.use();
  }

 private:
  // Needed to reach into |impl_| in the constructor.
  template <typename U, typename V> friend class scoped_ptr;
  rtc::internal::scoped_ptr_impl<element_type, deleter_type> impl_;

  // Forbidden for API compatibility with std::unique_ptr.
  explicit scoped_ptr(int disallow_construction_from_null);

  template <class U> bool operator==(scoped_ptr<U> const& p2) const;
  template <class U> bool operator!=(scoped_ptr<U> const& p2) const;
};

template <class T, class D>
class scoped_ptr<T[], D> {
public:
    // The element and deleter types.
    typedef T element_type;
    typedef D deleter_type;
    scoped_ptr() : impl_(nullptr) {}
    explicit scoped_ptr(element_type* array) : impl_(array) {}
    scoped_ptr(decltype(nullptr)) : impl_(nullptr) {}
    scoped_ptr(scoped_ptr&& other) : impl_(&other.impl_) {}
    scoped_ptr& operator=(scoped_ptr&& rhs) {
        impl_.TakeState(&rhs.impl_);
        return *this;
    }
    scoped_ptr& operator=(decltype(nullptr)) {
        reset();
        return *this;
    }
    scoped_ptr(const scoped_ptr& other) = delete;
    scoped_ptr& operator=(const scoped_ptr& other) = delete;
    scoped_ptr&& Pass() { return static_cast<scoped_ptr&&>(*this); }
    void reset(element_type* array = nullptr) { impl_.reset(array); }
    element_type& operator[](size_t i) const {
        assert(impl_.get() != nullptr);
        return impl_.get()[i];
    }
    element_type* get() const { return impl_.get(); }
    deleter_type& get_deleter() { return impl_.get_deleter(); }
    const deleter_type& get_deleter() const { return impl_.get_deleter(); }
private:
    typedef rtc::internal::scoped_ptr_impl<element_type, deleter_type>scoped_ptr::* Testable;
public:
    operator Testable() const {
        return impl_.get() ? &scoped_ptr::impl_ : nullptr;
    }
    bool operator==(element_type* array) const { return impl_.get() == array; }
    bool operator!=(element_type* array) const { return impl_.get() != array; }    
    void swap(scoped_ptr& p2) {
        impl_.swap(p2.impl_);
    }
    element_type* release() WARN_UNUSED_RESULT {
        return impl_.release();
    }
    element_type** accept() WARN_UNUSED_RESULT {
        return impl_.accept();
    }
    element_type** use() WARN_UNUSED_RESULT {
        return impl_.use();
    }

private:    
    enum { type_must_be_complete = sizeof(element_type) };
    rtc::internal::scoped_ptr_impl<element_type, deleter_type> impl_;
    template <typename U> explicit scoped_ptr(U* array);
    explicit scoped_ptr(int disallow_construction_from_null);
    template <typename U> void reset(U* array);
    void reset(int disallow_reset_from_null);
    template <class U> bool operator==(scoped_ptr<U> const& p2) const;
    template <class U> bool operator!=(scoped_ptr<U> const& p2) const;
};

template <class T, class D>
void swap(rtc::scoped_ptr<T, D>& p1, rtc::scoped_ptr<T, D>& p2) {
  p1.swap(p2);
}

}  // namespace rtc

template <class T, class D>
bool operator==(T* p1, const rtc::scoped_ptr<T, D>& p2) {
  return p1 == p2.get();
}

template <class T, class D>
bool operator!=(T* p1, const rtc::scoped_ptr<T, D>& p2) {
  return p1 != p2.get();
}

template <typename T>
rtc::scoped_ptr<T> rtc_make_scoped_ptr(T* ptr) {
  return rtc::scoped_ptr<T>(ptr);
}

#endif  // #ifndef WEBRTC_BASE_SCOPED_PTR_H__
