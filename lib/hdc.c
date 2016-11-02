#include "hdc.h"
#include "klib/khash.h"
#include <stdio.h>

/*
 * to implement as static functions:
 * computeNgram -> compute_ngram
 * cosAngle -> cos_angle
 * computeSumHV -> compute_sum_hv
 * lookupItemMemory -> lookup_item_memory
 * dotProduct -> dot_product
 * entrywiseProduct -> entrywise_product
 * circShift -> circ_shift
 * norm -> norm
*/

/**
 * Calculates dot product of OP1 and OP2.
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
