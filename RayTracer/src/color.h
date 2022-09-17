#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include "vec3.h"
#include "utility.h"

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // gamma correction with gamma=2
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

void write_color(std::vector<std::vector<color>> &outputBuffer, color pixel_color, int samples_per_pixel, int i, int j)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // gamma correction with gamma=2
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    outputBuffer[i][j].e[0] = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    outputBuffer[i][j].e[1] = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    outputBuffer[i][j].e[2] = static_cast<int>(256 * clamp(b, 0.0, 0.999));
}

#endif
