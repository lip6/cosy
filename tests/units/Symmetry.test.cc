#include <vector>
#include <memory>

#include <gtest/gtest.h>

#include "cosy/Types.h"
#include "cosy/Symmetry.h"
#include "cosy/Assigns.h"
#include "cosy/Permutation.h"

using namespace cosy;

class SymmetryTest : public testing::Test {
 protected:
	virtual void SetUp() {
	    const unsigned int index = 0;
	    const unsigned int nvars = 6;
	    std::vector<Lit> order;

	    std::unique_ptr<Permutation> generator(new Permutation(nvars));

	    assigns = std::unique_ptr<Assigns>(new Assigns(nvars));
	    symmetry = std::unique_ptr<Symmetry>(new Symmetry(*assigns, index));

	    for (int i=1; i<=(int)nvars; i++)
		order.push_back(i);
	    for (int i=1; i<=(int)nvars; i++)
		order.push_back(negate(i));
	    lex.assign(order, T_LESS_F);

	    /* (2 6 3) (4 5) */
	    generator->addToCurrentCycle(2);
	    generator->addToCurrentCycle(6);
	    generator->addToCurrentCycle(3);
	    generator->closeCurrentCycle();
	    generator->addToCurrentCycle(4);
	    generator->addToCurrentCycle(5);
	    generator->closeCurrentCycle();

	    for (int c=0; c<generator->numCycles(); c++) {
		int element = generator->lastElementInCycle(c);
		for (const Lit image : generator->cycle(c)) {
		    symmetry->addInverse(element, image);
		    element = image;
		}
	    }

	    symmetry->addLocalOrder(2);
	    symmetry->addLocalOrder(3);
	    symmetry->addLocalOrder(4);
	    symmetry->addLocalOrder(5);
	    symmetry->addLocalOrder(6);
	}

	LexOrder lex;
	Propagator propagator;
	std::unique_ptr<Assigns> assigns;
	std::unique_ptr<Symmetry> symmetry;
};


TEST_F(SymmetryTest, Initialisation) {
    unsigned int ref = 0;
    ASSERT_EQ(symmetry->lookup_index(), ref);
}

TEST_F(SymmetryTest, Propagator1) {
    std::vector<Lit> reason;
    Lit propagate;

    assigns->setLitTrue(-2, 0);
    symmetry->verify(-2, &propagator, lex);

    ASSERT_TRUE(propagator.propagate());
    propagator.propagation(&propagate, &reason);
    ASSERT_EQ(propagate, -3);
}

TEST_F(SymmetryTest, Propagator2) {
    std::vector<Lit> reason;
    Lit propagate;

    assigns->setLitTrue(3, 0);
    symmetry->verify(3, &propagator, lex);

    ASSERT_TRUE(propagator.propagate());
    propagator.propagation(&propagate, &reason);
    ASSERT_EQ(propagate, 2);
}


TEST_F(SymmetryTest, MinimalitySimple) {
    Minimality minimality;

    Lit lit = -2;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_TRUE(minimality.minimal());

    lit = 3;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_FALSE(minimality.minimal());

}

TEST_F(SymmetryTest, MinimalitySecondCycle) {
    Minimality minimality;

    Lit lit = 2;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_TRUE(minimality.minimal());

    lit = 3;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_TRUE(minimality.minimal());

    lit = 6;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_TRUE(minimality.minimal());

    lit = 5;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_TRUE(minimality.minimal());

    lit = -4;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_FALSE(minimality.minimal());

    ASSERT_EQ(minimality.cause(), varOf(lit));

    minimality.removeIfCause(lit);
    ASSERT_EQ(minimality.cause(), VAR_UNDEF);

    assigns->setLitUndef(lit);
    symmetry->backwardLookupIndex(lit);

    lit = 5;
    assigns->setLitUndef(lit);
    symmetry->backwardLookupIndex(lit);

    symmetry->verify(lit, &minimality, lex);
    ASSERT_TRUE(minimality.minimal());

    lit = -4;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_TRUE(minimality.minimal());

    lit = 5;
    assigns->setLitTrue(lit, 0);
    symmetry->forwardLookupIndex(lit);
    symmetry->verify(lit, &minimality, lex);
    ASSERT_FALSE(minimality.minimal());

    ASSERT_EQ(minimality.cause(), varOf(lit));
}
