#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate an array of 2D arrays with random values
int*** generate_2d_arrays(int num_arrays, int rows, int cols) {
    // Allocate memory for the array of 2D arrays
    int*** array = (int***)malloc(num_arrays * sizeof(int**));
    if (array == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // Seed the random number generator
    srand(time(NULL));

    for (int i = 0; i < num_arrays; i++) {
        // Allocate memory for the 2D array
        array[i] = (int**)malloc(rows * sizeof(int*));
        if (array[i] == NULL) {
            printf("Memory allocation failed for array %d.\n", i);
            return NULL;
        }

        for (int j = 0; j < rows; j++) {
            array[i][j] = (int*)malloc(cols * sizeof(int));
            if (array[i][j] == NULL) {
                printf("Memory allocation failed for row %d of array %d.\n", j, i);
                return NULL;
            }

            // Fill the row with random values between 0 and 10
            for (int k = 0; k < cols; k++) {
                array[i][j][k] = rand() % 11; // Random values between 0 and 10
            }
        }
    }

    return array;
}

// Function to free the allocated memory
void free_2d_arrays(int*** array, int num_arrays, int rows) {
    for (int i = 0; i < num_arrays; i++) {
        for (int j = 0; j < rows; j++) {
            free(array[i][j]); // Free each row
        }
        free(array[i]); // Free each 2D array
    }
    free(array); // Free the main array
}

// Function to save the 2D arrays in JSON format to a file
void save_to_json(int*** array, int num_arrays, int rows, int cols, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to open file %s for writing.\n", filename);
        return;
    }

    fprintf(file, "[\n"); // Start of the JSON array
    for (int i = 0; i < num_arrays; i++) {
        fprintf(file, "  [\n"); // Start of a single 2D array
        for (int j = 0; j < rows; j++) {
            fprintf(file, "    [");
            for (int k = 0; k < cols; k++) {
                fprintf(file, "%d", array[i][j][k]);
                if (k < cols - 1) fprintf(file, ", "); // Add comma between elements
            }
            fprintf(file, "]");
            if (j < rows - 1) fprintf(file, ","); // Add comma between rows
            fprintf(file, "\n");
        }
        fprintf(file, "  ]");
        if (i < num_arrays - 1) fprintf(file, ","); // Add comma between 2D arrays
        fprintf(file, "\n");
    }
    fprintf(file, "]\n"); // End of the JSON array

    fclose(file);
    printf("Data saved to %s\n", filename);
}

int main() {
    // Parameters for the arrays
    int num_arrays = 10; // Number of 2D arrays
    int rows = 5;       // Number of rows in each 2D array
    int cols = 5;       // Number of columns in each 2D array

    // Generate the array of 2D arrays
    int*** arrays = generate_2d_arrays(num_arrays, rows, cols);
    if (arrays == NULL) {
        return 1; // Exit if memory allocation failed
    }

    // Save the generated arrays to a file
    save_to_json(arrays, num_arrays, rows, cols, "../input.json");

    // Free the allocated memory
    free_2d_arrays(arrays, num_arrays, rows);

    return 0;
}
