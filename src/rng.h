#pragma once
#include <random>
#include "vec3.h"

inline std::mt19937_64& global_rng() {
    static thread_local std::mt19937_64 gen{std::random_device{}()};
    return gen;
}
inline double rand_double(double min=0.0, double max=1.0) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(global_rng());
}

inline vec3 random_vec(double min=0.0, double max=1.0) {
    return vec3(rand_double(min,max), rand_double(min,max), rand_double(min,max));
}
inline vec3 random_in_unit_sphere() {
    while (true) {
        vec3 p = random_vec(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}
inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}
inline vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in = random_in_unit_sphere();
    return (dot(in, normal) > 0.0) ? in : -in;
}
inline vec3 random_in_unit_disk() {
    while (true) {
        vec3 p(rand_double(-1,1), rand_double(-1,1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}
