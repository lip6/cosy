
// Copyright 2017 Hakan Metin
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SAT_UTIL_LOGGING_H_
#define SAT_UTIL_LOGGING_H_

#include <cassert>
#include <iostream>

#include "cosy/integral_types.h"
#include "cosy/macros.h"

namespace {

#define DCHECK_GE(value, cond) (assert((value) >= (cond)))
#define DCHECK_LE(value, cond) (assert((value) <= (cond)))
#define DCHECK_GT(value, cond) (assert((value) >  (cond)))
#define DCHECK_LT(value, cond) (assert((value) <  (cond)))
#define DCHECK_NE(value, cond) (assert((value) != (cond)))
#define DCHECK_EQ(value, cond) (assert((value) == (cond)))
#define DCHECK(cond) (assert((cond)))
#define DCHECK_NOTNULL(cond) (assert((cond != nullptr)))

#define CHECK_GE(value, cond) (assert((value) >= (cond)))
#define CHECK_LE(value, cond) (assert((value) <= (cond)))
#define CHECK_GT(value, cond) (assert((value) >  (cond)))
#define CHECK_LT(value, cond) (assert((value) <  (cond)))
#define CHECK_NE(value, cond) (assert((value) !=  (cond)))
#define CHECK_EQ(value, cond) (assert((value) == (cond)))
#define CHECK(cond) (assert((cond)))
#define CHECK_NOTNULL(cond) (assert((cond != nullptr)))


struct X {
    ~X() { std::cout << std::endl; }
};

#define LOG(mode) (X(), std::cout << __FILE__ << " " << __LINE__ << " ")

}


#endif  // SAT_UTIL_LOGGING_H_
