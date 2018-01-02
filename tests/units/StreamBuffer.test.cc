
// #include <gtest/gtest.h>

// #include "cosy/StreamBuffer.h"

// const std::string resources = "tests/resources/";

// TEST(StreamBufferTest, FileNotExist) {
//     std::string path = resources + "not_exist.cnf";
//     StreamBuffer in(path);
//     ASSERT_FALSE(in.valid());
// }

// TEST(StreamBufferTest, FileExist) {
//     std::string path = resources + "exist.cnf";
//     StreamBuffer in(path);
//     ASSERT_TRUE(in.valid());
// }

// TEST(StreamBufferTest, OperatorStar) {
//     std::string path = resources + "exist.cnf";
//     StreamBuffer in(path);

//     ASSERT_TRUE(in.valid());
//     ASSERT_EQ(*in, 'p');
// }

// TEST(StreamBufferTest, OperatorPlusPlus) {
//     std::string path = resources + "exist.cnf";
//     StreamBuffer in(path);

//     ASSERT_TRUE(in.valid());
//     ASSERT_EQ(*in, 'p');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, 'c');
//     ++in; ASSERT_EQ(*in, 'n');
//     ++in; ASSERT_EQ(*in, 'f');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, '3');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, '2');
//     ++in; ASSERT_EQ(*in, '\n');
//     ++in; ASSERT_EQ(*in, '-');
//     ++in; ASSERT_EQ(*in, '2');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, '3');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, '0');
//     ++in; ASSERT_EQ(*in, '\n');
//     ++in; ASSERT_EQ(*in, '1');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, '2');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, '0');
//     ++in; ASSERT_EQ(*in, '\n');
//     ++in; ASSERT_EQ(*in, ' ');
// }

// TEST(StreamBufferTest, OperatorParseInt) {
//     std::string path = resources + "exist.cnf";
//     StreamBuffer in(path);
//     int read;

//     ASSERT_TRUE(in.valid());
//     ASSERT_EQ(*in, 'p');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, 'c');
//     ++in; ASSERT_EQ(*in, 'n');
//     ++in; ASSERT_EQ(*in, 'f');
//     ++in; ASSERT_EQ(*in, ' ');

//     read = in.parseInt();
//     ASSERT_EQ(read, 3);

//     read = in.parseInt();
//     ASSERT_EQ(read, 2);

//     read = in.parseInt();
//     ASSERT_EQ(read, -2);

//     read = in.parseInt();
//     ASSERT_EQ(read, 3);

//     read = in.parseInt();
//     ASSERT_EQ(read, 0);

//     read = in.parseInt();
//     ASSERT_EQ(read, 1);

//     read = in.parseInt();
//     ASSERT_EQ(read, 2);

//     read = in.parseInt();
//     ASSERT_EQ(read, 0);
// }

// TEST(StreamBufferTest, SkipLine) {
//     std::string path = resources + "exist.cnf";
//     StreamBuffer in(path);

//     in.skipLine();
//     ASSERT_EQ(*in, '-');
//     ++in; ASSERT_EQ(*in, '2');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, '3');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, ' ');
//     ++in; ASSERT_EQ(*in, '0');
// }
