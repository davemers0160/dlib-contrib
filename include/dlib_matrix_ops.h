#ifndef _DLIB_MATRIX_OPERATIONS_H
#define _DLIB_MATRIX_OPERATIONS_H

#include <cstdint>
#include <vector>
#include <algorithm>
#include <ostream>
#include <iomanip>

#include "dlib/matrix.h"

// ----------------------------------------------------------------------------
// template<typename T>
// void print_matrix_as_csv(T &m, uint32_t w)
// {

    // for (long r = 0; r < m.nr(); ++r)
    // {
        // for (long c = 0; c < m.nc(); ++c)
        // {
            // if (c + 1 == m.nc())
                // std::cout << std::setw(w) << m(r, c) << std::endl;
            // else
                // std::cout << std::setw(w) << m(r, c) << ", ";
        // }
    // }
// }   // end of print_matrix_as_csv


template <typename T>
struct fixed_width_mat
{
    fixed_width_mat(T m_, int w_) : m(m_), w(w_) {}
    T m;
    int w;
};

template<typename T>
//void print_matrix_as_csv(T &m, uint32_t w)
std::ostream& operator<< (std::ostream& out, const fixed_width_mat<T>& fmw)
{

    for (long r = 0; r < fmw.m.nr(); ++r)
    {
        for (long c = 0; c < fmw.m.nc(); ++c)
        {
            if (c + 1 == fmw.m.nc())
                out << std::setw(fmw.w) << fmw.m(r, c) << std::endl;
            else
                out << std::setw(fmw.w) << fmw.m(r, c) << ", ";
        }
    }

    return out;
}   // end of ostream operator



#endif  // _DLIB_MATRIX_OPERATIONS_H
