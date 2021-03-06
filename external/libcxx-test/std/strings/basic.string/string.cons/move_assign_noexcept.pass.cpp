/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

// <string>

// basic_string& operator=(basic_string&& c)
//     noexcept(
//         allocator_traits<allocator_type>::propagate_on_container_move_assignment::value ||
//         allocator_traits<allocator_type>::is_always_equal::value); // C++17
//
//  before C++17, we use the conforming extension
//     noexcept(
//         allocator_type::propagate_on_container_move_assignment::value &&
//         is_nothrow_move_assignable<allocator_type>::value);

#include <string>
#include <cassert>
#include "libcxx_tc_common.h"

#include "test_macros.h"
#include "test_allocator.h"

template <class T>
struct some_alloc
{
    typedef T value_type;
    some_alloc(const some_alloc&);
};

template <class T>
struct some_alloc2
{
    typedef T value_type;

    some_alloc2() {}
    some_alloc2(const some_alloc2&);
    void deallocate(void*, unsigned) {}

    typedef std::false_type propagate_on_container_move_assignment;
    typedef std::true_type is_always_equal;
};

template <class T>
struct some_alloc3
{
    typedef T value_type;

    some_alloc3() {}
    some_alloc3(const some_alloc3&);
    void deallocate(void*, unsigned) {}

    typedef std::false_type propagate_on_container_move_assignment;
    typedef std::false_type is_always_equal;
};

int tc_libcxx_strings_string_cons_move_assign_noexcept(void)
{
    {
        typedef std::string C;
        static_assert(std::is_nothrow_move_assignable<C>::value, "");
    }
    {
        typedef std::basic_string<char, std::char_traits<char>, test_allocator<char>> C;
        static_assert(!std::is_nothrow_move_assignable<C>::value, "");
    }
    {
        typedef std::basic_string<char, std::char_traits<char>, some_alloc<char>> C;
#if TEST_STD_VER > 14
    //  if the allocators are always equal, then the move assignment can be noexcept
        static_assert( std::is_nothrow_move_assignable<C>::value, "");
#else
        static_assert(!std::is_nothrow_move_assignable<C>::value, "");
#endif
    }
#if TEST_STD_VER > 14
    {
    //  POCMA is false, always equal
        typedef std::basic_string<char, std::char_traits<char>, some_alloc2<char>> C;
        static_assert( std::is_nothrow_move_assignable<C>::value, "");
    }
    {
    //  POCMA is false, not always equal
        typedef std::basic_string<char, std::char_traits<char>, some_alloc3<char>> C;
        static_assert(!std::is_nothrow_move_assignable<C>::value, "");
    }
#endif
    TC_SUCCESS_RESULT();
    return 0;
}
