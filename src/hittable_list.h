#pragma once
#include <vector>
#include <memory>
#include "hittable.h"
#include "interval.h"

class hittable_list : public hittable {
public:
    std::vector<std::unique_ptr<hittable>> objects;

    void clear() { objects.clear(); }
    void add(std::unique_ptr<hittable> obj) { objects.emplace_back(std::move(obj)); }

    bool hit(const ray& r, interval t_range, hit_record& rec) const override {
        hit_record temp;
        bool hit_anything = false;
        double closest = t_range.max;

        for (const auto& obj : objects) {
            if (obj->hit(r, interval(t_range.min, closest), temp)) {
                hit_anything = true;
                closest = temp.t;
                rec = temp;
            }
        }
        return hit_anything;
    }
};
