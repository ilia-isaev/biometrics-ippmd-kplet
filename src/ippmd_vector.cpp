/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/

#include "ippmd_vector.hpp"

#include <cmath>
#include <cassert>


#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif


using namespace ippmd;


Vector
Vector::connection_to(short mx, short my) const
{
	const short dx =  mx-x;
	const short dy = -my+y;

	return {dx, dy};
}

bool
Vector::is_null() const
{
	return x == 0 && y == 0;
}

short
Vector::norm_x4() const
{
	return roundf( sqrtf( powf( x,2 )+pow( y,2 ) ) * 4.f );
}

short
Vector::angle_720() const
{
	const float PI = static_cast<float>(M_PI);

	const float angl  = atan2f(y, x);

	const short angle = roundf( angl * 360.f / PI );

	//const float angle = angl < 0 ? (2*PI+angl)*360.f/PI : fabsf(angl)*360.f/PI;

	const short a720 = angle < 0 ? 720+angle : angle;

	assert(a720 >= 0 && a720 < 720);

	return a720;
}

