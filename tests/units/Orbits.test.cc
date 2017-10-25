
#include <gtest/gtest.h>

#include "cosy/Orbits.h"

using namespace cosy;


class OrbitsTest : public testing::Test {
 protected:
	virtual void SetUp() {
            std::unique_ptr<Permutation> generator(new Permutation(nvars));
            generator->addToCurrentCycle(2);
            generator->addToCurrentCycle(6);
            generator->addToCurrentCycle(3);
            generator->closeCurrentCycle();
            generator->addToCurrentCycle(4);
            generator->addToCurrentCycle(5);
            generator->closeCurrentCycle();

            permutations.emplace_back(generator.release());
            permutationsRef.push_back(permutations.front().get());
        }

        const unsigned int nvars = 6;
        std::unique_ptr<Permutation> generator;
        std::vector<std::unique_ptr<Permutation> > permutations;
        std::vector<Permutation*> permutationsRef;
};

TEST_F(OrbitsTest, Compute) {
    Orbits orbits;

    orbits.compute(permutationsRef);
    ASSERT_EQ(orbits.numOrbits(), static_cast<unsigned int>(2));
}
