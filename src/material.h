#pragma once
#include "rng.h"
#include "hittable.h"

class material {
public:
    virtual ~material() = default;
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
    virtual color emitted() const { return color(0,0,0); }
};

class lambertian : public material {
public:
    color albedo;
    explicit lambertian(const color& a): albedo(a) {}

    bool scatter(const ray&, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 scatter_dir = rec.normal + random_unit_vector();
        if (scatter_dir.near_zero()) scatter_dir = rec.normal;
        scattered = ray(rec.p, scatter_dir);
        attenuation = albedo;
        return true;
    }
};

class metal : public material {
public:
    color albedo;
    double fuzz;
    metal(const color& a, double f): albedo(a), fuzz(f<1?f:1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
};

class dielectric : public material {
public:
    double ir; // index of refraction
    explicit dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    static double schlick(double cosine, double ref_idx) {
        double r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow(1-cosine, 5);
    }

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

        vec3 unit_dir = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_dir, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract || schlick(cos_theta, refraction_ratio) > rand_double())
            direction = reflect(unit_dir, rec.normal);
        else
            direction = refract(unit_dir, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);
        return true;
    }
};

class diffuse_light : public material {
public:
    color emit;
    explicit diffuse_light(const color& c): emit(c) {}
    bool scatter(const ray&, const hit_record&, color&, ray&) const override { return false; }
    color emitted() const override { return emit; }
};
