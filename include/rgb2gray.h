#ifndef RGB2GRAY_INCLUDE_
#define RGB2GRAY_INCLUDE_

#include <dlib/pixel.h>

// 0.299 * R + 0.587 * G + 0.114 * B
namespace dlib
{
    template <typename color_pixel, typename gray_pixel>
    void c2g(color_pixel &p, gray_pixel &g)
    {
        g = (gray_pixel)(0.299*(double)p.red + 0.587*(double)p.green + 0.114*(double)p.blue);
    }   // end c2g

    template <typename gray_pixel>
    void rgb2gray(dlib::matrix<dlib::rgb_pixel> color_img, dlib::matrix<gray_pixel> &gray_img)
    {
        gray_img.set_size(color_img.nr(), color_img.nc());

        for (long r = 0; r < color_img.nr(); ++r)
        {
            for (long c = 0; c < color_img.nc(); ++c)
            {
                c2g(color_img(r, c), gray_img(r, c));
            }
        }
    }   // end of rgb2gray

    template <typename gray_pixel>
    void bgr2gray(dlib::matrix<dlib::bgr_pixel> color_img, dlib::matrix<gray_pixel> &gray_img)
    {
        gray_img.set_size(color_img.nr(), color_img.nc());

        for (long r = 0; r < color_img.nr(); ++r)
        {
            for (long c = 0; c < color_img.nc(); ++c)
            {
                c2g(color_img(r, c), gray_img(r, c));
            }
        }
    }   // end of rgb2gray

}   // end of namespace

#endif // RGB2GRAY_INCLUDE_