#ifndef _RANDOM_COLOR_CHANNEL_SWAP_H_
#define _RANDOM_COLOR_CHANNEL_SWAP_H_

#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>


// dlib includes
#include <dlib/matrix.h>
#include <dlib/image_transforms.h>
#include <dlib/rand.h>


//-----------------------------------------------------------------------------
template<typename img_data_type, int array_depth>
void swap_channels(std::array<img_data_type, array_depth> &img, dlib::rand &rnd)
{
    DLIB_CASSERT(array_depth >= 3);

    uint64_t index = rnd.get_integer(5);

    switch (index)
    {
        case 0:
            dlib::swap(img[2], img[1]);
            break;
        case 1:
            dlib::swap(img[1], img[0]);
            break;
        case 2:
            dlib::swap(img[1], img[0]);
            dlib::swap(img[1], img[2]);
            break;
        case 3:
            dlib::swap(img[2], img[0]);
            dlib::swap(img[2], img[1]);
            break;
        case 4:
            dlib::swap(img[2], img[0]);
            break;
        default:
            break;
    }
}

//-----------------------------------------------------------------------------
template<typename pixel_type>
void swap_channels(dlib::matrix<pixel_type>& img, dlib::rand& rnd)
{
    uint64_t index = rnd.get_integer(6);

    long r, c;

    if ((dlib::pixel_traits<pixel_type>::rgb == true) && (dlib::pixel_traits<pixel_type>::has_alpha == false))
    {
        pixel_type p;

        switch (index)
        {
        case 0:
            for (r = 0; r < img.nr(); ++r)
            {
                for (c = 0; c < img.nc(); ++c)
                {
                    dlib::assign_pixel(p, img(r, c));
                    dlib::assign_pixel(img(r, c), pixel_type(p.red, p.green, p.blue));
                }
            }
            break;

        case 1:
            for (r = 0; r < img.nr(); ++r)
            {
                for (c = 0; c < img.nc(); ++c)
                {
                    dlib::assign_pixel(p, img(r, c));
                    dlib::assign_pixel(img(r, c), pixel_type(p.blue, p.red, p.green));
                }
            }
            break;

        case 2:
            for (r = 0; r < img.nr(); ++r)
            {
                for (c = 0; c < img.nc(); ++c)
                {
                    dlib::assign_pixel(p, img(r, c));
                    dlib::assign_pixel(img(r, c), pixel_type(p.green, p.blue, p.red));
                }
            }
            break;

        case 3:
            for (r = 0; r < img.nr(); ++r)
            {
                for (c = 0; c < img.nc(); ++c)
                {
                    dlib::assign_pixel(p, img(r, c));
                    dlib::assign_pixel(img(r, c), pixel_type(p.red, p.blue, p.green));
                }
            }
            break;

        case 4:
            for (r = 0; r < img.nr(); ++r)
            {
                for (c = 0; c < img.nc(); ++c)
                {
                    dlib::assign_pixel(p, img(r, c));
                    dlib::assign_pixel(img(r, c), pixel_type(p.green, p.red, p.blue));
                }
            }
            break;

        case 5:
            for (r = 0; r < img.nr(); ++r)
            {
                for (c = 0; c < img.nc(); ++c)
                {
                    dlib::assign_pixel(p, img(r, c));
                    dlib::assign_pixel(img(r, c), pixel_type(p.blue, p.green, p.red));
                }
            }
            break;

        default:
            break;
        }
    }

}


#endif  // _RANDOM_COLOR_CHANNEL_SWAP_H_
