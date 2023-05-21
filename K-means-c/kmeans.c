#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <float.h>

/* To Compile:
 * gcc -ansi -Wall -Wextra -Werror -pedantic-errors kmeans.c -o kmeans -lm
 * */

static int N = 0;
static int d = 1; /* Dimension is at least 1. */
static int K = 0;
static int iter = 200;
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
int main(int argc, char *argv[]);
struct vector* read_data_points(void);
void print_vectors(struct vector *vectors);
void print_centroids(struct vector *centroids);
int check_argument(int smallest, char arg[], int largest);
int is_number(char number[]);

struct vector* k_means(struct vector *vectors);
struct vector* copy_first_K_vectors(struct vector* vectors);
struct entry* copy_entries(struct entry* original_entries);
struct vector** assign_data_points_to_clusters(struct vector *data_points, struct vector *centroids);
int arg_min_dist(struct vector data_point, struct vector *centroids);
struct vector* get_new_centroids(struct vector **clusters);
int compute_flag_delta(struct vector *old_centroids, struct vector *new_centroids);

struct entry* sum_entries(struct entry *u, struct entry *v);
struct vector* zero_vector(void);
struct vector* sum_vectors_in_cluster(struct vector *cluster);
struct vector divide_by_scalar(struct vector v, double scalar);
int count_vectors_in_cluster(struct vector *cluster);
double dist(struct vector u, struct vector v);
void free_vectors(struct vector *head);
void free_centroids(struct vector *centroids);
void free_entries(struct entry *head);
void free_clusters(struct vector **clusters);

