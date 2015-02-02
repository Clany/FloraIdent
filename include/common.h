#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <functional>
#include <numeric>
#include <chrono>
#include <random>
#include <opencv2/opencv.hpp>
#include "clany/cls_defs.h"

#if defined _MSC_VER && _MSC_VER <= 1800
#  define constexpr const
#endif

_CLANY_BEGIN
const uint  SEED = random_device()();
static auto rand_engine = default_random_engine(SEED);
_CLANY_END

#endif // COMMON_H