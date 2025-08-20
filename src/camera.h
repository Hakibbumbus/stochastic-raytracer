#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <thread>
#include <atomic>
#include <vector>
#include "rng.h"
#include "color.h"
#include "hittable.h"
#include "material.h"

class camera {
public:
    // public params
    double aspect_ratio = 16.0/9.0;
    int image_width = 800;
    int samples_per_pixel = 100;
    int max_depth = 50;

    double vfov = 40.0;          // vertical field-of-view in degrees
    point3 lookfrom = point3(13,2,3);
    point3 lookat   = point3(0,0,0);
    vec3   vup      = vec3(0,1,0);

    double defocus_angle = 0.0;  // 0 disables depth of field
    double focus_dist = 10.0;

    void render(const hittable& world, std::ostream& out, int image_height_threads=0) {
        initialize();

        out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        // Simple row-parallel rendering
        int threads = image_height_threads > 0 ? image_height_threads
                                               : (int)std::thread::hardware_concurrency();
        if (threads < 1) threads = 1;

        std::vector<std::vector<color>> rows(image_height);
        std::atomic<int> next_row{0};
        auto worker = [&]() {
            while (true) {
                int j = next_row.fetch_add(1);
                if (j >= image_height) break;
                std::vector<color> scanline(image_width);
                for (int i = 0; i < image_width; ++i) {
                    color pixel_color(0,0,0);
                    for (int s = 0; s < samples_per_pixel; ++s) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, world, max_depth);
                    }
                    scanline[i] = pixel_color;
                }
                rows[j] = std::move(scanline);
            }
        };

        std::vector<std::thread> pool;
        for (int t = 0; t < threads; ++t) pool.emplace_back(worker);
        for (auto& th : pool) th.join();

        // Write rows in image (top to bottom)
        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                write_color(out, rows[j][i], samples_per_pixel);
            }
        }
    }

private:
    int image_height;
    point3 center;
    point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u,v,w;
    vec3 defocus_disk_u, defocus_disk_v;

    void initialize() {
        image_height = std::max(1, (int)(image_width / aspect_ratio));

        center = lookfrom;

        double theta = vfov * M_PI/180.0;
        double h = std::tan(theta/2);
        double viewport_height = 2.0 * h * focus_dist;
        double viewport_width = viewport_height * (double(image_width)/image_height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = viewport_height * -v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        point3 viewport_upper_left = center - focus_dist*w - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);

        double defocus_radius = focus_dist * std::tan((defocus_angle/2) * M_PI/180.0);
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        vec3 offset = sample_square();
        point3 pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        point3 origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        vec3 direction = pixel_sample - origin;
        return ray(origin, direction);
    }

    vec3 sample_square() const {
        return vec3(rand_double()-0.5, rand_double()-0.5, 0);
    }

    point3 defocus_disk_sample() const {
        vec3 p = random_in_unit_disk();
        return center + p.x()*defocus_disk_u + p.y()*defocus_disk_v;
    }

    color ray_color(const ray& r, const hittable& world, int depth) const {
        if (depth <= 0) return color(0,0,0);

        hit_record rec;
        if (world.hit(r, interval(0.001, INFINITY), rec)) {
            ray scattered;
            color attenuation;
            color emit = rec.mat->emitted();
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return emit + attenuation * ray_color(scattered, world, depth-1);
            return emit;
        }

        // Sky gradient
        vec3 unit_dir = unit_vector(r.direction());
        double t = 0.5*(unit_dir.y() + 1.0);
        return (1.0 - t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
    }
};
