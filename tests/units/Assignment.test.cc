
#include <gtest/gtest.h>

#include "cosy/Assignment.h"

namespace cosy {

TEST(AssignmentTest, empty) {
    Assignment assignment;

    ASSERT_EQ(assignment.numberOfVariables(), static_cast<unsigned int>(0));
}

TEST(AssignmentTest, init) {
    Assignment assignment(6);

    ASSERT_EQ(assignment.numberOfVariables(), static_cast<unsigned int>(6));
}

TEST(AssignmentTest, resize) {
    Assignment assignment(6);

    ASSERT_EQ(assignment.numberOfVariables(), static_cast<unsigned int>(6));
    assignment.resize(12);
    ASSERT_EQ(assignment.numberOfVariables(), static_cast<unsigned int>(12));
}


TEST(AssignmentTest, assignFromTruePositiveLiteral) {
    Assignment assignment(6);
    Literal literal(1);

    assignment.assignFromTrueLiteral(literal);
    EXPECT_TRUE(assignment.variableIsAssigned(literal.variable()));
    EXPECT_TRUE(assignment.literalIsAssigned(literal));
    EXPECT_TRUE(assignment.literalIsTrue(literal));
    EXPECT_FALSE(assignment.literalIsFalse(literal));
}

TEST(AssignmentTest, assignFromTrueNegativeLiteral) {
    Assignment assignment(6);
    Literal literal(-4);

    assignment.assignFromTrueLiteral(literal);
    EXPECT_TRUE(assignment.variableIsAssigned(literal.variable()));
    EXPECT_TRUE(assignment.literalIsAssigned(literal));
    EXPECT_TRUE(assignment.literalIsTrue(literal));
    EXPECT_FALSE(assignment.literalIsFalse(literal));
}

TEST(AssignmentTest, unassignLiteral) {
    Assignment assignment(6);
    Literal literal(-4);

    assignment.assignFromTrueLiteral(literal);
    assignment.unassignLiteral(literal);
    EXPECT_FALSE(assignment.variableIsAssigned(literal.variable()));
    EXPECT_FALSE(assignment.literalIsAssigned(literal));
}

TEST(AssignmentTest, hasSameAssignmentValue) {
    Assignment assignment(6);
    Literal x(-1);
    Literal y(2);

    ASSERT_FALSE(assignment.hasSameAssignmentValue(x, y));
    assignment.assignFromTrueLiteral(x);
    ASSERT_FALSE(assignment.hasSameAssignmentValue(x, y));
    assignment.assignFromTrueLiteral(y);
    ASSERT_TRUE(assignment.hasSameAssignmentValue(x, y));
    assignment.unassignLiteral(x);
    ASSERT_FALSE(assignment.hasSameAssignmentValue(x, y));
    assignment.assignFromTrueLiteral(x.negated());
    ASSERT_FALSE(assignment.hasSameAssignmentValue(x, y));
    ASSERT_TRUE(assignment.hasSameAssignmentValue(x.negated(), y));
}

TEST(AssignmentTest, getTrueLiteralForAssignedVariable) {
    Assignment assignment(6);
    Literal literal(-4);

    assignment.assignFromTrueLiteral(literal);
    Literal lit =
        assignment.getTrueLiteralForAssignedVariable(literal.variable());

    ASSERT_EQ(literal, lit);
}

TEST(AssignmentTest, getFalseLiteralForAssignedVariable) {
    Assignment assignment(6);
    Literal literal(-4);

    assignment.assignFromTrueLiteral(literal);
    Literal lit =
        assignment.getFalseLiteralForAssignedVariable(literal.variable());

    ASSERT_EQ(literal.negated(), lit);
}


}  // namespace cosy
