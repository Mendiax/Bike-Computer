#include "common_utils.hpp"

size_t check_free_mem()
{
    size_t avaible_memory = SIZE_MAX;
    void* mem;
    while((avaible_memory != 0) && (mem = malloc(avaible_memory)) == NULL)
    {
        avaible_memory >>= 1;
    }
    if(mem != NULL)
        free(mem);
    return avaible_memory;
}