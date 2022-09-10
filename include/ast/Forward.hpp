#pragma once

#include <memory>
#include <utility>

namespace ast {

// clang-format off
template<class T>
requires (!std::is_array_v<T>)
// a custom std::unique_ptr like smart ptr which does not allow an empty state
class Forward
// clang-format on
{
public:
    using value_type = T;
    constexpr Forward() = delete;
    constexpr Forward(Forward&&) noexcept = default;
    constexpr Forward(const Forward&) = delete;
    constexpr auto operator=(Forward&&) noexcept -> Forward& = default;
    constexpr auto operator=(const Forward&) noexcept -> Forward& = delete;

    // clang-format off
    constexpr Forward(T* t) noexcept : elem_(t) {}
    constexpr Forward(T&& t) noexcept : elem_(std::make_unique<T>(std::move(t))) {}
    // clang-format on

    constexpr auto operator==(const Forward& rhs) const noexcept
    {
        return *elem_ == *rhs.elem_;
    }

    // clang-format off
    constexpr auto operator*() noexcept -> T& { return *elem_;}
    constexpr auto operator*() const noexcept -> const T& { return *elem_;}
    constexpr auto operator->() noexcept -> T* { return elem_.get();}
    constexpr auto operator->() const noexcept -> const T* { return elem_.get();}
    constexpr auto get() const noexcept -> const T* {return elem_.get();};
    constexpr auto get() noexcept -> T* {return elem_.get();};
    // clang-format on

private:
    std::unique_ptr<T> elem_;
};

// clang-format off
template<class T, class... Args>
requires (!std::is_array_v<T>)
constexpr auto forward(Args&&... args)noexcept
{
    return Forward<T>(new T(std::forward<Args>(args)...));
}
// clang-format on

} // namespace ast
