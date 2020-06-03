#define BARRIER_VALUE 10e17
#define BARRIER_LIMIT 10e15

#define GET_RED(color) (((size_t) (color))/(256*256))
#define GET_GREEN(color) ((((size_t) (color))/256) % 256)
#define GET_BLUE(color) (((size_t) (color)) % 256)

/**
  * @brief Finds vertical seam with minimum cost
  * @param input Image of energy levels
  * @param rows Width of image
  * @param cols Height of image
  * @param default_cols Width of buffered image (power of two)
  * @param current_cost Array for storing seam costs of processed image row
  * @param previous_cost Array for storing seam costs of already calculated image row
  * @param directiosn Matrix is used to restore path from end to begining
  * @param seam_points Array of seam column indexes, from end to begining
  */
void kernel verticalSeamRight(const global float *input,
                int rows,
                int cols,
                int default_cols,
                local float *current_cost,
                local float *previous_cost,
                global int *directions,
                global size_t *seam_points,
                const global float *barriers
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
               previous_cost[current_col] = input[current_col]+barriers[current_col];
           }
       }
       barrier(CLK_LOCAL_MEM_FENCE);

       for (int row = 1; row < rows; ++row) {
           for (int i = 0; i < thread_cols; ++i) {
               int current_col = offset + i;
               if (current_col < cols) {
                    float gradientL = 3*(input[row*default_cols + current_col-1] - input[row*default_cols + current_col+1])
                                        *(input[row*default_cols + current_col-1] - input[row*default_cols + current_col+1])
                                        + 3*(input[(row-1)*default_cols + current_col] - input[row*default_cols + current_col-1])
                                        *(input[(row-1)*default_cols + current_col] - input[row*default_cols + current_col-1]);
                                        
                    float gradientM = 3*(input[row*default_cols + current_col-1] - input[row*default_cols + current_col+1])
                                        *(input[row*default_cols + current_col-1] - input[row*default_cols + current_col+1]);
                                        
                    float gradienR = 3*(input[row*default_cols + current_col-1] - input[row*default_cols + current_col+1])
                                        *(input[row*default_cols + current_col-1] - input[row*default_cols + current_col+1])
                                        + 3*(input[(row-1)*default_cols + current_col] - input[row*default_cols + current_col+1])
                                        *(input[(row-1)*default_cols + current_col] - input[row*default_cols + current_col+1]);
                                        
                    int barrier_left = barriers[row*default_cols + current_col - 1];
                    int barrier_right = barriers[row*default_cols + current_col + 1];
                    int barrier_mid = barriers[row*default_cols + current_col];
                    /// Left minimum
                    if ((current_col > 0) && ((previous_cost[current_col-1]+gradientL) < (previous_cost[current_col]+gradientM)) &&
                        ((current_col == cols-1) || ((previous_cost[current_col-1]+gradientL) < (previous_cost[current_col+1]+gradienR)))) {
                        directions[row*default_cols + current_col] = -1;
                        current_cost[current_col] = previous_cost[current_col-1] + barrier_left + (input[row*default_cols + current_col] + gradientL);
                    /// Right minimum
                    } else if ((current_col < cols-1) && ((previous_cost[current_col+1]+gradienR) < (previous_cost[current_col]+gradientM)) &&
                        ((current_col == 0) || ((previous_cost[current_col+1]+gradienR) < (previous_cost[current_col-1]+gradientL)))) {
                        directions[row*default_cols + current_col] = 1;
                        current_cost[current_col] = previous_cost[current_col+1] + barrier_right + (input[row*default_cols + current_col] + gradienR);
                    /// Mid minimum
                    } else {
                        directions[row*default_cols + current_col] = 0;
                        current_cost[current_col] = previous_cost[current_col] + barrier_mid + (input[row*default_cols + current_col] + gradientM);
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

/**
  * @brief Finds vertical seam with minimum cost
  * @param input Image of energy levels
  * @param rows Width of image
  * @param cols Height of image
  * @param default_cols Width of buffered image (power of two)
  * @param current_cost Array for storing seam costs of processed image row
  * @param previous_cost Array for storing seam costs of already calculated image row
  * @param directiosn Matrix is used to restore path from end to begining
  * @param seam_points Array of seam column indexes, from end to begining
  */
void kernel verticalSeam(const global float *input,
                int rows,
                int cols,
                int default_cols,
                local float *current_cost,
                local float *previous_cost,
                global int *directions,
                global size_t *seam_points,
                const global float *barriers
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
                   if ((previous_cost[current_col-1] < BARRIER_LIMIT) && (current_col > 0) && (previous_cost[current_col-1] < previous_cost[current_col]) &&
                       ((current_col == cols-1) || (previous_cost[current_col-1] < previous_cost[current_col+1]))) {
                       directions[row*default_cols + current_col] = -1;
                       current_cost[current_col] = previous_cost[current_col-1] + input[row*default_cols + current_col];
                   /// Right minimum
                   } else if ((previous_cost[current_col+1] < BARRIER_LIMIT) && (current_col < cols-1) && (previous_cost[current_col+1] < previous_cost[current_col]) &&
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


/**
  * @brief Removes vertical seam from images
  * @param mask First image
  * @param image Second image
  * @param rows Width of image
  * @param cols Height of image
  * @param default_cols Width of buffered image (power of two)
  * @param seam_points Array of seam column indexes, from end to begining
  */
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

/**
  * @brief Rotates image
  * @param image input image
  * @param result rotated image
  * @param rows Width of image
  * @param cols Height of image
  * @param default_cols Width of buffered image (power of two)
  * @param direction 0 for clockwise, 1 for counterclockwise
  */
void kernel rotateImage(const global float *image,
                   global float *result,
                   int rows,
                   int cols,
                   int default_cols,
                   int direction
                   )
{
    int g_id_x = get_global_id(0);
    int g_id_y = get_global_id(1);
    if (direction == 1) {
        result[rows-1-g_id_y + (g_id_x)*default_cols] = image[g_id_x + g_id_y*default_cols];
    } else {
        result[g_id_y + (cols-1-g_id_x)*default_cols] = image[g_id_x + g_id_y*default_cols];
    }
}


/**
  * @brief Applies gray filter to image
  * @param image Processed image
  * @param result Gray colored image
  * @param rows Width of image
  * @param cols Height of image
  * @param default_cols Width of buffered image (power of two)
  */
void kernel grayLevel(const global float *image,
                global float *result,
                int rows,
                int cols,
                int default_cols
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

/**
  * @brief Applies Sobel operator to image
  * @param image Processed image
  * @param result Image with hightlighted borders
  * @param rows Width of image
  * @param cols Height of image
  * @param default_cols Width of buffered image (power of two)
  */
void kernel sobelOperator(const global float *image,
                global float *result,
                int rows,
                int cols,
                int default_cols
                )
{
    int g_id_x = get_global_id(0);
    int g_id_y = get_global_id(1);
    if ((g_id_x < cols) && (g_id_y < rows)) {
        int n = 3;

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
                    gy += ImageClone[n/2+x][n/2+y] * SobelY[n/2+y][n/2+x];
                    gx += ImageClone[n/2+x][n/2+y] * SobelX[n/2+y][n/2+x];
                }
            }

            gy = 255*gy/1020;
            gx = 255*gx/1020;
            int temp = sqrt(gx*gx + gy*gy)/sqrt(2.0);
            result[g_id_y*default_cols + g_id_x] = temp;
        } else {
            result[g_id_y*default_cols + g_id_x] = 1;
        }
    }
}

float averageColor(float m, float t, float r, float b, float l, float tr, float tl, float br, float bl) {
    size_t red = (10*GET_RED(m) + 3*GET_RED(t) + 3*GET_RED(b) + 3*GET_RED(r) + 3*GET_RED(l) + GET_RED(tr) + GET_RED(tl) + GET_RED(br) + GET_RED(bl))/26;
    size_t blue = (10*GET_BLUE(m) + 3*GET_BLUE(t) + 3*GET_BLUE(b) + 3*GET_BLUE(r) + 3*GET_BLUE(l) + GET_BLUE(tr) + GET_BLUE(tl) + GET_BLUE(br) + GET_BLUE(bl))/26;
    size_t green = (10*GET_GREEN(m) + 3*GET_GREEN(t) + 3*GET_GREEN(b) + 3*GET_GREEN(r) + 3*GET_GREEN(l) + GET_GREEN(tr) + GET_GREEN(tl) + GET_GREEN(br) + GET_GREEN(bl))/26;
    return red*256*256 + green*256 + blue;
//     return 255*256*256;
}

void kernel insertVerticalSeam(global float *image,
                       global float *mask,
                       int rows,
                       int cols,
                       int default_cols,
                       const global size_t *seam,
                       local float *seam_colors,
                       global float *barriers
                       )
{
    int id = get_local_id(0);

    /// Calculate colors
    if (id == 0) {
        seam_colors[0] = image[seam[0]];
        seam_colors[rows-1] = image[(rows-1)*default_cols + seam[rows-1]];
        for (int i = 1; i < rows-1; ++i) {
            seam_colors[i] = image[i*default_cols + seam[i]];
            if ((seam[i] > 0) && (seam[i] < cols-1)) {
                seam_colors[i] = averageColor(image[i*default_cols + seam[i]], image[i*default_cols + seam[i] + 1], image[i*default_cols + seam[i] - 1],
                                              image[(i+1)*default_cols + seam[i]], image[(i-1)*default_cols + seam[i]], 
                                              image[(i+1)*default_cols + seam[i] + 1], image[(i-1)*default_cols + seam[i] - 1],
                                              image[(i+1)*default_cols + seam[i] - 1], image[(i-1)*default_cols + seam[i] + 1]);
            }
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    int thread_rows = ceil((float)rows / get_local_size(0));
    int offset = id * thread_rows;

        for (int index = 0; index < thread_rows; ++index) {
            if (offset + index < rows) {
                for (int i = cols; i > seam[offset+index]; --i) {
                    image[(offset+index)*default_cols + i] = image[(offset+index)*default_cols + i - 1];
                    mask[(offset+index)*default_cols + i] = mask[(offset+index)*default_cols + i - 1];
                    barriers[(offset+index)*default_cols + i] = barriers[(offset+index)*default_cols + i - 1];
                }
                image[(offset+index)*default_cols + seam[offset+index]] = seam_colors[offset+index];
                mask[(offset+index)*default_cols + seam[offset+index]] = BARRIER_VALUE;
                mask[(offset+index)*default_cols + seam[offset+index]+1] = BARRIER_VALUE;
                barriers[(offset+index)*default_cols + seam[offset+index]] = 255;
                barriers[(offset+index)*default_cols + seam[offset+index] + 1] = 255;
//                 barriers[(offset+index)*default_cols + seam[offset+index] + 2] = 255*256;
//                 barriers[(offset+index)*default_cols + seam[offset+index] - 1] = 255*256;
            }
        }
}

void kernel resetBarrier(global float *barrier,
                        int default_cols
                        )
{
    int g_id_x = get_global_id(0);
    int g_id_y = get_global_id(1);
    barrier[g_id_y*default_cols + g_id_x] = 1;
}
