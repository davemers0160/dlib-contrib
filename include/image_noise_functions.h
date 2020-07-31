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
template<typename T, typename pixel_type>
void apply_poisson_noise(
    dlib::matrix<pixel_type>& img,
    double k,
    dlib::rand& rnd,
    T lower_limit,
    T upper_limit
)
{
    long r, c;
    double n1, n2, n3;


    if ((dlib::pixel_traits<pixel_type>::grayscale == true))
    {
        T p;

        for (r = 0; r < img.nr(); ++r)
        {
            for (c = 0; c < img.nc(); ++c)
            {
                dlib::assign_pixel(p, img(r, c));

                n1 = (double)(p) + (double)get_random_poisson(k, rnd) - k;
                p = static_cast<T>(std::max(std::min(n1, (double)upper_limit), (double)lower_limit));

                dlib::assign_pixel(img(r, c), p);
            }
        }
    }
    else if ((dlib::pixel_traits<pixel_type>::rgb == true) && (dlib::pixel_traits<pixel_type>::has_alpha == false))
    {
        pixel_type p;

        for (r = 0; r < img.nr(); ++r)
        {
            for (c = 0; c < img.nc(); ++c)
            {
                dlib::assign_pixel(p, img(r, c));
                n1 = (double)p.red + (double)get_random_poisson(k, rnd) - k;
                n2 = (double)p.blue + (double)get_random_poisson(k, rnd) - k;
                n3 = (double)p.green + (double)get_random_poisson(k, rnd) - k;

                p.red = (T)(std::max(std::min(n1, (double)upper_limit), (double)lower_limit));
                p.blue = (T)(std::max(std::min(n2, (double)upper_limit), (double)lower_limit));
                p.green = (T)(std::max(std::min(n3, (double)upper_limit), (double)lower_limit));

                dlib::assign_pixel(img(r, c), p);
            }
        }
    }

}   // end of apply_poisson_noise

//-----------------------------------------------------------------------------
template<typename T, uint32_t array_depth>
void apply_poisson_noise(
    std::array<dlib::matrix<T>, array_depth>& img,
    double k,
    dlib::rand& rnd,
    double lower_limit,
    double upper_limit
)
{
    uint32_t idx;
    long r, c;
    double n;

    for (idx = 0; idx < array_depth; ++idx)
    {
        for (r = 0; r < img[idx].nr(); ++r)
        {
            for (c = 0; c < img[idx].nc(); ++c)
            {
                n = (double)get_random_poisson(k, rnd) - k;
                img[idx](r, c) = (T)(std::max(std::min((double)img[idx](r, c) + n, upper_limit), lower_limit));
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
                v = (T)(rnd.get_random_gaussian()*std);
                img[idx](r, c) = dlib::put_in_range(lower_limit, upper_limit, img[idx](r, c) + v);
            }
        }

    }

}   // end of apply_gaussian_noise

#endif	// IMAGE_NOISE_FUNCTIONS_H
