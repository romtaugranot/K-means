/*
 * Created by Yoav Shkedy on 03/05/2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <float.h>

static int N = 0;
static int d = 1; /* Dimension is at least 1. */
static int K = 0;
static int iter = 0;
static double eps = 0.001;

/* Structs definitions */
struct entry {
    double value;
    struct entry *next;
};

struct vector {
    struct vector *next;
    struct entry *entries;
};

/* Functions declarations */
struct vector* read_data_points();
int check_argument(int smallest, char arg[], int largest);
int is_number(char number[]);

double dist(struct vector u, struct vector v);
struct vector* sum_vectors(struct vector u, struct vector v);
struct vector* sum_vectors_in_cluster(struct vector *centroid, struct vector *cluster);
struct vector* divide_by_scalar(struct vector v, double scalar);
int count_vectors_in_cluster(struct vector *cluster);

int compute_flag_delta(struct vector *old_centroids, struct vector *new_centroids);
int arg_min_dist(struct vector data_point, struct vector *centroids);
struct vector** assign_data_points_to_clusters(struct vector *data_points, struct vector * centroids);
struct vector* k_means(struct vector *vectors);

/* Code */
int main(int argc, char *argv[]) {
    struct vector *vectors;
    int flag_K, flag_iter;

    /* Read vectors from file */
    vectors = read_data_points();

    /* Check validity of arguments */
    flag_K = check_argument(1, argv[1], N);

    if (argc == 3)
        flag_iter = check_argument(1, argv[2], 1000);

    /* Execute algorithm only if arguments are valid */
    if (flag_K == 1 && flag_iter == 1) {

        /* Get K and iter */
        K = atoi(argv[1]);
        iter = argc == 3 ? atoi(argv[2]) : 200;

        /* Execute K-means algorithm */
        struct vector *centroids = k_means(vectors);

        printf("K: %d, iter: %d, N: %d, d: %d\n", K, iter, N, d);
        /*
        printf("%f, ", vectors->entries->value);
        printf("%f, ", vectors->entries->next->value);
        printf("%f\n", vectors->entries->next->next->value);
         */

        return 0;
    }

    /* Otherwise raise error messages */
    else {

        if (flag_K == 0)
            printf("Invalid number of clusters!");

        if (flag_iter == 0)
            printf("Invalid maximum iteration!");

        return 1;
    }
}

/** Argument reading and processing **/

struct vector* read_data_points(){

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

        /* We have read all the entries for the current vector */
        if (c == '\n') {
            curr_entry->value = n;
            curr_vec->entries = head_entry;
            curr_vec->next = malloc(sizeof(struct vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_entry = malloc(sizeof(struct entry));
            curr_entry = head_entry;
            curr_entry->next = NULL;

            /* Count the number of vectors N */
            N++;
            continue;
        }

        /* Read the next entry of the current vector */
        curr_entry->value = n;
        curr_entry->next = malloc(sizeof(struct entry));
        curr_entry = curr_entry->next;
        curr_entry->next = NULL;

        /* Count the dimension d */
        if (N == 0)
            d++;
    }

    free(curr_vec->next);
    free(head_entry);

    return head_vec;
}

/* Returns 1 if and only if all requirements of the argument are met. */
int check_argument(int smallest, char arg[], int largest){
    int flag_is_num = is_number(arg);
    int num;

    if (flag_is_num == 1) {
        num = atoi(arg);
        if (num < smallest || largest < num)
            return 0;
        return 1;
    }
    return 0;
}

/* Returns 1 if and only if number is an integer. */
int is_number(char number[]) {
    int i = 0;

    /* Checking for negative numbers */
    if (number[0] == '-')
        i = 1;

    for (; number[i] != 0; i++) {
        /* If (number[i] > '9' || number[i] < '0') */
        if (!isdigit(number[i])) {
            return 0;
        }
    }
    return 1;
}

/** Math functions **/

double dist(struct vector u, struct vector v) {
    int i = 0;

    struct entry *u_entry = u.entries;
    struct entry *v_entry = v.entries;
    double sum = 0;

    for(; i < d; i++) {
        sum += pow(u_entry->value - v_entry->value, 2);
        u_entry = u_entry->next;
        v_entry = v_entry->next;
    }

    return sqrt(sum);
}

struct vector* sum_vectors(struct vector u, struct vector v) {
    int i = 0;

    struct vector* result_vector = malloc(sizeof(struct vector));
    result_vector->entries = malloc(d * sizeof(struct entry));

    struct entry* u_entry = u.entries;
    struct entry* v_entry = v.entries;
    struct entry* result_entry = result_vector->entries;

    for (; i < d; i++) {
        result_entry->value = u_entry->value + v_entry->value;
        u_entry = u_entry->next;
        v_entry = v_entry->next;
        result_entry = result_entry->next;
    }

