#pragma once

#include <memory>

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using Unique = std::unique_ptr<T>;