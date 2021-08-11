#pragma once

#include <variant>
#include <string>

template <typename... Base> struct VariantVisitor : Base...
{
    using Base::operator()...;
};

template <typename... T> VariantVisitor(T...)->VariantVisitor<T...>;


