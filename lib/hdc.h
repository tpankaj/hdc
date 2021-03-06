 #pragma once

struct hdc_item_memories
{
    double** cim;
    int cim_length;
    double** im;
    int im_length;
};

struct hdc_trained_model
{
    struct hdc_item_memories* item_memories;
    double** am;
    int num_classes;
    int* num_pat;
};

struct hdc_accuracy
{
    double accuracy;
    double acc_exc_trnz;
};

struct hdc_trained_model* hdctrain(int* label_train_set, double** train_set,
                                   int num_classes, int D, int N, int maxl,
                                   double precision, double cutting_angle);

struct hdc_accuracy hdcpredict(struct hdc_trained_model* model,
                               int* label_test_set, double** test_set,
                               int test_set_len, int D, int N, double precision);

void hdcdeinit(struct hdc_trained_model* model);
