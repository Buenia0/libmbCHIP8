/*
    This file is part of libmbCHIP8.
    Copyright (C) 2022 BueniaDev.

    libmbCHIP8 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libmbCHIP8 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libmbCHIP8.  If not, see <https://www.gnu.org/licenses/>.
*/

    
#ifndef LIBMBCHIP8_API_H
#define LIBMBCHIP8_API_H

#if defined(_MSC_VER) && !defined(LIBMBCHIP8_STATIC)
    #ifdef LIBMBCHIP8_EXPORTS
        #define LIBMBCHIP8_API __declspec(dllexport)
    #else
        #define LIBMBCHIP8_API __declspec(dllimport)
    #endif // LIBMBCHIP8_EXPORTS
#else
    #define LIBMBCHIP8_API
#endif // _MSC_VER

#endif // LIBMBCHIP8_API_H
