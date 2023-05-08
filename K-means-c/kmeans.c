/*
 * Created by Yoav Shkedy on 03/05/2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<ctype.h>

static int N = 0;
static int d = 1; /* dimension is atleast 1. */
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
struct vector* read_datapoints();
int check_argument(int smallest, char arg[], int largest);
int isNumber(char number[]);

double dist(struct vector u, struct vector v);
struct vector* sum_vectors(struct vector u, struct vector v);
struct vector* divide_by_scalar(struct vector v, double scalar);

int compute_flag_delta(struct vector *old_centroids, struct vector *new_centroids);
int find_min_index(struct vector data_point, struct vector *centroids);
struct vector** assign_datapoints_to_clusters(struct vector *data_points, struct vector * centroids);
struct vector* k_means(struct vector *vectors);

/* Code */
int main(int argc, char *argv[]) {
    struct vector *vectors;
    int flag_K, flag_iter;

    vectors = read_datapoints();

    flag_K = check_argument(1, argv[1], N);
    if (argc == 3){
        flag_iter = check_argument(1, argv[2], 1000);
    }

    if (flag_K == 1 && flag_iter == 1){
        K = atoi(argv[1]);
        iter = argc == 3 ? atoi(argv[2]) : 200;
        /*
        centroids = k_means(vectors);
        */
        printf("K: %d, iter: %d, N: %d, d: %d\n", K, iter, N, d);
        printf("%f, ", vectors->entries->value);
        printf("%f, ", vectors->entries->next->value);
        printf("%f\n", vectors->entries->next->next->value);
        
        return 0;
    } else { 
        return 1;
    }
}

/** Argument reading and processing **/

struct vector* read_datapoints(){

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

/* returns 1 if and only if all requirements about the argument are met. */
int check_argument(int smallest, char arg[], int largest){
    int flag_is_num = isNumber(arg);
    int num;

    if (flag_is_num == 1){
        num = atoi(arg);
        if (!(smallest < num && num < largest)){
            return 0;
        }
        return 1;
    }
    return 0;
}

/* returns 1 if and only if number is an integer. */
int isNumber(char number[])
{
    int i = 0;

    /*checking for negative numbers */
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        /*if (number[i] > '9' || number[i] < '0') */
        if (!isdigit(number[i])){
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
    
    struct entry *u_entry = u.entries;
    struct entry *v_entry = v.entries;

    struct vector *head_vec, *curr_vec;
    struct entry *head_entry, *curr_entry;

    head_entry = malloc(sizeof(struct entry));
    curr_entry = head_entry;
    curr_entry->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;

    for (; i < d; i++) {

        curr_entry->value = u_entry->value + v_entry->value;
        curr_entry->next = malloc(sizeof(struct entry));
        curr_entry = curr_entry->next;
        curr_entry->next = NULL;

        u_entry = u_entry->next;
        v_entry = v_entry->next;

    }

    free(curr_vec->next);
    free(head_entry);

    return head_vec;
}

struct vector* divide_by_scalar(struct vector v, double scalar) {
    int i = 0;

    struct entry *v_entry = v.entries;

    struct vector *head_vec, *curr_vec;
    struct entry *head_entry, *curr_entry;

    head_entry = malloc(sizeof(struct entry));
    curr_entry = head_entry;
    curr_entry->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;

    for (; i < d; i++) {
        curr_entry->value = (v_entry->value) / scalar; 
        curr_entry->next = malloc(sizeof(struct entry));
        curr_entry = curr_entry->next;
        curr_entry->next = NULL;

        v_entry = v_entry->next;

    }

    free(curr_vec->next);
    free(head_entry);

    return head_vec;
}

/** Algorithm **/


/**
 * pre-condition: old_centroids' length == new_centroids' length.
 * returns: 1 if and only if every delta is less than eps.
*/
int compute_flag_delta(struct vector *old_centroids, struct vector *new_centroids){
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

int find_min_index(struct vector data_point, struct vector *centroids){
    double min_dis = 1.79769e+308; /** largest double **/
    int min_index = -1;
    int i = 0;
    double dis = 0;

    for(; i < K; i++){
        dis = dist(data_point, centroids[i]);
        if (dis < min_dis){
            min_dis = dis;
            min_index = i;
        }
    }

    return min_index;
}

/* 
 * returns a pointers array (of length K) of arrays containing datapoints assigning each one to a centroid.
 * i.e. assignment[i][j] means that cluster indexed i was the closest cluster to the data point at assignment[i][j].
 */
struct vector** assign_datapoints_to_clusters(struct vector *data_points, struct vector *centroids){

    struct vector *groups_of_clusters = calloc(N*K, sizeof(struct vector));
    struct vector **assignment = calloc(K, sizeof(struct vector*));
    int *number_of_vectors_in_each_cluster = calloc(K, sizeof(int));
    int i = 0, min_index = -1;

    for (; i < K; i++){
        assignment[i] = groups_of_clusters + i*N;
    }

    i = 0;

    for (; i < N; i++){
        min_index = find_min_index(data_points[i], centroids);
        assignment[min_index][number_of_vectors_in_each_cluster[min_index]] = data_points[i];
        number_of_vectors_in_each_cluster[min_index]++;
    }

    return assignment;
}


/**
 * returns an array (of length K) of updated centroids.
*/
struct vector* update_centroids(struct vector** assignment){
    return assignment[0];
}

struct vector* k_means(struct vector *data_points) {
    int i = 0;
    int iteration_number = 0;
    int flag_delta = 0;
    struct vector **assignment;
    struct vector *new_centroids;
    
    /* Initialize centroids as first k vectors */
    struct vector *centroids = malloc(N);
    for (; i < N; i++)
        centroids[i] = data_points[i];

    /* Repeat until convergence of centroids or until iteration_number == iter */
    while (!(iteration_number == iter || flag_delta == 1)) {
        iteration_number++; 
        /* Assign every x_i to the closest cluster */
        assignment = assign_datapoints_to_clusters(data_points, centroids);
        new_centroids = update_centroids(assignment);
        flag_delta = compute_flag_delta(centroids, new_centroids);
        centroids = new_centroids;
    }

    return centroids;
}

