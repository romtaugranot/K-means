//
// Created by Yoav Shkedy on 03/05/2023.
//

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

static int N = 0;
static int d = 0;
static int K = 0;
static int iter = 0;

// Structs definitions
struct entry {
    double value;
    struct entry *next;
};

struct vector {
    struct vector *next;
    struct entry *entries;
};

// Functions declarations
struct vector* read_vectors(FILE *file_ptr);
double dist(struct vector u, struct vector v);
struct vector* k_means(struct vector *vectors);

// Code
int main(int argc, char *argv[]) {

    K = atoi(argv[1]);
    iter = atoi(argv[2]);
    char *file_name = argv[3];

    // Read vectors
    struct vector *vectors = read_vectors(fopen(file_name, "r"));

    // Check requirements

    // Run K-means algorithm
    struct vector *centroids = k_means(vectors);

    // Print centroids

    return 0;
}

struct vector* read_vectors(FILE *file_ptr){

    struct vector *head_vec, *curr_vec;
    struct entry *head_entry, *curr_entry;
    double n;
    char c;

    head_entry = malloc(sizeof(struct entry));
    curr_entry = head_entry;
    curr_entry->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;

    while (scanf("%lf%c", &n, &c) == 2) {

        // We have read all the entries for the current vector
        if (c == '\n') {
            curr_entry->value = n;
            curr_vec->entries = head_entry;
            curr_vec->next = malloc(sizeof(struct vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_entry = malloc(sizeof(struct entry));
            curr_entry = head_entry;
            curr_entry->next = NULL;

            // Count the number of vectors N
            N++;
            continue;
        }

        // Read the next entry of the current vector
        curr_entry->value = n;
        curr_entry->next = malloc(sizeof(struct entry));
        curr_entry = curr_entry->next;
        curr_entry->next = NULL;

        // Count the dimension d
        if (N == 0)
            d++;
    }

    free(curr_vec->next);
    free(head_entry);

    return head_vec;
}

double dist(struct vector u, struct vector v) {

    struct entry *u_entry = u.entries;
    struct entry *v_entry = v.entries;
    double sum = 0;

    for (int i = 0; i < d; i++) {
        sum += pow(u_entry->value - v_entry->value, 2);
        u_entry = u_entry->next;
        v_entry = v_entry->next;
    }

    return sqrt(sum);
}

struct vector* k_means(struct vector *vectors) {

    double eps = 0.001;
    
    // Initialize centroids as first k vectors
    struct vector *centroids = malloc(N);
    for (int i = 0; i < N; i++)
        centroids[i] = vectors[i];

    int iteration_number = 0;
    // Repeat until convergence of centroids or until iteration_number == iter
    while (1) {

        // Assign every x_i to the closest cluster

        // Update the centroids

        // Check conditions
        if (iteration_number == iter)
            break;

        iteration_number++;
    }

    return centroids;
}
