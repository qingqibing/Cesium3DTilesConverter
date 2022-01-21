#pragma once

#include <osg/Image>
#include <vector>

struct Color {
    int r;
    int g;
    int b;
};

void resizeImage(std::vector<unsigned char>& jpeg_buf, int width, int height, int new_w, int new_h);
void fill4BitImage(std::vector<unsigned char>& jpeg_buf, osg::Image* img, int& width, int& height);