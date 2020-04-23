#ifndef IMAGE_NOISE_FUNCTIONS_H
#define IMAGE_NOISE_FUNCTIONS_H

#include <cstdint>
#include <vector>
#include <algorithm>

// dlib includes
#include <dlib/matrix.h>
#include <dlib/algs.h>
#include <dlib/rand.h>
#include <dlib/image_transforms.h>


//-----------------------------------------------------------------------------
uint32_t get_random_poisson(double k, dlib::rand& rnd)
{
    uint32_t x = 0;
    double s = std::exp(-k);
    double p = s;
    double u = rnd.get_random_double();

    while (u > s)
    {
        ++x;
        p *= (k / (double)x);
        s += p;
    }

    return x;
}


//-----------------------------------------------------------------------------
//template<typename image_type>
//void add_poisson_noise(dlib::matrix<image_type> &src, dlib::matrix<image_type> &dst, double k, dlib::rand& rnd)
//{
//    uint32_t idx, r, c;
//    double n;
//
//    DLIB_CASSERT(src.size() == dst.size());
//
//    for (idx = 0; idx < src.size(); ++idx)
//    {
//        dst[idx].set_size(src[idx].nr(), src[idx].nc());
//
//        for (r = 0; r < src[idx].nr(); ++r)
//        {
//            for (c = 0; c < src[idx].nc(); ++c)
//            {
//                n = (double)get_random_poisson(k, rnd) - k;
//                dst[idx](r, c) = (uint16_t)dlib::max(dlib::min((double)src[idx](r, c) + n, 255.0), 0.0);
//            }
//        }
//    }
//
//}

//-----------------------------------------------------------------------------
template<typename T, typename array_type>
void apply_poisson_noise(
    array_type &img,
    double k, 
    dlib::rand& rnd,
    T lower_limit,
    T upper_limit 
)
{
    uint32_t idx;
    long r, c;
    double n;

    for (idx = 0; idx < img.size(); ++idx)
    {
        for (r = 0; r < img[idx].nr(); ++r)
        {
            for (c = 0; c < img[idx].nc(); ++c)
            {
                n = (double)get_random_poisson(k, rnd) - k;
                img[idx](r, c) = (uint16_t)std::max(std::min((double)img[idx](r, c) + n, upper_limit), lower_limit);
            }
        }
    }

}   // end of apply_poisson_noise

//-----------------------------------------------------------------------------

template<typename T, typename image_type>
void apply_uniform_noise(
    image_type& img,
    double std,
    dlib::rand& rnd,
    T lower_limit,
    T upper_limit
)
{
    uint32_t idx;
    long r, c;
    double v;

    for (idx = 0; idx < img.size(); ++idx)
    {
        for (r = 0; r < img[idx].nr(); ++r)
        {
            for (c = 0; c < img[idx].nc(); ++c)
            {
                v = rnd.get_integer_in_range(-std, std);
                img[idx](r, c) = dlib::put_in_range(lower_limit, upper_limit, img[idx](r, c) + v);
            }
        }

    }

}   // end of apply_uniform_noise

//-----------------------------------------------------------------------------
template<typename T, typename image_type>
void apply_gaussian_noise(

    image_type& img,
    double std,
    dlib::rand& rnd,
    T lower_limit,
    T upper_limit
)
{
    uint32_t idx;
    long r, c;
    double v;

    for (idx = 0; idx < img.size(); ++idx)
    {
        for (r = 0; r < img[idx].nr(); ++r)
        {
            for (c = 0; c < img[idx].nc(); ++c)
            {
                v = dlib::round(rnd.get_random_gaussian()*std);
                img[idx](r, c) = dlib::put_in_range(lower_limit, upper_limit, img[idx](r, c) + v);
            }
        }

    }

}   // end of apply_gaussian_noise

#endif	// IMAGE_NOISE_FUNCTIONS_H
