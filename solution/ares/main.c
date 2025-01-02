#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define IS_ROOT (world_rank == 0)

void save_time(double execution_time, int tasks) {
    char filename[16];
    sprintf(filename, "t%d.txt", tasks);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    
    fprintf(file, "%f\n", execution_time);
    fclose(file);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    MPI_Barrier(MPI_COMM_WORLD);
    double timer_start = MPI_Wtime();
    
    /* Plane 1x1, T=6.5 */
    const double dx = 1. / SIDE_LENGTH;
    const double dy = 1. / SIDE_LENGTH;
    const double dt = 6.5 / ITERATIONS;
    
    const int row_height = (SIDE_LENGTH - 2) / world_size;
    int begin_row_idx = 1;
    int end_row_idx = begin_row_idx + row_height;
    if (IS_ROOT)
    {
        end_row_idx += (SIDE_LENGTH - 2) % world_size;
    }
    
    int total_points;
    double* membrane;
    if (IS_ROOT)
    {
        total_points = SIDE_LENGTH * SIDE_LENGTH;
        membrane = (double*)malloc(sizeof(double) * total_points);
        /*** INITIAL CONDITION ***/
        const double R = 100.;
        for (int row = 1; row < SIDE_LENGTH - 1; row++)
        {
            const int point_row_idx = SIDE_LENGTH * row;
            for (int col = 1; col < SIDE_LENGTH - 1; col++)
            {
                membrane[point_row_idx + col] = 1. / (1. + exp(0.5 * R * (dy * (double)(row - 1) + dx * (double)(col - 1))));
            }
        }
        /*** INITIAL CONDITION ***/
        
        /*** SET GRADIENT ON EDGES TO 0 ***/
        for (int row = 1; row < SIDE_LENGTH - 1; row++)
        {
            const int point_row_idx = SIDE_LENGTH * row;
            membrane[point_row_idx] = membrane[point_row_idx + 1];
            membrane[point_row_idx + SIDE_LENGTH - 1] = membrane[point_row_idx + SIDE_LENGTH - 2];
        }
        for (int col = 0; col < SIDE_LENGTH; col++)
        {
            membrane[col] = membrane[col + SIDE_LENGTH];
            membrane[(SIDE_LENGTH - 1) * SIDE_LENGTH + col] = membrane[(SIDE_LENGTH - 2) * SIDE_LENGTH + col];
        }
        /*** SET GRADIENT ON EDGES TO 0 ***/
        
        /*** SEND CORRECT PART OF PLANE TO OTHER NODES ***/
        const int data_size = SIDE_LENGTH * (row_height + 2);
        for (int i = 1; i < world_size; i++)
        {
            MPI_Send(membrane + SIDE_LENGTH * (end_row_idx - 1 + (i - 1) * row_height), data_size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD); 
        }
        /*** SEND CORRECT PART OF PLANE TO OTHER NODES ***/
    }
    else
    {
        total_points = SIDE_LENGTH * (row_height + 2);
        membrane = (double*)malloc(sizeof(double) * total_points);
        
        /*** RECEIVE INIT PART OF PLANE FROM ROOT NODE ***/
        MPI_Recv(membrane, total_points, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /*** RECEIVE INIT PART OF PLANE FROM ROOT NODE ***/
    }
    
    const int max_procs = 12; // Change if needed, could be put as '-D MAX_PROCS=12' during compilation
    if (world_size > max_procs)
    {
        printf("world_size > max_procs, ERROR!");
        return 1;
    }
    double max_membrane_storage[max_procs];
    for (int i = 0; i < max_procs; i++)
    {
        max_membrane_storage[i] = 0;
    }
    double alpha_x = 0.;
    double alpha_y = 0.;
    
    for (int itr = 0; itr < ITERATIONS; itr++)
    {
        /*** SYNC MAX VALUE FROM WHOLE PLANE ***/
        double max_value = 0;
        for (int i = 0; i < total_points; i++)
        {
            const double membrane_fabs = fabs(membrane[i]);
            if (membrane_fabs > max_value)
            {
                max_value = membrane_fabs;
            }
        }
        
        if (world_size > 1)
        {
            if (IS_ROOT)
            {
                MPI_Gather(&max_value, 1, MPI_DOUBLE, max_membrane_storage, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Gather(&max_value, 1, MPI_DOUBLE, 0, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            }
            
            double all_max_value = 0;
            if (IS_ROOT)
            {
                for (int i = 0; i < world_size; i++)
                {
                    if (max_membrane_storage[i] > all_max_value)
                    {
                        all_max_value = max_membrane_storage[i];
                    }
                }
                for (int i = 1; i < world_size; i++)
                {
                    MPI_Send(&all_max_value, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
                }
            }
            else
            {
                MPI_Recv(&all_max_value, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            alpha_x = all_max_value;
            alpha_y = all_max_value;
        }
        else
        {
            alpha_x = max_value;
            alpha_y = max_value;
        }
        /*** SYNC MAX VALUE FROM WHOLE PLANE ***/
        
        const int offset = (row_height % 2) * (world_rank % 2);
        
        /*** CALCULATE BLACK SQUARES ***/
        for (int row = begin_row_idx; row < end_row_idx; row++)
        {
            const int point_row_idx = SIDE_LENGTH * row;
            for (int col = 2 - ((row + offset) % 2); col < SIDE_LENGTH - 1; col+=2)
            {
                const double center = membrane[point_row_idx + col];
                const double center_sqr = center * center;
                const double top = membrane[point_row_idx + col - SIDE_LENGTH];
                const double bot = membrane[point_row_idx + col + SIDE_LENGTH];
                const double left = membrane[point_row_idx + col - 1];
                const double right = membrane[point_row_idx + col + 1];
                
                const double Fx_p = 0.25 * (center_sqr + bot * bot) - 0.5 * alpha_x * (bot - center);
                const double Fx_m = 0.25 * (top * top + center_sqr) - 0.5 * alpha_x * (center - top);
                const double Gy_p = 0.25 * (center_sqr + right * right) - 0.5 * alpha_y * (right - center);
                const double Gy_m = 0.25 * (left * left + center_sqr) - 0.5 * alpha_y * (center - left);
                
                membrane[point_row_idx + col] = center - dt / dx * (Fx_p - Fx_m) - dt / dy * (Gy_p - Gy_m);
            }
        }
        /*** CALCULATE BLACK SQUARES ***/
        
        /*** SYNC ADJACENT ROWS ***/
        if (world_size > 1)
        {
            if (IS_ROOT)
            {
                MPI_Send(membrane + SIDE_LENGTH * (end_row_idx - 1), SIDE_LENGTH, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(membrane + SIDE_LENGTH * end_row_idx, total_points, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else if (world_rank < world_size - 1)
            {
                MPI_Recv(membrane, SIDE_LENGTH, MPI_DOUBLE, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(membrane + SIDE_LENGTH, SIDE_LENGTH, MPI_DOUBLE, world_rank - 1, 0, MPI_COMM_WORLD);
                MPI_Send(membrane + SIDE_LENGTH * row_height, SIDE_LENGTH, MPI_DOUBLE, world_rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(membrane + SIDE_LENGTH * (row_height + 1), SIDE_LENGTH, MPI_DOUBLE, world_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else
            {
                MPI_Recv(membrane, SIDE_LENGTH, MPI_DOUBLE, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(membrane + SIDE_LENGTH, SIDE_LENGTH, MPI_DOUBLE, world_rank - 1, 0, MPI_COMM_WORLD);
            }
        }
        /*** SYNC ADJACENT ROWS ***/
        
        /*** CALCULATE WHITE SQUARES ***/
        for (int row = begin_row_idx; row < end_row_idx; row++)
        {
            const int point_row_idx = SIDE_LENGTH * row;
            for (int col = 2 - ((row + 1 + offset) % 2); col < SIDE_LENGTH - 1; col+=2)
            {
                const double center = membrane[point_row_idx + col];
                const double center_sqr = center * center;
                const double top = membrane[point_row_idx + col - SIDE_LENGTH];
                const double bot = membrane[point_row_idx + col + SIDE_LENGTH];
                const double left = membrane[point_row_idx + col - 1];
                const double right = membrane[point_row_idx + col + 1];
                
                const double Fx_p = 0.25 * (center_sqr + bot * bot) - 0.5 * alpha_x * (bot - center);
                const double Fx_m = 0.25 * (top * top + center_sqr) - 0.5 * alpha_x * (center - top);
                const double Gy_p = 0.25 * (center_sqr + right * right) - 0.5 * alpha_y * (right - center);
                const double Gy_m = 0.25 * (left * left + center_sqr) - 0.5 * alpha_y * (center - left);
                
                membrane[point_row_idx + col] = center - dt / dx * (Fx_p - Fx_m) - dt / dy * (Gy_p - Gy_m);
            }
        }
        /*** CALCULATE WHITE SQUARES ***/
        
        /*** SET GRADIENT ON EDGES TO 0 ***/
        if (IS_ROOT)
        {
            for (int col = 1; col < SIDE_LENGTH - 1; col++)
            {
                membrane[col] = membrane[col + SIDE_LENGTH];
            }
        }
        if (world_rank == world_size - 1)
        {
            for (int col = 1; col < SIDE_LENGTH - 1; col++)
            {
                membrane[total_points - SIDE_LENGTH + col] = membrane[total_points - 2 * SIDE_LENGTH + col];
            }
        }
        for (int row = 0; row <= end_row_idx; row++)
        {
            const int point_row_idx = SIDE_LENGTH * row;
            membrane[point_row_idx] = membrane[point_row_idx + 1];
            membrane[point_row_idx + SIDE_LENGTH - 1] = membrane[point_row_idx + SIDE_LENGTH - 2];
        }
        /*** SET GRADIENT ON EDGES TO 0 ***/
    }
    
    /*** GATHER RESULTS FROM ALL NODES TO ROOT ***/
    if (world_size > 1)
    {
        const int data_size = SIDE_LENGTH * row_height;
        double* root_membrane_target = membrane + SIDE_LENGTH * end_row_idx - data_size;
        if (IS_ROOT)
        {
            MPI_Gather(root_membrane_target, data_size, MPI_DOUBLE, root_membrane_target, data_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Gather(membrane + SIDE_LENGTH, data_size, MPI_DOUBLE, 0, 0, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
    }
    /*** GATHER RESULTS FROM ALL NODES TO ROOT ***/
    
    MPI_Barrier(MPI_COMM_WORLD);
    double timer_end = MPI_Wtime();

    /*** SAVE RESULTS (WITHOUT EDGES) AS CSV ***/
    if (IS_ROOT)
    {
        for (int row = 1; row < SIDE_LENGTH - 1; row++)
        {
            for (int col = 1; col < SIDE_LENGTH - 2; col++)
            {
                printf("%.5f,", membrane[SIDE_LENGTH * row + col]);
            }
            printf("%.5f\n", membrane[SIDE_LENGTH * row + SIDE_LENGTH - 1]);
        }

        double execution_time = timer_end - timer_start;
        save_time(execution_time, world_size);
    }
    /*** SAVE RESULTS (WITHOUT EDGES) AS CSV ***/
    
    free(membrane);
    MPI_Finalize();
    return 0;
}
