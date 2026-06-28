/**
 * @file StaticTesting.hpp
 * Contains helper code used in static tests.
 */

#pragma once

#define STATIC_ASSERT_EQ(a, b) static_assert(a == b, a);
