/*
 * MIT License
 * Copyright (c) 2022 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Increase the value of TEST_DATA_SIZE to reduce cache impact */
#define TEST_DATA_SIZE    (256 * 1024 * 1024)

/* Increase the value of TEST_REPEAT_NUM to reduce timing errors */
#define TEST_REPEAT_NUM   100

#if defined(ARDUINO)
#  define GET_TICK_MS()           millis()
#  define LOG_PRINTF(fmt, ...)    Serial.printf(fmt, ##__VA_ARGS__)
#elif defined (WIN32)
#  include <Windows.h>
#  define GET_TICK_MS()           GetTickCount()
#  define LOG_PRINTF(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#  include <time.h>
#  define GET_TICK_MS()           get_timestamp()
#  define LOG_PRINTF(fmt, ...)    printf(fmt, ##__VA_ARGS__)

static uint32_t get_timestamp(void)
{
    struct timespec ts;
    uint32_t ms;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ms = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return ms;
}

#endif

static void memcpy_speed_test(void* dest, const void* src, size_t size, uint32_t repeat)
{
    uint32_t start = GET_TICK_MS();
    for (uint32_t i = 0; i < repeat; i++)
    {
        memcpy(dest, src, size);
    }
    uint32_t cost_time = GET_TICK_MS() - start;

    if (cost_time == 0)
    {
        LOG_PRINTF("Time-consuming is too short, please increase the <repeat>\r\n");
        return;
    }

    uint64_t total_size = (uint64_t)size * repeat;

    float rate = total_size * 1000 / cost_time / 1024.0f / 1024.0f;
    LOG_PRINTF(
        "memcpy(): rate = %0.2f MB/s [cost: %d ms]\r\n",
        rate,
        (int)cost_time
    );
}

static void memset_speed_test(void* dest, uint8_t value, size_t size, uint32_t repeat)
{
    uint32_t start = GET_TICK_MS();
    for (uint32_t i = 0; i < repeat; i++)
    {
        memset(dest, value, size);
    }
    uint32_t cost_time = GET_TICK_MS() - start;

    if (cost_time == 0)
    {
        LOG_PRINTF("Time-consuming is too short, please increase the <repeat>\r\n");
        return;
    }

    uint64_t total_size = (uint64_t)size * repeat;

    float rate = total_size * 1000 / cost_time / 1024.0f / 1024.0f;
    LOG_PRINTF(
        "memset(): rate = %0.2f MB/s [cost: %d ms]\r\n",
        rate,
        (int)cost_time
    );
}

int main(void)
{
    static const uint32_t test_data_rd[TEST_DATA_SIZE / sizeof(uint32_t)];
    static uint32_t test_data_wr[TEST_DATA_SIZE / sizeof(uint32_t)];
    memcpy_speed_test(test_data_wr, test_data_rd, TEST_DATA_SIZE, TEST_REPEAT_NUM);
    memset_speed_test(test_data_wr, 0x55, TEST_DATA_SIZE, TEST_REPEAT_NUM);
    return 0;
}
