#include "hdc.h"
#include "klib/khash.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*
 * to implement as static functions:
 * computeNgram -> compute_ngram
 * *cosAngle -> cos_angle
 * computeSumHV -> compute_sum_hv
 * lookupItemMemory -> lookup_item_memory
 * *dotProduct -> dot_product
 * *entrywiseProduct -> entrywise_product
 * circShift -> circ_shift
 * computeSumHV -> compute_sum_hv
 * *norm -> norm
 * initItemMemories -> init_item_memories
 * randperm -> rand_perm
 * genRandomHV -> gen_random_hv
 *
 * to implement as public functions:
 * hdc_init
 * hdc_train
 * hdc_predict
 * hdc_deinit
 */

/**
 * Calculates the dot product of OP1 and OP2.
 * @param op1  First operand
 * @param op2  Second operand
 * @param len  Length of vectors
 * @return Dot product of OP1 and OP2
 */
static double dot_product(double op1[], double op2[], size_t len)
{
    double accum = 0.0;
    for (int i = 0; i < len; i++)
    {
        accum += op1[i] * op2[i];
    }
    return accum;
}

/**
 * Calculates the norm of VEC.
 * @param vec  Input vector
 * @param len  Length of VEC
 * @return Norm of VEC
 */
static double norm(double vec[], size_t len)
{
    return sqrt(dot_product(vec, vec, len));
}

/**
 * Calculates the entrywise product of OP1 and OP2, and places it in DEST.
 * @param dest  Destination vector
 * @param op1   First operand
 * @param op2   Second operand
 * @param len   Length of vectors
 */
static void entrywise_product(double dest[], double op1[], double op2[], size_t len)
{
    for (int i = 0; i < len; i++)
    {
        dest[i] = op1[i] * op2[i];
    }
}

/**
 * Calculates the cosine similarity of OP1 and OP2.
 * @param op1  First operand
 * @param op2  Second operand
 * @param len  Length of vectors
 * @return Cosine similarity of OP1 and OP2.
 */
static double cos_angle(double op1[], double op2[], size_t len)
{
    return dot_product(op1, op2, len) / (norm(op1, len) * norm(op2, len));
}

/**
 * Generates a random permutation of the integers from 1 to LEN inclusive.
 * @param vec  Array to store random permutation in
 * @param len  Length of the random permutation
 */
static void rand_perm(int vec[], int len)
{
    /* Initialize vector */
    for (int i = 0; i < len; i++)
    {
        vec[i] = i + 1;
    }

    /* Randomly shuffle vector using Fisher-Yates */
    int j;
    int swap_temp;
    for (int i = 0; i < len - 2; i++)
    {
        j = i + (rand() % (len - i));
        swap_temp = vec[i];
        vec[i] = vec[j];
        vec[j] = swap_temp;
    }
}

/**
 * Generate a random vector VEC of length LEN with zero mean
 */
static void gen_random_hv(double vec[], int len)
{
    if (len % 2 != 0)
    {
        fprintf(stderr, "gen_random_hv: vector must be of even length\n");
        return;
    }
    int *randomIndices = malloc(len * sizeof(int));
    if (randomIndices == NULL)
    {
        fprintf(stderr, "gen_random_hv: failed to allocate memory\n");
        return;
    }
    rand_perm(randomIndices, len);
    for (int i = 0; i < len / 2; i++)
    {
        vec[randomIndices[i]] = 1;
    }
    for (int i = len / 2 + 1; i < len; i++)
    {
        vec[randomIndices[i]] = -1;
    }
    free(randomIndices);
}
