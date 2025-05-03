#include "kplet.hpp"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include <vector>
#include <algorithm>

//P_FP_FEATURE_VECTOR p_feature_vector =
//	(P_FP_FEATURE_VECTOR)calloc(sizeof(FP_FEATURE_VECTOR) +
//		(minutiae_amount-1) * sizeof(MINUTIA_ELEMENT), sizeof(uint8_t));


//#define MAX_STAR_MINUTIAE_AMOUNT    50
#define LCS_MAX_RAY_SIZE            (MAX_RAY_AMOUNT+1)
#define LCS_MAX_RAY_AMOUNT          (LCS_MAX_RAY_SIZE*LCS_MAX_RAY_SIZE)

int32_t LeftColorArray [MAX_STAR_MINUTIAE_AMOUNT];
int32_t RightColorArray[MAX_STAR_MINUTIAE_AMOUNT];
int32_t LeftStack [MAX_STAR_MINUTIAE_AMOUNT];
int32_t RightStack[MAX_STAR_MINUTIAE_AMOUNT];

int32_t CostArray[LCS_MAX_RAY_AMOUNT];
int32_t DirArray [LCS_MAX_RAY_AMOUNT];


// matching parameters
//#define MIN_MINUTIAE_PAIR_THR 12
#define MIN_MINUTIAE_PAIR_THR 4

#define TRUE_WEIGHT       16
#define FALSE_WEIGHT     -16
   
#define STAR_DIST_THR    12
#define STAR_ALPHA_THR   20
#define STAR_THETA_THR   30

#define STAR_DIST_COEFF    2
#define STAR_ALPHA_COEFF   4
#define STAR_THETA_COEFF   6

// constants of DFS algorithm
#define WHITE_COLOR      0
#define GRAY_COLOR       1
#define BLACK_COLOR      2

#define STACK_EMPTY      (-1)

// for LCS algorithm
#define LEFT_TOP_DIR     0
#define LEFT_DIR         1
#define UP_DIR           2

// for search space dimension
#define SEARCH_SPACE_DIM    15