/* Code */
int main(int argc, char *argv[]) {
    struct vector *vectors;
    int flag_K = 0, flag_iter = 1;
    struct vector *centroids;

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

        /* printf("K: %d, iter: %d, N: %d, d: %d\n\n", K, iter, N, d); */

        /* Execute K-means algorithm */
        centroids = k_means(vectors);
        print_centroids(centroids);

        free_centroids(centroids);
        free_vectors(vectors);

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

struct vector* read_data_points(void) {

    struct vector *head_vec, *curr_vec;
    struct entry *head_entry, *curr_entry;
    double n;
    char c;

    head_entry = malloc(sizeof(struct entry));
    if (head_entry == NULL) {   /* Memory allocation failed */
        printf("Failed to allocate memory\n");
        exit(1);
    }
    curr_entry = head_entry;
    curr_entry->next = NULL;

    curr_vec = malloc(sizeof(struct vector));
    if (curr_vec == NULL) {   /* Memory allocation failed */
        printf("Failed to allocate memory\n");
        exit(1);
    }

    curr_vec->next = NULL;
    head_vec = curr_vec;

    while (scanf("%lf%c", &n, &c) == 2) {

        /* We have read all the entries for the current vector */
        if (c == '\n') {

            curr_entry->value = n;
            curr_vec->entries = head_entry;
            curr_vec->next = calloc(1, sizeof(struct vector));
            if (curr_vec == NULL) {   /* Memory allocation failed */
                printf("Failed to allocate memory\n");
                exit(1);
            }

            curr_vec = curr_vec->next;
            curr_vec->next = NULL;

            head_entry = malloc(sizeof(struct entry));
            if (head_entry == NULL) {   /* Memory allocation failed */
                printf("Failed to allocate memory\n");
                exit(1);
            }

            curr_entry = head_entry;
            curr_entry->next = NULL;

            /* Count the number of vectors N */
            N++;
            continue;
        }

        /* Read the next entry of the current vector */
        curr_entry->value = n;
        curr_entry->next = malloc(sizeof(struct entry));
        if (curr_entry == NULL) {   /* Memory allocation failed */
            printf("Failed to allocate memory\n");
            exit(1);
        }
        curr_entry = curr_entry->next;
        curr_entry->next = NULL;

        /* Count the dimension d */
        if (N == 0)
            d++;
    }

    free_entries(head_entry);

    return head_vec;
}

void print_vectors(struct vector *vectors) {
    struct vector* curr_vec = vectors;

    while (curr_vec != NULL) {
        struct entry* entry = curr_vec->entries;

        while (entry != NULL) {
            printf("%.4f ", entry->value);
            entry = entry->next;
        }

        printf("\n");
        fflush(stdout); 
        curr_vec = curr_vec->next;
    }
}

void print_centroids(struct vector *centroids) {
    struct entry* entry;
    int i = 0;

    for (; i < K; i++) {
        entry = centroids[i].entries;

        while (entry != NULL) {

            if (entry->next == NULL)
                printf("%.4f", entry->value);
            else
                printf("%.4f,", entry->value);
            entry = entry->next;

        }

        printf("\n");
        fflush(stdout); 
    }
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

    if (number == NULL || number[0] == '\0') {
        return 0;
    }

    /* Checking for negative numbers */
    if (number[0] == '-')
        i = 1;

    for (; number[i] != 0; i++) {
        /* If (number[i] > '9' || number[i] < '0') */
        if (!isdigit(number[i]))
            return 0;
    }
    return 1;
}

/*  input: Linked list of vectors.
    output: array of vectors */
struct vector* k_means(struct vector *vectors) {
    int iteration_number = 0;
    int flag_delta = 0;
    struct vector **clusters;
    struct vector *new_centroids, *centroids;

    /* Initialize centroids as first K vectors */
    centroids = copy_first_K_vectors(vectors);

    /* Repeat until convergence of centroids or until iteration_number == iter */
    while ((flag_delta == 0) && (iteration_number < iter)) {
        iteration_number++;

        /* Free previous clusters */
        if (iteration_number > 1)
            free_clusters(clusters);

        /* Assign every x_i to the closest cluster */
        clusters = assign_data_points_to_clusters(vectors, centroids);

        /* Get new centroids */
        new_centroids = get_new_centroids(clusters);

        /* Check convergence of centroids */
        flag_delta = compute_flag_delta(centroids, new_centroids);

        /* Free previous centroids */
        free_centroids(centroids);

        /* Update centroids */
        centroids = new_centroids;
    }

    free_clusters(clusters);

    return centroids;
}

struct vector* copy_first_K_vectors(struct vector* vectors){
    struct vector *centroids;
    struct vector *curr_vec;
    struct vector *curr_cent;
    int i = 0;

    centroids = malloc(K * sizeof(struct vector));
    if (centroids == NULL) {   /* Memory allocation failed */
        printf("Failed to allocate memory\n");
        exit(1);
    }

    curr_vec = vectors;
    for (; i < K; i++) {
        curr_cent = &centroids[i];
        curr_cent->entries = copy_entries(curr_vec->entries);
        curr_cent->next = NULL;
        curr_vec = curr_vec->next;
    }

    return centroids;
}

struct entry* copy_entries(struct entry* original_entries) {
    struct entry* new_entries = NULL;
    struct entry* current = original_entries;
    struct entry* prev = NULL;
    struct entry *new_entry;

    while (current != NULL) {

        new_entry = malloc(sizeof(struct entry));

        if (new_entry == NULL) {   /* Memory allocation failed */
            printf("Failed to allocate memory\n");
            exit(1);
        }

        new_entry->value = current->value;
        new_entry->next = NULL;

        if (prev != NULL)
            prev->next = new_entry;
        else
            new_entries = new_entry;

        prev = new_entry;
        current = current->next;

    }

    return new_entries;
}

/* 
 * Returns an array of pointers of length K.
 * Each pointer in the array represents a centroid and points to a linked list of data points assigned to that centroid.
 * clusters[i] means that the cluster at index i has centroid indexed i as its closest centroid.
 */
struct vector** assign_data_points_to_clusters(struct vector *data_points, struct vector *centroids) {
    struct vector **clusters;
    struct vector *curr_data_point = data_points; /* used to iterate over the data points. */
    struct vector *data_point; /* used to make a copy of a data point. */
    int i = 0;
    int min_index = -1;

    clusters = malloc(K * sizeof(struct vector*));
    if (clusters == NULL) {   /* Memory allocation failed */
        printf("Failed to allocate memory\n");
        exit(1);
    }

    for (; i < K; i++)
        clusters[i] = NULL;

    i = 0;
    for (; i < N; i++) {

        data_point = malloc(sizeof(struct vector));

        if (data_point == NULL) {   /* Memory allocation failed */
            printf("Failed to allocate memory\n");
            exit(1);
        }

        data_point->entries = copy_entries(curr_data_point->entries);
        data_point->next = NULL;
                
        min_index = arg_min_dist(*data_point, centroids);

        if (clusters[min_index] != NULL)
            data_point->next = clusters[min_index];

        clusters[min_index] = data_point;
        curr_data_point = curr_data_point->next;
    }

    return clusters;
}

int arg_min_dist(struct vector data_point, struct vector *centroids) {
    double min_dis = DBL_MAX;
    int min_index = -1;
    int i = 0;
    double distance;

    for(; i < K; i++) {

        distance = dist(data_point, centroids[i]);
        if (distance < min_dis) {
            min_dis = distance;
            min_index = i;
        }

    }

    return min_index;
}

/*
 * Returns a new array of length K which contains updated centroids.
*/
struct vector* get_new_centroids(struct vector **clusters) {
    struct vector *new_centroids = malloc(K * sizeof(struct vector));
    int i = 0, k = -1;
    struct vector *sum_vector;

    if (new_centroids == NULL) {   /* Memory allocation failed */
        printf("Failed to allocate memory\n");
        exit(1);
    }

    i = 0;

    /* For each centroid */
    for (; i < K; ++i) {

        /* Count number of vectors in cluster. */
        k = count_vectors_in_cluster(clusters[i]);

        /* Sum the vectors in its cluster. */
        sum_vector = sum_vectors_in_cluster(clusters[i]);

        /* Divide by the number of vectors in the cluster. */
        new_centroids[i] = divide_by_scalar(*sum_vector, k);

        free_entries(sum_vector->entries);
        free(sum_vector);
    }

    return new_centroids;
}

struct entry* sum_entries(struct entry *u, struct entry *v) {
    struct entry* new_entries = NULL;
    struct entry* curr1 = u;
    struct entry* curr2 = v;
    struct entry* prev = NULL;
    struct entry *new_entry;
    double sum = 0;

    while (curr1 != NULL && curr2 != NULL) {
        new_entry = calloc(1, sizeof(struct entry));
        sum = curr1->value + curr2->value;

        if (new_entry == NULL) {   /* Memory allocation failed */
            printf("Failed to allocate memory\n");
            exit(1);
        }

        new_entry->value = sum;
        new_entry->next = NULL;

        if (prev != NULL)
            prev->next = new_entry;
        else
            new_entries = new_entry;

        prev = new_entry;
        curr1 = curr1->next;
        curr2 = curr2->next;
    }

    return new_entries;
}

struct vector* zero_vector(void) {
    struct vector *v = calloc(1, sizeof(struct vector));
    struct entry* zero_entries = NULL;
    struct entry* prev = NULL;
    struct entry *new_entry;
    int i = 0;

    for (; i < d; i++) {
        new_entry = malloc(sizeof(struct entry));
        if (new_entry == NULL) {   /* Memory allocation failed */
            printf("Failed to allocate memory\n");
            exit(1);
        }
        new_entry->value = 0;
        new_entry->next = NULL;

        if (prev != NULL)
            prev->next = new_entry;
        else
            zero_entries = new_entry;

        prev = new_entry;
    }

    v->entries = zero_entries;
    v->next = NULL;

    return v;
}

struct vector* sum_vectors_in_cluster(struct vector *cluster) {
   struct vector *curr_cluster_vector = cluster;
   struct vector *sum_vector = zero_vector();

   while (curr_cluster_vector != NULL){
        struct entry *temp = sum_vector->entries;
        sum_vector->entries = sum_entries(curr_cluster_vector->entries, temp);
        free_entries(temp);

        curr_cluster_vector = curr_cluster_vector->next;
   }
   return sum_vector;
}

struct vector divide_by_scalar(struct vector v, double scalar) {
    struct vector result_vector;
    struct entry* new_entries = NULL;
    struct entry* curr_entry = v.entries;
    struct entry* prev_entry = NULL;

    while (curr_entry != NULL) {
        struct entry* new_entry = malloc(sizeof(struct entry));
        if (new_entry == NULL) {   /* Memory allocation failed */
            printf("Failed to allocate memory\n");
            exit(1);
        }

        new_entry->value = curr_entry->value / scalar;
        new_entry->next = NULL;

        if (prev_entry != NULL)
            prev_entry->next = new_entry;
        else
            new_entries = new_entry;

        prev_entry = new_entry;
        curr_entry = curr_entry->next;
    }

    result_vector.entries = new_entries;
    result_vector.next = NULL;

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

/*
 * pre-condition: length of old_centroids == length of new_centroids.
 * returns: 1 if and only if each delta is strictly less than eps.
*/
int compute_flag_delta(struct vector *old_centroids, struct vector *new_centroids) {
    int flag_delta = 1;
    double delta = 0;
    int i = 0;

    for(; i < K; i++) {
        delta = dist(old_centroids[i], new_centroids[i]);
        if (delta >= eps){
            flag_delta = 0;
            break;
        }

    }

    return flag_delta;
}


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

void free_vectors(struct vector *head) {
    if (head != NULL) {
        free_entries(head->entries);
        free_vectors(head->next);
        free(head);
    }
}

void free_centroids(struct vector *centroids) {
    int i = 0;

    for(; i < K; i++)
        free_entries(centroids[i].entries);

    free(centroids);
}

void free_entries(struct entry *head) {
    if (head != NULL){
        free_entries(head->next);
        free(head);
    }
}

void free_clusters(struct vector **clusters) {
    int i = 0;

    if (clusters == NULL)
        return;

    for(; i < K; i++) {
        free_vectors(clusters[i]);
    }

    free(clusters);
}
