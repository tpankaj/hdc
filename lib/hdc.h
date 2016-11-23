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
    int* num_pat;
};
