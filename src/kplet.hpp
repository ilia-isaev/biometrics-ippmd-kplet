#pragma once


#include <stdint.h>


#define MAX_RAY_AMOUNT             8
//#define MAX_STAR_MINUTIAE_AMOUNT 50
#define MAX_STAR_MINUTIAE_AMOUNT 128


typedef struct _ray_element
{
    int32_t neighbour_minutiae;
    int32_t rel_dist;
    int32_t rel_angle;
    int32_t rel_theta;
} RAY_ELEMENT, *P_RAY_ELEMENT;

typedef struct _minutiae_element
{
    uint32_t    ray_amount;
    RAY_ELEMENT rays[MAX_RAY_AMOUNT];
} MINUTIA_ELEMENT, *P_MINUTIA_ELEMENT;

typedef struct _fp_feature_vector
{
    uint32_t        minutiae_amount;
    //MINUTIA_ELEMENT minutiae_data[MAX_STAR_MINUTIAE_AMOUNT];
    P_MINUTIA_ELEMENT minutiae_data;
} FP_FEATURE_VECTOR, *P_FP_FEATURE_VECTOR;


double
PerformMatching(/*P_MATCHER_HANDLE pHandle, */P_FP_FEATURE_VECTOR pLeftFV, P_FP_FEATURE_VECTOR pRightFV);

