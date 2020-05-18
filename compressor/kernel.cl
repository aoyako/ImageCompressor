// #pragma OPENCL EXTENSION cl_amd_printf : enable
// #pragma OPENCL EXTENSION cl_intel_printf : enable

void kernel verticalSeam(global size_t *input,
                int rows,
                int cols,
                int default_cols,
                 local size_t *current_cost, 
                 local size_t *previous_cost,
                 global int *directions,
                 global size_t *seam_points
                )
{
       int id = get_local_id(0);
       int threads = get_local_size(0);
       int thread_cols = ceil((float) cols / threads);
       int offset = id * thread_cols;
        
       /// Initialise previous_cost for first row
       for (int i = 0; i < thread_cols; ++i) {
           int current_col = offset + i;
           if (current_col < cols) {
               previous_cost[current_col] = input[current_col];
           }
       }
       barrier(CLK_LOCAL_MEM_FENCE);
        
       for (int row = 1; row < rows; ++row) {
           for (int i = 0; i < thread_cols; ++i) {
               int current_col = offset + i;
               if (current_col < cols) {
                    
                   /// Left minimum
                   if ((current_col > 0) && (previous_cost[current_col-1] < previous_cost[current_col]) &&
                       ((current_col == cols-1) || (previous_cost[current_col-1] < previous_cost[current_col+1]))) {
                       directions[row*default_cols + current_col] = -1;
                       current_cost[current_col] = previous_cost[current_col-1] + input[row*default_cols + current_col];
                   /// Right minimum
                   } else if ((current_col < cols-1) && (previous_cost[current_col+1] < previous_cost[current_col]) &&
                       ((current_col == 0) || (previous_cost[current_col+1] < previous_cost[current_col-1]))) {
                       directions[row*default_cols + current_col] = 1;
                       current_cost[current_col] = previous_cost[current_col+1] + input[row*default_cols + current_col];
                   /// Mid minimum
                   } else {
                       directions[row*default_cols + current_col] = 0;
                       current_cost[current_col] = previous_cost[current_col] + input[row*default_cols + current_col];
                   }
               }
           }
           local size_t *temp = previous_cost;
           previous_cost = current_cost;
           current_cost = temp;
           barrier(CLK_LOCAL_MEM_FENCE);
       }
        
       /// Find minimum cost seam
       if (id == 0) {
           int seam_col = 0;
           for (int i = 1; i < cols; ++i) {
               if (previous_cost[i] < previous_cost[seam_col]) {
                   seam_col = i;
               }
           }
           int index = 0;
           for (int row = rows-1; row >= 1; --row) {
                seam_points[index] = seam_col;
                seam_col = seam_col + directions[row*default_cols + seam_col];
                index++;
           }
           seam_points[index] = seam_col;
       }
}


void kernel removeVerticalSeam(global size_t *mask,
                global size_t *image,
                int rows,
                int cols,
                int default_cols,
                 global size_t *seam_points
                )
{
    int id = get_local_id(0);
    int thread_cols = ceil((float)cols / get_local_size(0));
    int offset = id * thread_cols;
    
    for(int row = 0; row < rows; ++row) {
        size_t last_pixel_image = image[row*default_cols + offset + thread_cols];
        size_t last_pixel_mask = mask[row*default_cols + offset + thread_cols];
        barrier(CLK_LOCAL_MEM_FENCE);
 
        for(int i = 0; i < thread_cols - 1; ++i) {
            int col = offset + i;
            if ((col < cols) && (col >= seam_points[rows-row-1])) {
                image[row*default_cols + col] = image[row*default_cols + col + 1];
                mask[row*default_cols + col] = mask[row*default_cols + col + 1];
            }
        }
 
        int last_index = offset + thread_cols - 1;
        if ((last_index < cols) && (last_index >= seam_points[rows-row-1])) {
            image[row*default_cols + last_index] = last_pixel_image;
            mask[row*default_cols + last_index] = last_pixel_mask;
        }
 
        barrier(CLK_LOCAL_MEM_FENCE);
    }
}
