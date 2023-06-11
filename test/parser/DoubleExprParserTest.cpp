#include <gtest/gtest.h>
#include <parser/Utils.hpp>


using parser::parse_double_unsafe;

TEST(ParseDoubleTest, PositiveValues)
{
    EXPECT_DOUBLE_EQ(parse_double_unsafe("123"), 123.0);
    EXPECT_DOUBLE_EQ(parse_double_unsafe("123.456"), 123.456);
    EXPECT_DOUBLE_EQ(parse_double_unsafe("123.456e3"), 123456.0);
    EXPECT_DOUBLE_EQ(parse_double_unsafe("123.456E3"), 123456.0);
}

TEST(ParseDoubleTest, Zero)
{
    EXPECT_DOUBLE_EQ(parse_double_unsafe("0"), 0.0);
}

TEST(ParseDoubleTest, SmallValues)
{
    EXPECT_DOUBLE_EQ(parse_double_unsafe("0.123"), 0.123);
    EXPECT_DOUBLE_EQ(parse_double_unsafe("0.123e-3"), 0.000123);
}

TEST(ParseDoubleTest, DecimalFractions)
{
    EXPECT_DOUBLE_EQ(parse_double_unsafe(".123"), 0.123);
    EXPECT_DOUBLE_EQ(parse_double_unsafe(".123e3"), 123.0);
}

TEST(ParseDoubleTest, ExponentialNotation)
{
    EXPECT_DOUBLE_EQ(parse_double_unsafe("1e1"), 10.0);
    EXPECT_DOUBLE_EQ(parse_double_unsafe("1E1"), 10.0);
    EXPECT_DOUBLE_EQ(parse_double_unsafe("1e+1"), 10.0);
    EXPECT_DOUBLE_EQ(parse_double_unsafe("1E-1"), 0.1);
    EXPECT_DOUBLE_EQ(parse_double_unsafe("1.23e-1"), 0.123);
}
