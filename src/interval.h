#pragma once
struct interval {
    double min, max;
    interval(): min(+INFINITY), max(-INFINITY) {}
    interval(double a, double b): min(a), max(b) {}
    bool contains(double x) const { return min <= x && x <= max; }
    bool surrounds(double x) const { return min < x && x < max; }
    double clamp(double x) const { if (x<min) return min; if (x>max) return max; return x; }

    static const interval empty, universe;
};
inline const interval interval::empty = interval(+INFINITY, -INFINITY);
inline const interval interval::universe = interval(-INFINITY, +INFINITY);
