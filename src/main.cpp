#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "rng.h"

hittable_list random_scene() {
    hittable_list world;

    auto ground_mat = new lambertian(color(0.5,0.5,0.5));
    world.add(std::make_unique<sphere>(point3(0,-1000,0), 1000, ground_mat));

    for (int a=-11; a<11; ++a) {
        for (int b=-11; b<11; ++b) {
            double choose_mat = rand_double();
            point3 center(a + 0.9*rand_double(), 0.2, b + 0.9*rand_double());

            if ((center - point3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.65) {
                    // diffuse
                    color albedo = random_vec(0,1) * random_vec(0,1);
                    auto mat = new lambertian(albedo);
                    world.add(std::make_unique<sphere>(center, 0.2, mat));
                } else if (choose_mat < 0.85) {
                    // metal
                    color albedo = color(0.5*(1+rand_double()), 0.5*(1+rand_double()), 0.5*(1+rand_double()));
                    double fuzz = rand_double(0, 0.5);
                    auto mat = new metal(albedo, fuzz);
                    world.add(std::make_unique<sphere>(center, 0.2, mat));
                } else {
                    // glass
                    auto mat = new dielectric(1.5);
                    world.add(std::make_unique<sphere>(center, 0.2, mat));
                }
            }
        }
    }

    auto mat1 = new dielectric(1.5);
    world.add(std::make_unique<sphere>(point3(0,1,0), 1.0, mat1));

    auto mat2 = new lambertian(color(0.4, 0.2, 0.1));
    world.add(std::make_unique<sphere>(point3(-4,1,0), 1.0, mat2));

    auto mat3 = new metal(color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_unique<sphere>(point3(4,1,0), 1.0, mat3));

    return world;
}

int main(int argc, char** argv) {
    // Defaults
    int width = 800;
    int spp = 100;
    int max_depth = 50;
    double aspect = 16.0/9.0;
    std::string out_path = "image.ppm";

    // Parse args: --width 1200 --spp 200 --max-depth 60 --aspect 1.777 --out file.ppm
    for (int i=1; i<argc; ++i) {
        std::string key = argv[i];
        auto need = [&](const char* name)->std::string{
            if (i+1 >= argc) { std::cerr << "Missing value for " << name << "\n"; std::exit(1); }
            return std::string(argv[++i]);
        };
        if (key == "--width") width = std::stoi(need("--width"));
        else if (key == "--spp") spp = std::stoi(need("--spp"));
        else if (key == "--max-depth") max_depth = std::stoi(need("--max-depth"));
        else if (key == "--aspect") aspect = std::stod(need("--aspect"));
        else if (key == "--out") out_path = need("--out");
        else if (key == "--help" || key == "-h") {
            std::cout <<
            "Stochastic Ray Tracer\n"
            "  --width <int>       Image width (default 800)\n"
            "  --spp <int>         Samples per pixel (default 100)\n"
            "  --max-depth <int>   Max path depth (default 50)\n"
            "  --aspect <float>    Aspect ratio (default 1.777...)\n"
            "  --out <file.ppm>    Output PPM path (default image.ppm)\n";
            return 0;
        }
    }

    hittable_list world = random_scene();

    camera cam;
    cam.aspect_ratio = aspect;
    cam.image_width = width;
    cam.samples_per_pixel = spp;
    cam.max_depth = max_depth;
    cam.lookfrom = point3(13,2,3);
    cam.lookat = point3(0,0,0);
    cam.vup = vec3(0,1,0);
    cam.vfov = 20.0;
    cam.focus_dist = 10.0;
    cam.defocus_angle = 0.05; // small DOF

    std::ofstream out(out_path, std::ios::binary);
    cam.render(world, out);
    std::cerr << "Wrote " << out_path << "\n";
    return 0;
}
