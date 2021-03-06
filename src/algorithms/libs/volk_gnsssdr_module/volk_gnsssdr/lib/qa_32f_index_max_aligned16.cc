/* Copyright (C) 2010-2015 (see AUTHORS file for a list of contributors)
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 */

#include <volk_gnsssdr/volk_gnsssdr_runtime.h>
#include <volk_gnsssdr/volk_gnsssdr.h>
#include <qa_32f_index_max_aligned16.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERR_DELTA (1e-4)
#define NUM_ITERS 1000000
#define VEC_LEN 3097

static float uniform()
{
    return 2.0 * ((float) rand() / RAND_MAX - 0.5);	// uniformly (-1, 1)
}

static void
random_floats (float *buf, unsigned n)
{
    unsigned int i = 0;
    for (; i < n; i++)
        {
            buf[i] = uniform () * 32767;
        }
}


#ifndef LV_HAVE_SSE

void qa_32f_index_max_aligned16::t1()
{
    printf("sse not available... no test performed\n");
}

#else


void qa_32f_index_max_aligned16::t1()
{
    const int vlen = VEC_LEN;

    volk_gnsssdr_runtime_init();

    volk_gnsssdr_environment_init();
    int ret;

    unsigned int* target_sse4_1;
    unsigned int* target_sse;
    unsigned int* target_generic;
    float* src0 ;

    unsigned int i_target_sse4_1;
    target_sse4_1 = &i_target_sse4_1;
    unsigned int i_target_sse;
    target_sse = &i_target_sse;
    unsigned int i_target_generic;
    target_generic = &i_target_generic;

    ret = posix_memalign((void**)&src0, 16, vlen *sizeof(float));

    random_floats((float*)src0, vlen);

    printf("32f_index_max_aligned16\n");

    clock_t start, end;
    double total;

    start = clock();
    for(int k = 0; k < NUM_ITERS; ++k)
        {
            volk_gnsssdr_32f_index_max_aligned16_manual(target_generic, src0, vlen, "generic");
        }
    end = clock();
    total = (double)(end-start)/(double)CLOCKS_PER_SEC;
    printf("generic time: %f\n", total);

    start = clock();
    for(int k = 0; k < NUM_ITERS; ++k)
        {
            volk_gnsssdr_32f_index_max_aligned16_manual(target_sse, src0, vlen, "sse2");
        }

    end = clock();
    total = (double)(end-start)/(double)CLOCKS_PER_SEC;
    printf("sse time: %f\n", total);

    start = clock();
    for(int k = 0; k < NUM_ITERS; ++k)
        {
            get_volk_gnsssdr_runtime()->volk_gnsssdr_32f_index_max_aligned16(target_sse4_1, src0, vlen);
        }

    end = clock();
    total = (double)(end-start)/(double)CLOCKS_PER_SEC;
    printf("sse4.1 time: %f\n", total);

    printf("generic: %u, sse: %u, sse4.1: %u\n", target_generic[0], target_sse[0], target_sse4_1[0]);
    CPPUNIT_ASSERT_EQUAL(target_generic[0], target_sse[0]);
    CPPUNIT_ASSERT_EQUAL(target_generic[0], target_sse4_1[0]);

    free(src0);
}

#endif /*LV_HAVE_SSE3*/
