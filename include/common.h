#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>
#include <numeric>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "flora_ident_config.h"
#include "clany/clany_macros.h"

#if defined _MSC_VER && _MSC_VER <= 1800
#  define constexpr const
#endif

_CLANY_BEGIN
const uint RAND_SEED = chrono::system_clock::now().time_since_epoch().count();
_CLANY_END

#endif // COMMON_H