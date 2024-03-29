#ifndef _DLIB_ARRAY_IMAGE_CONVERSIONS_H
#define _DLIB_ARRAY_IMAGE_CONVERSIONS_H


#include <cstdint>
#include <vector>
#include <algorithm>
#include <array>

#include "dlib/matrix.h"
#include "dlib/pixel.h"
#include "dlib/image_io.h"
#include "dlib/rand.h"

// ----------------------------------------------------------------------------
template<typename T, size_t img_depth>
void scale_intensity(std::array<dlib::matrix<T>, img_depth>& img,
    dlib::rand& rnd,
    double lower_limit,
    double upper_limit)
{
    uint32_t idx;
    uint32_t intensity = (uint32_t)rnd.get_integer_in_range((uint32_t)(lower_limit *10), (uint32_t)(upper_limit*10)+1);
    //double v = std::min(rnd.get_double_in_range(lower_limit, upper_limit + std::numeric_limits<double>::epsilon()), upper_limit);
    double v = (double)intensity / 10.0;

    for (idx = 0; idx < img.size(); ++idx)
    {
        img[idx] = dlib::matrix_cast<T>(dlib::matrix_cast<double>(img[idx]) * v);
    }
}   // end of scale_intensity 

// ----------------------------------------------------------------------------------------
template<typename array_type>
void merge_channels(array_type &a_img, dlib::matrix<dlib::rgb_pixel> &img, uint64_t index=0)
{
    uint64_t r, c;
    uint32_t channels = dlib::pixel_traits<dlib::rgb_pixel>::num;

    DLIB_CASSERT(a_img.size() >= (channels + index), "Array image does not contain enough channels: "
        << "Array Size: " << a_img.size() << "; index + channels: " << (channels + index));

    uint64_t rows = a_img[0].nr();
    uint64_t cols = a_img[0].nc();

    img.set_size(rows, cols);

    for (r = 0; r < rows; ++r)
    {
        for (c = 0; c < cols; ++c)
        {
            dlib::rgb_pixel p((uint8_t)(a_img[index+0](r, c)), (uint8_t)(a_img[index+1](r, c)), (uint8_t)(a_img[index+2](r, c)));
            dlib::assign_pixel(img(r, c),p);
        }
    }

}   // end of merge_channels

// ----------------------------------------------------------------------------------------
template<typename array_type>
void split_channels(array_type &img_a, dlib::matrix<dlib::rgb_pixel> &img, uint64_t index=0)
{
    uint64_t r, c;
    uint32_t channels = dlib::pixel_traits<dlib::rgb_pixel>::num;

    // get the size of the image
    uint64_t rows = img.nr();
    uint64_t cols = img.nc();

    DLIB_CASSERT(img_a.size() >= (channels+index), "Array image does not contain enough channels:"
        << "Array Size: " << img_a.size() << "; index + channels: " << (channels + index));

    // set the size of the image
    img_a[index + 0].set_size(rows, cols);
    img_a[index + 1].set_size(rows, cols);
    img_a[index + 2].set_size(rows, cols);

    for (r = 0; r < rows; ++r)
    {
        for (c = 0; c < cols; ++c)
        {
            dlib::rgb_pixel p;
            dlib::assign_pixel(p, img(r, c));
            dlib::assign_pixel(img_a[index+0](r, c), p.red);
            dlib::assign_pixel(img_a[index+1](r, c), p.green);
            dlib::assign_pixel(img_a[index+2](r, c), p.blue);
        }
    }

}   // end of split_channels

// ----------------------------------------------------------------------------------------

#endif  // _DLIB_ARRAY_IMAGE_CONVERSIONS_H
