#ifndef CONNECTED_COMPONENTS_H
#define CONNECTED_COMPONENTS_H

#include <cstdint>
#include <list>

#include <dlib/matrix.h>
#include <dlib/pixel.h>
#include <dlib/geometry.h>

// ----------------------------------------------------------------------------------------

template<typename pixel_type>
void connected_neighbors(dlib::point s, pixel_type V, dlib::matrix<pixel_type> &img, uint32_t &pix_count, std::vector<dlib::point> &c)
{
    pix_count = 0;

    if (s.y() - 1 >= 0)
    {
        if (abs(img(s.y() - 1, s.x()) - img(s.y(), s.x())) == V)
        {
            c[pix_count].y() = s.y() - 1;
            c[pix_count].x() = s.x();
            ++pix_count;
        }
    }

    if (s.y() + 1 < img.nr())
    {
        if (abs(img(s.y() + 1, s.x()) - img(s.y(), s.x())) == V)
        {
            c[pix_count].y() = s.y() + 1;
            c[pix_count].x() = s.x();
            ++pix_count;
        }
    }

    if (s.x() - 1 >= 0)
    {
        if (abs(img(s.y(), s.x() - 1) - img(s.y(), s.x())) == V)
        {
            c[pix_count].y() = s.y();
            c[pix_count].x() = s.x() - 1;
            ++pix_count;
        }
    }

    if (s.x() + 1 < img.nc())
    {
        if (abs(img(s.y(), s.x() + 1) - img(s.y(), s.x())) == V)
        {
            c[pix_count].y() = s.y();
            c[pix_count].x() = s.x() + 1;
            ++pix_count;
        }
    }

}    // end of connected_neighbors


// ----------------------------------------------------------------------------------------

template<typename pixel_type>
void connected_set(dlib::point s, dlib::matrix<pixel_type> &img, pixel_type class_label, pixel_type T)
{
    uint32_t idx, jdx;
    uint32_t pixel_count = 0;
    uint32_t num_con_pixels;
    std::vector<dlib::point> c(4);
    
    dlib::matrix<uint8_t> s_img = dlib::zeros_matrix<uint8_t>(img.nr(), img.nc());

    std::list<dlib::point> connected_nodes;

    // initialize the first node in the linked list and set it to the seed pixel
    connected_nodes.push_back(s);
    num_con_pixels = 1;

    // create the array that will track the status of the searching for connected pixels
    // if B[i][j] = -1 <= searched and not connected
    // if B[i][j] = 1 <= searched and connected
    // if B[i][j] = 0 <= not yet searched
    //B = (char **)get_img(width, height, sizeof(char));
    //memset(*B, 0, width*height);
    //B[s.y][s.x] = 255;
    //seg[s.y][s.x] = class_label;
    s_img(s.y(), s.x()) = 1;
    img(s.y(), s.x()) = class_label;
    // @mem(*B,UINT8,1,width,height,width)

    connected_neighbors(s, T, img, pixel_count, c);
    for (idx = 0; idx < pixel_count; idx++)
    {
        if (s_img(c[idx].y(), c[idx].x()) == 0)
        {
            s_img(c[idx].y(), c[idx].x()) = 1;
            img(c[idx].y(), c[idx].x()) = class_label;
			
			// add a new node
            connected_nodes.push_back(c[idx]);

            ++num_con_pixels;
        }
    }

    // remove the initial node which is the s passed into the function
    //ConnectedNodes = deleteNode(ConnectedNodes);
    connected_nodes.pop_front();
    
    //pix_count += M;

    //while (pix_count > 0)
    while (connected_nodes.size() > 0)
    {
		connected_neighbors(*connected_nodes.begin(), T, img, pixel_count, c);
		
        for (idx = 0; idx < pixel_count; idx++)
        {
            if (s_img(c[idx].y(), c[idx].x()) == 0)
            {
                s_img(c[idx].y(), c[idx].x()) = 1;
                img(c[idx].y(), c[idx].x()) = class_label;
			
			    // add a new node
                connected_nodes.push_back(c[idx]);

                ++num_con_pixels;
            }
        }

        // remove the node that was just checked
        connected_nodes.pop_front();

    }

}    // end of connected_set


#endif    // CONNECTED_COMPONENTS_H
