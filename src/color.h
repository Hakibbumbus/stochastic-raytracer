#pragma once
#include <iostream>
#include "interval.h"
#include "vec3.h"

inline void write_color(std::ostream& out, color pixel_color, int samples_per_pixel) {
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    // Divide by the number of samples and gamma-correct for gamma=2.0.
    double scale = 1.0 / samples_per_pixel;
    r = std::sqrt(scale * r);
    g = std::sqrt(scale * g);
    b = std::sqrt(scale * b);

    static const interval intensity(0.0, 0.999);
    int ir = static_cast<int>(256 * intensity.clamp(r));
    int ig = static_cast<int>(256 * intensity.clamp(g));
    int ib = static_cast<int>(256 * intensity.clamp(b));

    out << ir << ' ' << ig << ' ' << ib << '\n';
}
