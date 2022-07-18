#include <bc_test.h>

// test function
int test()
{
    return 0;
}

int main(void)
{
    BC_TEST_START();
    BC_TEST(test);
}