    return result_vector;
}

struct vector* sum_vectors_in_cluster(struct vector *centroid, struct vector *cluster) {
    if (cluster == NULL)
        return centroid;

    struct vector *curr_vector = cluster;
    struct vector *next_vector = curr_vector->next;
    struct vector *result_vector;

    if (next_vector == NULL)
        return curr_vector;
    else {

        while (next_vector != NULL) {
             result_vector = sum_vectors(*curr_vector, *next_vector);
             curr_vector = result_vector;
             next_vector = next_vector->next;
        }

    }
    return result_vector;
}

struct vector* divide_by_scalar(struct vector v, double scalar) {
    int i = 0;

    struct vector* result_vector = malloc(sizeof(struct vector));
    result_vector->entries = malloc(d * sizeof(struct entry));

    struct entry* v_entry = v.entries;
    struct entry* result_entry = result_vector->entries;

    for (; i < d; i++) {
        result_entry->value = v_entry->value / scalar;
        v_entry = v_entry->next;
        result_entry = result_entry->next;
    }

    return result_vector;
}

int count_vectors_in_cluster(struct vector *cluster) {
    struct vector *curr_vector = cluster;
    int cnt = 0;

    while (curr_vector != NULL) {
        cnt++;
        curr_vector = curr_vector->next;
    }

    return cnt;
}

/** K-means algorithm **/


/*
 * pre-condition: length of old_centroids == length of new_centroids.
 * returns: 1 if and only if each delta is strictly less than eps.
*/
int compute_flag_delta(struct vector *old_centroids, struct vector *new_centroids) {
    int flag_delta = 1;
    int delta = 0;
    int i = 0;

    for(; i < K; i++){
        delta = dist(old_centroids[i], new_centroids[i]);

        if (delta >= eps){
            flag_delta = 0;
            break;
        }

    }

    return flag_delta;
}

int arg_min_dist(struct vector data_point, struct vector *centroids){
    double min_dis = DBL_MAX;
    int min_index = -1;
    int i = 0;
    double distance = 0;

    for(; i < K; i++){

        distance = dist(data_point, centroids[i]);
        if (distance < min_dis) {
            min_dis = distance;
            min_index = i;
        }
    }

    return min_index;
}

/* 
 * Returns an array of pointers of length K.
 * Each pointer in the array represents a centroid and points to a linked list of data points assigned to that centroid.
 * clusters[i] means that the cluster at index i is the closest cluster to all data points in the linked list.
 */
struct vector** assign_data_points_to_clusters(struct vector *data_points, struct vector *centroids) {
    struct vector **clusters = malloc(K * sizeof(struct vector*));
    struct vector **last_nodes = malloc(K * sizeof(struct vector*));
    int i = 0;

    for (; i < K; i++)
        clusters[i] = NULL;

    i = 0;

    for (; i < N; i++) {
        int min_index = arg_min_dist(data_points[i], centroids);

        if (clusters[min_index] == NULL) {
            clusters[min_index] = malloc(sizeof(struct vector));
            *clusters[min_index] = data_points[i];
        } else
            last_nodes[min_index]->next = &data_points[i];

        last_nodes[min_index] = &data_points[i];
        last_nodes[min_index]->next = NULL;
    }

    free(last_nodes);

    return clusters;
}

/*
 * Returns an array of length K which contains updated centroids.
*/
struct vector* update_centroids(struct vector *centroids, struct vector **clusters) {
    int i = 0;

    /* For each centroid */
    for (; i < K; ++i) {
        /* Sum the vectors in its cluster */
        struct vector *sum_vector = sum_vectors_in_cluster(&centroids[i], clusters[i]);

        /* Divide by the number of vectors in the cluster */
        int k = count_vectors_in_cluster(clusters[i]);
        centroids[i] = *divide_by_scalar(*sum_vector, k);
    }

    return centroids;
}

struct vector* k_means(struct vector *data_points) {
    int i = 0;
    int iteration_number = 0;
    int flag_delta = 0;
    struct vector **clusters;
    struct vector *new_centroids;
    
    /* Initialize centroids as first k vectors */
    struct vector *centroids = malloc(K * sizeof(struct vector));
    for (; i < K; i++)
        centroids[i] = data_points[i];

    /* Repeat until convergence of centroids or until iteration_number == iter */
    while ((flag_delta == 0) && (iteration_number < iter)) {
        iteration_number++;

        /* Assign every x_i to the closest cluster */
        clusters = assign_data_points_to_clusters(data_points, centroids);

        /* Update centroids */
        new_centroids = update_centroids(centroids, clusters);

        /* Check convergence of centroids */
        flag_delta = compute_flag_delta(centroids, new_centroids);

        centroids = new_centroids;
    }

    return centroids;
}

