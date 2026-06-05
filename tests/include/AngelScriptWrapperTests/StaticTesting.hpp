/**
 * @file StaticTesting.hpp
 * Contains helper code used in static tests.
 */

#pragma once

#define COMMA ,
#define STATIC_ASSERT_EQ(a, b) static_assert(a == b, a);
