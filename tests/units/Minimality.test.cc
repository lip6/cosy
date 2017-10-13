
#include <gtest/gtest.h>

#include "cosy/Minimality.h"

using namespace cosy;


TEST(MinimalityTest, Minimal) {
    Minimality minimality;

    ASSERT_TRUE(minimality.minimal());
}


TEST(MinimalityTest, notMinimal) {
    Minimality minimality;

    std::vector<Lit> reason({2, -3});
    Lit cause = -3;

    minimality.add(cause, reason);
    ASSERT_FALSE(minimality.minimal());
}

TEST(MinimalityTest, sbp) {
    Minimality minimality;

    std::vector<Lit> reason({2, -3});
    std::vector<Lit> r;
    Lit cause = -3;

    minimality.add(cause, reason);
    ASSERT_FALSE(minimality.minimal());

    minimality.sbp(&r);
    ASSERT_EQ(reason, r);

    ASSERT_TRUE(minimality.minimal());
}

TEST(MinimalityTest, Cause) {
    Minimality minimality;

    std::vector<Lit> reason({2, -3});
    std::vector<Lit> r;
    Lit cause = -3;

    minimality.add(cause, reason);
    ASSERT_FALSE(minimality.minimal());

    ASSERT_EQ(minimality.cause(), static_cast<Var>(3));
}

TEST(MinimalityTest, RemoveOK) {
    Minimality minimality;

    std::vector<Lit> reason({2, -3});
    std::vector<Lit> r;
    Lit cause = -3;

    minimality.add(cause, reason);
    ASSERT_FALSE(minimality.minimal());

    minimality.removeIfCause(cause);
    ASSERT_TRUE(minimality.minimal());
}

TEST(MinimalityTest, NotRemove) {
    Minimality minimality;

    std::vector<Lit> reason({2, -3});
    std::vector<Lit> r;
    Lit cause = -3, other = 42;

    minimality.add(cause, reason);
    ASSERT_FALSE(minimality.minimal());

    minimality.removeIfCause(other);
    ASSERT_FALSE(minimality.minimal());
}
