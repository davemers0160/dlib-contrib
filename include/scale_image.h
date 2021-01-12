#ifndef _DLIB_SCALE_IMAGE_H_
#define _DLIB_SCALE_IMAGE_H_


#include <cstdint>
#include <vector>
#include <algorithm>
#include <array>

#include "dlib/matrix.h"
#include "dlib/pixel.h"
#include "dlib/image_io.h"
#include "dlib/rand.h"


// ----------------------------------------------------------------------------
template<typename T, int array_depth>
void random_scale_intensity(
    std::array<dlib::matrix<T>, array_depth> &img,
    dlib::rand& rnd,
    T lower_limit,
    T upper_limit
)
{
    uint32_t idx;

    uint32_t intensity = (uint32_t)rnd.get_integer_in_range((uint32_t)(lower_limit * 10), (uint32_t)(upper_limit * 10) + 1);
    //double v = std::min(rnd.get_double_in_range(lower_limit, upper_limit + std::numeric_limits<double>::epsilon()), upper_limit);
    double v = (double)intensity / 10.0;

    for (idx = 0; idx < img.size(); ++idx)
    {
        img[idx] = dlib::matrix_cast<T>(dlib::matrix_cast<double>(img[idx]) * v);
    }
}   // end of scale_intensity 


// ----------------------------------------------------------------------------
template<typename T, typename pixel_type>
void random_scale_intensity(
    dlib::matrix<pixel_type> &img,
    dlib::rand& rnd,
    T lower_limit,
    T upper_limit)
{
    uint32_t idx;
    uint32_t intensity = (uint32_t)rnd.get_integer_in_range((uint32_t)(lower_limit * 10), (uint32_t)(upper_limit * 10) + 1);
    //double v = std::min(rnd.get_double_in_range(lower_limit, upper_limit + std::numeric_limits<double>::epsilon()), upper_limit);
    double v = (double)intensity / 10.0;

    //for (idx = 0; idx < img.size(); ++idx)
    //{
    //    img[idx] = dlib::matrix_cast<uint16_t>(dlib::matrix_cast<double>(img[idx]) * v);
    //}
}   // end of scale_intensity 

// ----------------------------------------------------------------------------
template<typename T, int array_depth>
void scale_intensity(
    std::array<dlib::matrix<T>, array_depth>& img,
    double scale
)
{
    uint32_t idx;

    for (idx = 0; idx < array_depth; ++idx)
    {
        img[idx] = dlib::matrix_cast<T>(dlib::matrix_cast<double>(img[idx]) * scale);
    }
}   // end of scale_intensity 

// ----------------------------------------------------------------------------
template<typename T, typename pixel_type>
void scale_intensity(
    dlib::matrix<pixel_type>& img,
    double scale
)
{
    uint32_t idx;

    //for (idx = 0; idx < array_depth; ++idx)
    //{
    //    img[idx] = dlib::matrix_cast<T>(dlib::matrix_cast<double>(img[idx]) * scale);
    //}
}   // end of scale_intensity 

#endif	// _DLIB_SCALE_IMAGE_H_
