//
//  Chip8ConstructorTest.cpp
//  Chip8Tests
//
//  Created by Ruijing Li on 9/19/18.
//  Copyright © 2018 Ruijing. All rights reserved.
//

// Step 1. Include necessary header files such that the stuff your
// test logic needs is declared.
//
// Don't forget gtest.h, which declares the testing framework.
#include <stdio.h>
//#include "gtest/gtest.h"
#include "chip8.hpp"
#include <GoogleMock/GoogleMock.h>

namespace {
    // Step 2. Use the TEST macro to define your tests.
    //
    // TEST has two parameters: the test case name and the test name.
    // After using the macro, you should define your test logic between a
    // pair of braces.  You can use a bunch of macros to indicate the
    // success or failure of a test.  EXPECT_TRUE and EXPECT_EQ are
    // examples of such macros.  For a complete list, see gtest.h.
    //
    // <TechnicalDetails>
    //
    // In Google Test, tests are grouped into test cases.  This is how we
    // keep test code organized.  You should put logically related tests
    // into the same test case.
    //
    // The test case name and the test name should both be valid C++
    // identifiers.  And you should not use underscore (_) in the names.
    //
    // Google Test guarantees that each test you define is run exactly
    // once, but it makes no guarantee on the order the tests are
    // executed.  Therefore, you should write your tests in such a way
    // that their results don't depend on their order.
    //
    // </TechnicalDetails>
    
    
    // Tests Chip8().
    
    // Tests constructor is working properly.
    TEST(Chip8Test, Constructor) {
        // This test is named "Constructor", and belongs to the "Chip8Test"
        // test case.
        Chip8 myChip8;
        EXPECT_EQ(myChip8.pc, 0x200);
        EXPECT_EQ(myChip8.opcode, 0);
        EXPECT_EQ(myChip8.sp, 0);
        EXPECT_EQ(myChip8.I, 0);
        unsigned char testarr1[2048] = { };
        EXPECT_THAT(myChip8.gfx, testing::ElementsAreArray(testarr1, 2048));

        
        // <TechnicalDetails>
        //
        // EXPECT_EQ(expected, actual) is the same as
        //
        //   EXPECT_TRUE((expected) == (actual))
        //
        // except that it will print both the expected value and the actual
        // value when the assertion fails.  This is very helpful for
        // debugging.  Therefore in this case EXPECT_EQ is preferred.
        //
        // On the other hand, EXPECT_TRUE accepts any Boolean expression,
        // and is thus more general.
        //
        // </TechnicalDetails>
    }
    
}  // namespace

// Step 3. Call RUN_ALL_TESTS() in main().
//
// We do this by linking in src/gtest_main.cc file, which consists of
// a main() function which calls RUN_ALL_TESTS() for us.
//
// This runs all the tests you've defined, prints the result, and
// returns 0 if successful, or 1 otherwise.
//
// Did you notice that we didn't register the tests?  The
// RUN_ALL_TESTS() macro magically knows about all the tests we
// defined.  Isn't this convenient?

