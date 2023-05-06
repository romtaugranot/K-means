import math
import sys

eps = 0.001


### Math functions ###

# pre-condition: vector1's length == vector2's length
def euclidean_distance(vector1, vector2):
    dis = 0
    for i in range(len(vector1)):
        dis += (vector2[i] - vector1[i]) ** 2
    return math.sqrt(dis)


# pre-condition: vector1's length == vector2's length
def sum_vectors(vector1, vector2):
    vector = [0 for _ in range(len(vector1))]
    for i in range(len(vector1)):
        vector[i] = float("{:.4f}".format(vector1[i] + vector2[i]))
    return vector


def divide_by_scalar(vector, scalar):
    new_vector = [0 for _ in range(len(vector))]
    for i in range(len(vector)):
        new_vector[i] = float("{:.4f}".format(vector[i] / scalar))
    return new_vector


### Argument reading and processing ###

def read_file():
    iter = 200  # default value.
    i = 3  # represents the index of the file in the args array, assuming iter was provided.
    data_points = []
    flag_iter = True
    if len(sys.argv) == 4:  # iter was provided
        try:
            iter = int(sys.argv[2])
        except ValueError:
            flag_iter = False
    else:  # iter was not provided
        i -= 1
    flag_file = True
    try:
        with open(sys.argv[i]) as file:
            for line in file:
                vector = line.rstrip().split(',')
                vector = [eval(i) for i in vector]
                data_points += [vector]
    except OSError:
        flag_file = False
    return flag_file, flag_iter, data_points, iter


def initialize():
    # default values
    K = 0
    flag_K = True
    try:
        K = int(sys.argv[1])
    except ValueError:
        flag_K = False
    flag_file, flag_iter, data_points, iter = read_file()
    return flag_K, flag_iter, flag_file, K, iter, data_points


### Algorithm ###

# pre-condition: old_centroids' length == new_centroids' length
def compute_flag_delta(old_centroids, new_centroids):  # returns True if and only if every delta is less than eps.
    flag_delta = True
    for i in range(len(old_centroids)):
        delta = euclidean_distance(old_centroids[i], new_centroids[i])
        if delta >= eps:
            flag_delta = False
            break
    return flag_delta


def find_min_index(data_point, centroids):
    min_dis = math.inf
    min_index = -1
    for k in range(len(centroids)):
        dis = euclidean_distance(data_point, centroids[k])
        if dis < min_dis:
            min_dis = dis
            min_index = k
    return min_index


# returns an array (of length K) of arrays containing datapoints assigning each one to a centroid.
# i.e. assignment[i][j] means that cluster indexed i was the closest cluster to the data point at assignment[i][j].
def assign_datapoints_to_clusters(data_points, centroids):
    assignment = [[] for _ in range(len(centroids))]
    for i in range(len(data_points)):
        min_index = find_min_index(data_points[i], centroids)
        assignment[min_index] += [data_points[i]]
    return assignment


# returns an array (of length K) of updated centroids.
def update_centroids(assignment, K, d):
    updated = []
    for i in range(K):
        new_centroid = [0 for _ in range(d)]
        for j in range(len(assignment[i])):
            new_centroid = sum_vectors(new_centroid, assignment[i][j])
        new_centroid = divide_by_scalar(new_centroid, len(assignment[i]))
        updated += [new_centroid]
    return updated


def check_arguments(flag_K, flag_iter, flag_file, K, N, iter):
    flag_K = flag_K and 1 < K < N
    if flag_K:
        # print("K:", K)
        pass
    else:
        print("Invalid number of clusters!")

    flag_iter = flag_iter and 1 < iter < 1000
    if flag_iter:
        # print("iter:", iter)
        pass
    else:
        print("Invalid maximum iteration!")

    if not flag_file:
        print("NA")

    return flag_K and flag_iter and flag_file


def print_results(clusters):
    for i in range(len(clusters)):
        print(str(clusters[i]).replace(" ", "")[1:-1])


def k_means_algorithm():
    flag_K, flag_iter, flag_file, K, iter, data_points = initialize()
    N = len(data_points)
    d = len(data_points[0])
    arguments_ok = check_arguments(flag_K, flag_iter, flag_file, K, N, iter)

    if arguments_ok:  # True if and only if all requirements about the arguments are met.
        centroids = [data_points[i] for i in range(K)]
        iteration_number = 0
        flag_delta = compute_flag_delta(centroids, [[0 for _ in range(d)] for _ in range(K)])

        while not (iteration_number == iter or flag_delta):
            iteration_number += 1
            assignment = assign_datapoints_to_clusters(data_points, centroids)
            new_centroids = update_centroids(assignment, K, d)
            flag_delta = compute_flag_delta(centroids, new_centroids)
            centroids = new_centroids

        print_results(centroids)


k_means_algorithm()
