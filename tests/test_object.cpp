#include <gtest/gtest.h>
#include "object.hpp"

/*
    how to run a unit test
    TEST(name of the class, name of the test)
    {
        and then your test, which is basically for comparing the value of the output 
        to the expected value from the function called with the function calls EXPECT_*...
    }

*/

TEST(GitBlob, RoundTrip)
{
    GitBlob blob;
    blob.deserialize("hello");

    EXPECT_EQ(blob.serialize(), "hello");
    EXPECT_EQ(blob.fmt(), "blob");
}


