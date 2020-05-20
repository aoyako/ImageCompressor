// #pragma OPENCL EXTENSION cl_amd_printf : enable
// #pragma OPENCL EXTENSION cl_intel_printf : enable

void kernel verticalSeam(const global float *input,
                int rows,
                int cols,
                int default_cols,
                 local float *current_cost,
                 local float *previous_cost,
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
           local float *temp = previous_cost;
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

void kernel horisontalSeam(const global float *input,
                int rows,
                int cols,
                int default_cols,
                 local float *current_cost,
                 local float *previous_cost,
                 global int *directions,
                 global size_t *seam_points
                )
{
       int id = get_local_id(0);
       int threads = get_local_size(0);
       int thread_rows = ceil((float) rows / threads);
       int offset = id * thread_rows;

       /// Initialise previous_cost for first row
       for (int i = 0; i < thread_rows; ++i) {
           int current_row = offset + i;
           if (current_row < rows) {
               previous_cost[current_row] = input[current_row * default_cols];
           }
       }
       barrier(CLK_LOCAL_MEM_FENCE);

       for (int col = 1; col < cols; ++col) {
           for (int i = 0; i < thread_rows; ++i) {
               int current_row = offset + i;
               if (current_row < rows) {

                   /// Left minimum
                   if ((current_row > 0) && (previous_cost[current_row-1] < previous_cost[current_row]) &&
                       ((current_row == rows-1) || (previous_cost[current_row-1] < previous_cost[current_row+1]))) {
                       directions[current_row*default_cols + col] = -1;
                       current_cost[current_row] = previous_cost[current_row-1] + input[current_row*default_cols + col];
                   /// Right minimum
                   } else if ((current_row < rows-1) && (previous_cost[current_row+1] < previous_cost[current_row]) &&
                       ((current_row == 0) || (previous_cost[current_row+1] < previous_cost[current_row-1]))) {
                       directions[current_row*default_cols + col] = 1;
                       current_cost[current_row] = previous_cost[current_row+1] + input[current_row*default_cols + col];
                   /// Mid minimum
                   } else {
                       directions[current_row*default_cols + col] = 0;
                       current_cost[current_row] = previous_cost[current_row] + input[current_row*default_cols + col];
                   }
               }
           }
           local float *temp = previous_cost;
           previous_cost = current_cost;
           current_cost = temp;
           barrier(CLK_LOCAL_MEM_FENCE);
       }

       /// Find minimum cost seam
       if (id == 0) {
           int seam_row = 0;
           for (int i = 1; i < rows; ++i) {
               if (previous_cost[i] < previous_cost[seam_row]) {
                   seam_row = i;
               }
           }
           int index = 0;
           for (int col = cols-1; col >= 1; --col) {
                seam_points[index] = seam_row;
                seam_row = seam_row + directions[seam_row*default_cols + col];
                index++;
           }
           seam_points[index] = seam_row;
       }
}


void kernel removeVerticalSeam(global float *mask,
                global float *image,
                int rows,
                int cols,
                int default_cols,
                const global size_t *seam_points
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

void kernel removeHorisontalSeam(global float *mask,
                global float *image,
                int rows,
                int cols,
                int default_cols,
                const global size_t *seam_points
                )
{
    int id = get_local_id(0);
    int thread_rows = ceil((float)cols / get_local_size(0));
    int offset = id * thread_rows;

    for(int col = 0; col < cols; ++col) {
        size_t last_pixel_image = image[(thread_rows + offset)*default_cols + col];
        size_t last_pixel_mask = mask[(thread_rows + offset)*default_cols + col];
        barrier(CLK_LOCAL_MEM_FENCE);

        for(int i = 0; i < thread_rows - 1; ++i) {
            int row = offset + i;
            if ((row < rows) && (row >= seam_points[cols-col-1])) {
                image[row*default_cols + col] = image[(row+1)*default_cols + col];
                mask[row*default_cols + col] = mask[(row+1)*default_cols + col];
            }
        }

        int last_index = offset + thread_rows - 1;
        if ((last_index < rows) && (last_index >= seam_points[cols-col-1])) {
            image[last_index*default_cols + col] = last_pixel_image;
            mask[last_index*default_cols + col] = last_pixel_mask;
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }
}

void kernel grayLevel(const global float *image,
                global float *result,
                int rows,
                int cols,
                int default_cols,
                local float *matrix
                )
{
    int g_id_x = get_global_id(0);
    int g_id_y = get_global_id(1);
    if ((g_id_x < cols) && (g_id_y < rows)) {
        size_t rgb_color = image[g_id_y*default_cols + g_id_x];
        float red = rgb_color / (256 * 256);
        float green = (rgb_color / 256) % 256;
        float blue = rgb_color % 256;
        float color = 0.33*red + 0.33*green + 0.34*blue;
        result[g_id_y*default_cols + g_id_x] = color;
}
}

void kernel sobelOperator(const global float *image,
                global float *result,
                int rows,
                int cols,
                int default_cols,
                local float *matrix
                )
{
    int g_id_x = get_global_id(0);
    int g_id_y = get_global_id(1);
    if ((g_id_x < cols) && (g_id_y < rows)) {
    int n = 3;
//    float Gaussian[3][3] = {
//           {1, 2, 1},
//           {2, 4, 2},
//           {1, 2, 1},
//    };
//    const int gaus_sum = 16;
//    float line = 0;
//    if ((g_id_x != 0) && (g_id_y != 0) && (g_id_x != cols-1) && (g_id_y != rows-1)) {
//        for (int x = -n/2; x < n/2; ++x) {
//            for (int y = -n/2; y < n/2; ++y) {
//                line += Gaussian[n/2+x][n/2+y] * temp[(g_id_y+y)*default_cols + g_id_x+x];
//            }
//        }
//    }
//    result[g_id_y*default_cols + g_id_x] = line / gaus_sum;
//    barrier(CLK_LOCAL_MEM_FENCE);

    const float SobelX[3][3] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1},
    };
    const float SobelY[3][3] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1},
    };

    if ((g_id_x != 0) && (g_id_y != 0) && (g_id_x != cols-1) && (g_id_y != rows-1)) {
        const float ImageClone[3][3] = {
            {image[(g_id_y+1)*default_cols + g_id_x-1], image[(g_id_y+1)*default_cols + g_id_x], image[(g_id_y+1)*default_cols + g_id_x+1]},
            {image[(g_id_y)*default_cols + g_id_x-1], image[(g_id_y)*default_cols + g_id_x], image[(g_id_y)*default_cols + g_id_x+1]},
            {image[(g_id_y-1)*default_cols + g_id_x-1], image[(g_id_y-1)*default_cols + g_id_x], image[(g_id_y-1)*default_cols + g_id_x+1]},
        };
        float gx = 0;
        float gy = 0;
        for (int x = -n/2; x <= n/2; ++x) {
            for (int y = -n/2; y <= n/2; ++y) {
//                gy += image[(g_id_y+x)*default_cols + g_id_x+y] * SobelY[n/2+y][n/2+x];
//                gx += image[(g_id_y+x)*default_cols + g_id_x+y] * SobelX[n/2+y][n/2+x];
                gy += ImageClone[n/2+x][n/2+y] * SobelY[n/2+y][n/2+x];
                gx += ImageClone[n/2+x][n/2+y] * SobelX[n/2+y][n/2+x];
            }
        }
//        printf("%d\n", ((size_t) sqrt(gx*gx + gy*gy)));
        gy = 255*gy/1020;
        gx = 255*gx/1020;
        int temp = sqrt(gx*gx + gy*gy)/sqrt(2.0);
        result[g_id_y*default_cols + g_id_x] = temp;
    } else {
        result[g_id_y*default_cols + g_id_x] = 0;
    }
}
}
