/*
 Tests for the capabilities-based memory allocator.
*/

#include <esp_types.h>
#include <stdio.h>
#include "unity.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_spi_flash.h"
#include <stdlib.h>
#include <sys/param.h>
#include <string.h>

TEST_CASE("Capabilities aligned allocator test", "[heap]")
{
    uint32_t alignments = 0;

    printf("[ALIGNED_ALLOC] Allocating from default CAP: \n");
    
    for(;alignments <= 1024; alignments++) {
        uint8_t *buf = (uint8_t *)heap_caps_aligned_alloc(alignments, (alignments + 137), MALLOC_CAP_DEFAULT);
        if(((alignments & (alignments - 1)) != 0) || (!alignments)) {
            TEST_ASSERT( buf == NULL );
            //printf("[ALIGNED_ALLOC] alignment: %u is not a power of two, don't allow allocation \n", aligments);
        } else {
            TEST_ASSERT( buf != NULL );
            printf("[ALIGNED_ALLOC] alignment required: %u \n", alignments);
            printf("[ALIGNED_ALLOC] address of allocated memory: %p \n\n", (void *)buf);
            //Address of obtained block must be aligned with selected value
            TEST_ASSERT(((intptr_t)buf & (alignments - 1)) == 0);

            //Write some data, if it corrupts memory probably the heap
            //canary verification will fail:
            memset(buf, 0xA5, (alignments + 137));

            heap_caps_aligned_free(buf);    
        }
    } 

    //Alloc from a non permitted area:
    uint32_t *not_permitted_buf = (uint32_t *)heap_caps_aligned_alloc(alignments, (alignments + 137), MALLOC_CAP_EXEC | MALLOC_CAP_32BIT);
    TEST_ASSERT( not_permitted_buf == NULL );

#if CONFIG_ESP32_SPIRAM_SUPPORT || CONFIG_ESP32S2_SPIRAM_SUPPORT
    alignments = 0;
    printf("[ALIGNED_ALLOC] Allocating from external memory: \n");

    for(;alignments <= 1024 * 1024; alignments++) {
        //Now try to take aligned memory from IRAM:
        uint8_t *buf = (uint8_t *)heap_caps_aligned_alloc(alignments, 10*1024, MALLOC_CAP_SPIRAM);
        if(((alignments & (alignments - 1)) != 0) || (!alignments)) {
            TEST_ASSERT( buf == NULL );
            //printf("[ALIGNED_ALLOC] alignment: %u is not a power of two, don't allow allocation \n", aligments);
        } else {
            TEST_ASSERT( buf != NULL );
            printf("[ALIGNED_ALLOC] alignment required: %u \n", alignments);
            printf("[ALIGNED_ALLOC] address of allocated memory: %p \n\n", (void *)buf);
            //Address of obtained block must be aligned with selected value
            TEST_ASSERT(((intptr_t)buf & (alignments - 1)) == 0);

            //Write some data, if it corrupts memory probably the heap
            //canary verification will fail:
            memset(buf, 0xA5, (10*1024));
            heap_caps_aligned_free(buf);    
        }
    } 
#endif

}