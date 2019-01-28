
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <parallel/algorithm>
#include <random>
#include <sstream>
#include <utility>
#include <vector>

//#include <glog/logging.h>
#include <ips4o.hpp>

#include "radix_sort.hpp"

int str_to_int(std::string s) {
    std::istringstream ss(s);
    int n;
    if (!(ss >> n)) {
        std::cerr << "Invalid number: " << s;
        return -1;
    } else if (!ss.eof()) {
        std::cerr << "Trailing characters after number: " << s;
        return -1;
    }
    return n;
}

double str_to_double(std::string s) {
    std::istringstream ss(s);
    double n = 0;
    // TODO(Chris): convert string to int

    return n;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "bench_radix_sort [number] [pprs | lsd | pms | pqs | std::sort | std::stable_sort | ips4o | pips4o] [std::rand | uniform | normal | poisson | exponential | bernoulli | geometric | binomial] [dist_params: seed | (s_range, e_range) | (mean, std_dev) | mean | lambda | p | p | (t, p)] " << std::endl;
        return -1;
    }
//    FLAGS_logtostderr = true;
//    google::InitGoogleLogging(argv[0]);

    int number = str_to_int(argv[1]);
    std::vector<std::pair<recomp::var_t, recomp::var_t>> vector(static_cast<size_t>(number));

    std::string algo(argv[2]);
    std::cout << "Using sorting algorithm: " << algo;

    std::string dist_type = std::string(argv[3]);
    std::cout << "Using random generator: " << dist_type;
    if (dist_type == "std::rand") {

        auto seed = std::time(nullptr);
        seed = str_to_int(argv[4]);
        std::srand(seed); // use current time as seed for random generator
        std::cout << "Using seed: " << std::to_string(seed);


        for (int i = 0; i < number; ++i) {
            vector[i] = std::make_pair(static_cast<recomp::var_t>(std::rand()),
                                       static_cast<recomp::var_t>(std::rand()));
        }
    } else if (dist_type == "uniform") {
        auto s_range = str_to_int(argv[4]);
        auto e_range = str_to_int(argv[5]);

        std::default_random_engine gen;
        std::uniform_int_distribution<recomp::var_t> distribution(s_range, e_range);

        for (int i = 0; i < number; ++i) {
            vector[i] = std::make_pair(distribution(gen), distribution(gen));
        }
    } else if (dist_type == "poisson") {
        auto mean = str_to_double(argv[4]);

        std::default_random_engine gen;
        std::poisson_distribution<recomp::var_t> distribution(mean);

        for (int i = 0; i < number; ++i) {
            vector[i] = std::make_pair(distribution(gen), distribution(gen));
        }
    } else if (dist_type == "exponential") {
        auto lambda = str_to_double(argv[4]);

        std::default_random_engine gen;
        std::exponential_distribution<double> distribution(lambda);

        for (int i = 0; i < number; ++i) {
            vector[i] = std::make_pair(distribution(gen), distribution(gen));
        }
    } else if (dist_type == "geometric") {
        auto p = str_to_double(argv[4]);

        std::default_random_engine gen;
        std::geometric_distribution<recomp::var_t> distribution(p);

        for (int i = 0; i < number; ++i) {
            vector[i] = std::make_pair(distribution(gen), distribution(gen));
        }
    } else if (dist_type == "binomial") {
        auto t = str_to_int(argv[4]);
        auto p = str_to_double(argv[5]);

        std::default_random_engine gen;
        std::binomial_distribution<recomp::var_t> distribution(t, p);

        for (int i = 0; i < number; ++i) {
            vector[i] = std::make_pair(distribution(gen), distribution(gen));
        }
    } else if (dist_type == "bernoulli") {
//        auto p = str_to_double(argv[4]);
//
//        std::default_random_engine gen;
//        std::bernoulli_distribution<double> distribution(p);
//
//        for (int i = 0; i < number; ++i) {
//            vector[i] = std::make_pair(distribution(gen), distribution(gen));
//        }
    } else if (dist_type == "normal") {
        auto mean = str_to_double(argv[4]);
        auto std_dev = str_to_double(argv[5]);

        std::default_random_engine gen;
        std::normal_distribution<double> distribution(mean, std_dev);

        for (int i = 0; i < number; ++i) {
            vector[i] = std::make_pair(distribution(gen), distribution(gen));
        }
    }

    if (number < 31) {
        std::cout << recomp::util::vector_blocks_to_string(vector) << std::endl;
    }
    if (recomp::util::is_sorted(vector)) {
        std::cout << "vector already sorted" << std::endl;
    } else {
        std::cout << "vector not sorted" << std::endl;
    }
    
    if (algo == "pprs") {
        const auto startTime = std::chrono::system_clock::now();

        recomp::parallel::partitioned_radix_sort<recomp::var_t, 8>(vector);
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Time for partitioned parallel radix sort: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
        std::cout << "Time for partitioned parallel radix sort: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        std::cout << "RESULT algorithm=" << algo << " elements=" << std::to_string(number) << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;

        if (recomp::util::is_sorted(vector)) {
            std::cout << "Partitioned parallel radix sort sorted correctly" << std::endl;
        } else {
            std::cerr << "Sorting with partitioned parallel radix sort failed" << std::endl;
        }
    } else if (algo == "lsd") {
        const auto startTime = std::chrono::system_clock::now();

        recomp::lsd_radix_sort<recomp::var_t, 8>(vector);
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Time for LSD radix sort: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
        std::cout << "Time for LSD radix sort: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        std::cout << "RESULT algorithm=" << algo << " elements=" << std::to_string(number) << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;

        if (recomp::util::is_sorted(vector)) {
            std::cout << "LSD radix sort sorted correctly" << std::endl;
        } else {
            std::cerr << "Sorting with LSD radix sort failed" << std::endl;
        }
    } else if (algo == "pms") {
        const auto startTime = std::chrono::system_clock::now();

        __gnu_parallel::sort(vector.begin(), vector.end(), __gnu_parallel::multiway_mergesort_tag());
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Time for parallel merge sort: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
        std::cout << "Time for parallel merge sort: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        std::cout << "RESULT algorithm=" << algo << " elements=" << std::to_string(number) << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;

        if (recomp::util::is_sorted(vector)) {
            std::cout << "parallel merge sort sorted correctly" << std::endl;
        } else {
            std::cerr << "Sorting with parallel merge sort failed" << std::endl;
        }
    } else if (algo == "pqs") {
        const auto startTime = std::chrono::system_clock::now();

        __gnu_parallel::sort(vector.begin(), vector.end(), __gnu_parallel::quicksort_tag());
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Time for parallel quick sort: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
        std::cout << "Time for parallel quick sort: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        std::cout << "RESULT algorithm=" << algo << " elements=" << std::to_string(number) << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;

        if (recomp::util::is_sorted(vector)) {
            std::cout << "parallel quick sort sorted correctly" << std::endl;
        } else {
            std::cerr << "Sorting with parallel quick sort failed" << std::endl;
        }
    } else if (algo == "std::sort") {
        const auto startTime = std::chrono::system_clock::now();

        std::sort(vector.begin(), vector.end());
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Time for std::sort: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
        std::cout << "Time for std::sort: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        std::cout << "RESULT algorithm=" << algo << " elements=" << std::to_string(number) << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;

        if (recomp::util::is_sorted(vector)) {
            std::cout << "std::sort sorted correctly" << std::endl;
        } else {
            std::cerr << "Sorting with std::sort failed" << std::endl;
        }
    } else if (algo == "std::stable_sort") {
        const auto startTime = std::chrono::system_clock::now();

        std::stable_sort(vector.begin(), vector.end());
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Time for std::stable_sort: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
        std::cout << "Time for std::stable_sort: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        std::cout << "RESULT algorithm=" << algo << " elements=" << std::to_string(number) << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;

        if (recomp::util::is_sorted(vector)) {
            std::cout << "std::stable_sort sorted correctly" << std::endl;
        } else {
            std::cerr << "Sorting with std::stable_sort failed" << std::endl;
        }
    } else if (algo == "pips4o") {
        const auto startTime = std::chrono::system_clock::now();

        ips4o::parallel::sort(vector.begin(), vector.end());
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Time for parallel IPS4o: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
        std::cout << "Time for parallel IPS4o: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        std::cout << "RESULT algorithm=" << algo << " elements=" << std::to_string(number) << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;

        if (recomp::util::is_sorted(vector)) {
            std::cout << "parallel IPS4o sorted correctly" << std::endl;
        } else {
            std::cerr << "Sorting with parallel IPS4o failed" << std::endl;
        }
    } else if (algo == "ips4o") {
        const auto startTime = std::chrono::system_clock::now();

        ips4o::sort(vector.begin(), vector.end());
        const auto endTime = std::chrono::system_clock::now();
        const auto timeSpan = endTime - startTime;
        std::cout << "Time for IPS4o: " << std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count() << "[s]" << std::endl;
        std::cout << "Time for IPS4o: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
        std::cout << "RESULT algorithm=" << algo << " elements=" << std::to_string(number) << " time=" << std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()) << std::endl;

        if (recomp::util::is_sorted(vector)) {
            std::cout << "IPS4o sorted correctly" << std::endl;
        } else {
            std::cerr << "Sorting with IPS4o failed" << std::endl;
        }
    } else {
        std::cerr << "Algorithm " << algo << " not supported" << std::endl;
    }
    
    if (number < 31) {
        std::cout << recomp::util::vector_blocks_to_string(vector) << std::endl;
    }

    return 0;
}
