#include "hdc.h"
#include "klib/khash.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define NUM_EMG_CHANNELS 4
/*
 * to implement as static functions:
 * *computeNgram -> compute_ngram
 * *cosAngle -> cos_angle
 * *computeSumHV -> compute_sum_hv
 * *lookupItemMemory -> lookup_item_memory
 * *dotProduct -> dot_product
 * *entrywiseProduct -> entrywise_product
 * *entrywiseSum -> entrywise_sum
 * *circShift -> circ_shift
 * *norm -> norm
 * *initItemMemories -> init_item_memories
 * *randperm -> rand_perm
 * *genRandomHV -> gen_random_hv
 *
 * to implement as public functions:
 * MAYBE NOT NEEDED hdc_init - make sure to do srand, init_item_memories
 * hdc_train
 * hdc_predict
 * MAYBE NOT NEEDED hdc_deinit
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
 * Calculates the entrywise sum of OP1 and OP2, and places it in DEST.
 * @param dest  Destination vector
 * @param op1   First operand
 * @param op2   Second operand
 * @param len   Length of vectors
 */
static void entrywise_sum(double dest[], double op1[], double op2[], size_t len)
{
    for (int i = 0; i < len; i++)
    {
        dest[i] = op1[i] + op2[i];
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
 * Circularly shift VEC.
 * @param vec  Vector to shift
 * @param len  Length of vector
 */
static void circ_shift(double vec[], int len)
{
    double last = vec[len - 1];
    memcpy(vec, vec + 1, len - 1);
    vec[0] = last;
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
 * Generate a random vector VEC of length LEN with zero mean.
 * @param vec  Array to store random hypervector in
 * @param len  Length of the random hypervector
 */
static void gen_random_hv(double vec[], int len)
{
    if (len % 2 != 0)
    {
        fprintf(stderr, "gen_random_hv: vector must be of even length\n");
        return;
    }
    int *randomIndices = malloc(len * sizeof(int));
    if (!randomIndices)
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

/**
 * Initialize item memories of length LEN and max EMG amplitude MAXL.
 * @param len   Length off item memory hypervectors
 * @param maxl  Maximum amplitude of EMG signal
 * @return Initialized CiM and iM
 */
static struct hdc_item_memories* init_item_memories(int len, double maxl)
{
    srand(1); /* Seed random number generator for predictable output */

    /* Allocate space for item memories */
    struct hdc_item_memories* memories =
        malloc(sizeof(struct hdc_item_memories));
    if (!memories) goto mem_error;
    
    /* Initialize iM with 4 orthogonal hypervectors for the 4 channels */
    memories->im = malloc(NUM_EMG_CHANNELS * sizeof(double*));
    if (!memories->im) goto mem_error;
    memories->im_length = NUM_EMG_CHANNELS;
    for (int i = 0; i < NUM_EMG_CHANNELS; i++)
    {
        memories->im[i] = malloc(len * sizeof(double));
        if (!memories->im[i]) goto mem_error;
        gen_random_hv(memories->im[i], len);
    }

    /* Initialize CiM */
    memories->cim = malloc((maxl + 1) * sizeof(double*));
    if (!memories->cim) goto mem_error;
    memories->cim_length = maxl + 1;
    double* init_hv = malloc(len * sizeof(double*));
    if (!init_hv) goto mem_error;
    double* current_hv = init_hv;
    int* random_indices = malloc(len * sizeof(int*));
    if (!random_indices) goto mem_error;
    rand_perm(random_indices, len);
    for (int i = 0; i <= maxl; i++)
    {
        memories->cim[i] = malloc(len * sizeof(double*));
        memcpy(memories->cim[i], current_hv, len);
        int sp = (int)floor(len / 2 / maxl);
        int start_index = (i * sp) + 1;
        int end_index = ((i + 1) * sp) + 1;
        for (int j = start_index; j <= end_index; j++)
        {
            current_hv[j] *= -1;
        }
    }

    return memories;
    
mem_error:
    fprintf(stderr, "init_item_memories: failed to allocate memory\n");
    return NULL;
}

/**
 * Recalls a vector from item memory based on inputs.
 * @param item_memory  item memory
 * @param im_length    length of item memory
 * @param raw_key      the input key
 * @param len          length of hypervectors
 * @param precision    precision used in quantization of input EMG signals
 * @return Pointer to copy of recalled vector (heap-allocated)
 */
static double* lookup_item_memory(double** item_memory, int im_length, int raw_key,
                               int len, double precision)
{
    int key = (int)((double)raw_key * precision);
    if (key < im_length)
    {
        double* random_hv = malloc(im_length * sizeof(double));
        if (!random_hv) goto mem_error;
        memcpy(random_hv, item_memory[key], len);
        return random_hv;
    }
    else
    {
        fprintf(stderr, "lookup_item_memory: cannot find key: %d\n", key);
        return NULL;
    }
mem_error:
    fprintf(stderr, "lookup_item_memory: failed to allocate memory\n");
    return NULL;
}

/**
 * Computes Ngrams.
 * @param buffer         data buffer
 * @param item_memories  continuous and discrete item memories
 * @param len            length of hypervectors
 * @param n              length of data buffer
 * @param precision      precision used in quantization of input EMG signals
 * @return Pointer to ngram (heap-allocated)
 */
static double* compute_ngram(double** buffer,
                             struct hdc_item_memories* item_memories, int len,
                             int n, double precision)
{
    double* record = calloc(len, sizeof(double));
    if (!record) goto mem_error;
    double* ngram = calloc(len, sizeof(double));
    if (!ngram) goto mem_error;

    double* ch_1_hv = lookup_item_memory(item_memories->cim,
                                         item_memories->cim_length,
                                         buffer[1][1], len, precision);
    double* ch_2_hv = lookup_item_memory(item_memories->cim,
                                         item_memories->cim_length,
                                         buffer[1][2], len, precision);
    double* ch_3_hv = lookup_item_memory(item_memories->cim,
                                         item_memories->cim_length,
                                         buffer[1][3], len, precision);
    double* ch_4_hv = lookup_item_memory(item_memories->cim,
                                         item_memories->cim_length,
                                         buffer[1][4], len, precision);
    entrywise_product(ch_1_hv, ch_1_hv, item_memories->im[1], len);
    entrywise_product(ch_2_hv, ch_2_hv, item_memories->im[2], len);
    entrywise_product(ch_3_hv, ch_3_hv, item_memories->im[3], len);
    entrywise_product(ch_4_hv, ch_4_hv, item_memories->im[4], len);
    entrywise_sum(ngram, ngram, ch_1_hv, len);
    entrywise_sum(ngram, ngram, ch_2_hv, len);
    entrywise_sum(ngram, ngram, ch_3_hv, len);
    entrywise_sum(ngram, ngram, ch_4_hv, len);
    free(ch_1_hv);
    free(ch_2_hv);
    free(ch_3_hv);
    free(ch_4_hv);

    for (int i = 2; i <= n; i++)
    {
        ch_1_hv = lookup_item_memory(item_memories->cim,
                                     item_memories->cim_length,
                                     buffer[i][1], len, precision);
        ch_2_hv = lookup_item_memory(item_memories->cim,
                                     item_memories->cim_length,
                                     buffer[i][2], len, precision);
        ch_3_hv = lookup_item_memory(item_memories->cim,
                                     item_memories->cim_length,
                                     buffer[i][3], len, precision);
        ch_4_hv = lookup_item_memory(item_memories->cim,
                                     item_memories->cim_length,
                                     buffer[i][4], len, precision);
        entrywise_product(ch_1_hv, ch_1_hv, item_memories->im[1], len);
        entrywise_product(ch_2_hv, ch_2_hv, item_memories->im[2], len);
        entrywise_product(ch_3_hv, ch_3_hv, item_memories->im[3], len);
        entrywise_product(ch_4_hv, ch_4_hv, item_memories->im[4], len);
        entrywise_sum(record, record, ch_1_hv, len);
        entrywise_sum(record, record, ch_2_hv, len);
        entrywise_sum(record, record, ch_3_hv, len);
        entrywise_sum(record, record, ch_4_hv, len);
        free(ch_1_hv);
        free(ch_2_hv);
        free(ch_3_hv);
        free(ch_4_hv);

        circ_shift(ngram, len);
        entrywise_product(ngram, ngram, record, len);
    }

    return ngram;
    
mem_error:
    fprintf(stderr, "compute_ngram: failed to allocate memory\n");
    return NULL;
}

/**
 * Computes hypervector sums.
 * @param buffer         data buffer
 * @param buffer_length  number of entries in data buffer
 * @param item_memories  continuous and discrete item memories
 * @param len            length of hypervectors
 * @param n              size of Ngram
 * @param precision      precision used in quantization of input EMG signals
 * @return Pointer to hypervector sum (heap-allocated)
 */
static double* compute_sum_hv(double** buffer, int buffer_length,
                              struct hdc_item_memories* item_memories,
                              int len, int n, double precision)
{
    double* sum_hv = calloc(len, sizeof(double));
    if (!sum_hv) goto mem_error;

    for (int i = 1; i <= buffer_length - n + 1; i++)
    {
        double* new_ngram = compute_ngram(buffer + i, item_memories, len, n,
                                          precision);
        entrywise_sum(sum_hv, sum_hv, new_ngram, len);
        free(new_ngram);
    }

    return sum_hv;
    
mem_error:
    fprintf(stderr, "compute_sum_hv: failed to allocate memory\n");
    return NULL;
}

/**
 * Trains hyperdimensional computing model.
 * @param label_train_set  Training set labels
 * @param train_set        Training set data
 * @param num_classes      Number of classes
 * @param D                Dimension of hypervectors
 * @param N                Size of Ngram
 * @param maxl             Maximum amplitude of EMG signal
 * @param precision        Precision used in quantization of input EMG signals
 * @param cutting_angle    Threshold angle for not including a vector
 * @return Trained hyperdimensional computing model
 */
struct hdc_trained_model* hdctrain(int* label_train_set, double** train_set, int num_classes, int D, int N, int maxl, double precision, double cutting_angle)
{
    /* Initialize trained model */
    struct hdc_trained_model* model = malloc(sizeof(struct hdc_trained_model));
    if (!model) goto mem_error;
    model->item_memories = init_item_memories(D, maxl);
    model->num_pat = calloc(num_classes, sizeof(int));
    if (!model->num_pat) goto mem_error;
    model->am = malloc(num_classes * sizeof(double*));
    if (!model->am) goto mem_error;
    for (int i = 0; i < num_classes; i++)
    {
        model->am[i] = calloc(D, sizeof(double));
    }

    /* Train model */
    int i = 1;
    while (i < num_classes - N + 1)
    {
        if (label_train_set[i] == label_train_set[i + N - 1])
        {
            double* ngram = compute_ngram(train_set + i, model->item_memories, D, N, precision);
            double angle = cos_angle(ngram, model->am[label_train_set[i + N - 1]], D
                );
            if (angle < cutting_angle)
            {
                entrywise_sum(model->am[label_train_set[i + N - 1]], model->am[label_train_set[i + N - 1]], ngram, D);
                model->num_pat[label_train_set[i + N - 1]]++;
            }
            i++;
        }
        else
        {
            i += N - 1;
        }
    }

    return model;

mem_error:
    fprintf(stderr, "hdctrain: failed to allocate memory\n");
    return NULL;
}