double
PerformMatching(/*P_MATCHER_HANDLE pHandle, */P_FP_FEATURE_VECTOR pLeftFV, P_FP_FEATURE_VECTOR pRightFV)
{
    int top_of_stack;
    unsigned int index1, index2;
    unsigned int index3, index4;
    int max_score, cur_score;
    int cur_template_root_minutiae,
        cur_input_root_minutiae;
    P_MINUTIA_ELEMENT pTemplateMinutiae = NULL;
    P_MINUTIA_ELEMENT pInputMinutiae    = NULL;

    // LCS algorithm variables
    int left_cost, up_cost, left_up_cost;
    int delta_dist;
    int delta_alpha;
    int delta_theta;

    // full search
    max_score = 0;

	// print
	//std::vector<std::pair<int,std::pair<int,int>>> acc_map;

    memset(CostArray, 0, LCS_MAX_RAY_AMOUNT*sizeof(int32_t));

    for (index1 = 0; index1 < (pLeftFV->minutiae_amount < SEARCH_SPACE_DIM ?
	                           pLeftFV->minutiae_amount : SEARCH_SPACE_DIM); ++index1)
    {
        for (index2 = 0; index2 < (pRightFV->minutiae_amount < SEARCH_SPACE_DIM ?
		                           pRightFV->minutiae_amount : SEARCH_SPACE_DIM); ++index2)
        {
            // DFS algorithm
            memset((void*)LeftColorArray,  WHITE_COLOR, MAX_STAR_MINUTIAE_AMOUNT*sizeof(int32_t));
            memset((void*)RightColorArray, WHITE_COLOR, MAX_STAR_MINUTIAE_AMOUNT*sizeof(int32_t));

            LeftColorArray [index1] = GRAY_COLOR;
            RightColorArray[index2] = GRAY_COLOR;

            top_of_stack = 0;

            LeftStack [top_of_stack] = index1;
            RightStack[top_of_stack] = index2;

            cur_score = 1;

            while (STACK_EMPTY < top_of_stack)
            {
                cur_template_root_minutiae = LeftStack [top_of_stack];
                cur_input_root_minutiae    = RightStack[top_of_stack];
                --top_of_stack;

                // LCS algorithm
                pTemplateMinutiae = &pLeftFV ->minutiae_data[cur_template_root_minutiae];
                pInputMinutiae    = &pRightFV->minutiae_data[cur_input_root_minutiae];

                // LCS-LENGTH
                for (index3 = 1; index3 < pTemplateMinutiae ->ray_amount + 1; ++index3)
                {
                    for (index4 = 1; index4 < pInputMinutiae ->ray_amount + 1; ++index4)
                    {
                        up_cost   = CostArray[LCS_MAX_RAY_SIZE*(index3 - 1) + index4];
                        left_cost = CostArray[LCS_MAX_RAY_SIZE*index3 + index4 - 1];

                        if ( LeftColorArray [pTemplateMinutiae->rays[index3 - 1].neighbour_minutiae] != WHITE_COLOR ||
                             RightColorArray[pInputMinutiae   ->rays[index4 - 1].neighbour_minutiae] != WHITE_COLOR )
                            left_up_cost = CostArray[LCS_MAX_RAY_SIZE*(index3 - 1) + index4 - 1] + FALSE_WEIGHT;
                        else
                        {
                            delta_dist  = abs(pTemplateMinutiae->rays[index3 - 1].rel_dist   -
								pInputMinutiae->rays[index4 - 1].rel_dist);

                            delta_alpha =  abs(pTemplateMinutiae->rays[index3 - 1].rel_angle -
								pInputMinutiae->rays[index4 - 1].rel_angle);

                            delta_alpha = delta_alpha > 180 ? 360 - delta_alpha : delta_alpha;

                            delta_theta = abs(pTemplateMinutiae->rays[index3 - 1].rel_theta  -
								pInputMinutiae->rays[index4 - 1].rel_theta );

                            delta_theta = delta_theta > 180 ? 360 - delta_theta : delta_theta;

                            if ( delta_dist  <= STAR_DIST_THR && delta_alpha <= STAR_ALPHA_THR &&
								 delta_theta <= STAR_THETA_THR )
                                left_up_cost = CostArray[LCS_MAX_RAY_SIZE*(index3 - 1) + index4 - 1] + TRUE_WEIGHT -
									delta_dist/STAR_DIST_COEFF -
									delta_alpha/STAR_ALPHA_COEFF - delta_theta/STAR_THETA_COEFF;
                            else
                                left_up_cost = CostArray[LCS_MAX_RAY_SIZE*(index3 - 1) + index4 - 1] + FALSE_WEIGHT;
                        }

                        if ( left_up_cost > up_cost && left_up_cost > left_cost )
                        {
                            CostArray[LCS_MAX_RAY_SIZE*index3 + index4] = left_up_cost;
                            DirArray [LCS_MAX_RAY_SIZE*index3 + index4] = LEFT_TOP_DIR;
                        }
                        else
                        {
                            if ( up_cost > left_cost && up_cost > left_up_cost )
                            {
                                CostArray[LCS_MAX_RAY_SIZE*index3 + index4] = up_cost;
                                DirArray [LCS_MAX_RAY_SIZE*index3 + index4] = UP_DIR;
                            }
                            else
                            {
                                CostArray[LCS_MAX_RAY_SIZE*index3 + index4] = left_cost;
                                DirArray [LCS_MAX_RAY_SIZE*index3 + index4] = LEFT_DIR;
                            }
                         }
                     }
                 }
 
                 // PRINT-LCS
                 --index3;
                 --index4;

                 while ( index3 > 0 && index4 > 0)
                 {
                     if ( LEFT_TOP_DIR == DirArray[LCS_MAX_RAY_SIZE*index3 + index4] )
                     {
                         // DFS algorithm part II
                         if (WHITE_COLOR == LeftColorArray [pTemplateMinutiae->rays[index3 - 1].neighbour_minutiae] &&
                             WHITE_COLOR == RightColorArray[pInputMinutiae   ->rays[index4 - 1].neighbour_minutiae] )
                         {
                             ++top_of_stack;
                             LeftStack [top_of_stack] = pTemplateMinutiae->rays[index3 - 1].neighbour_minutiae;
                             RightStack[top_of_stack] = pInputMinutiae   ->rays[index4 - 1].neighbour_minutiae;
                             ++cur_score;

							 /*auto map = std::make_pair(
								pTemplateMinutiae->rays[index3 - 1].neighbour_minutiae+1,
								pInputMinutiae   ->rays[index4 - 1].neighbour_minutiae+1);

							 auto has_map = [map](auto a) {return a.second == map;};

							 auto it = std::find_if(acc_map.begin(), acc_map.end(), has_map);

							 if (it == acc_map.end())
								 acc_map.push_back({1,map});
							 else
								 it->first++;
								 */
                         }

                         LeftColorArray [pTemplateMinutiae->rays[index3 - 1].neighbour_minutiae] = GRAY_COLOR;
                         RightColorArray[pInputMinutiae   ->rays[index4 - 1].neighbour_minutiae] = GRAY_COLOR;

                         --index3;
                         --index4;
                         continue;
                     }

                     if ( UP_DIR == DirArray[LCS_MAX_RAY_SIZE*index3 + index4] )
                     {
                         --index3;
                         continue;
                     }

                     if ( LEFT_DIR == DirArray[LCS_MAX_RAY_SIZE*index3 + index4] )
                     {
                         --index4;
                         continue;
                     }
                 }

                 // DFS algorithm part III
                 LeftColorArray [cur_template_root_minutiae] = BLACK_COLOR;
                 RightColorArray[cur_input_root_minutiae]    = BLACK_COLOR;
            }
            // end of DFS algorithm

            if ( cur_score > max_score )
                max_score = cur_score;
        }
     }

	 //std::sort(acc_map.begin(), acc_map.end(), [](auto &a, auto &b) {return a.first > b.first;});
	 //for (auto& cnt_map : acc_map)
	//	 fprintf(stdout, "%3i %3i|%3i\n", cnt_map.first, cnt_map.second.first, cnt_map.second.second);

     // calculate matching score
     if ( max_score > MIN_MINUTIAE_PAIR_THR )
         return (double)(max_score*max_score)/(pLeftFV->minutiae_amount*pRightFV->minutiae_amount);
     else
         return 0.0;
}

