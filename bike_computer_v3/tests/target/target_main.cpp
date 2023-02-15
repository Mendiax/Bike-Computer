#include "bc_test.h"

int test_basic()
{
    BC_CHECK_EQ(1,1);
    return 0;
}

int main()
{
    BC_TEST(test_basic);
    BC_TEST_END();
    return 0;
}