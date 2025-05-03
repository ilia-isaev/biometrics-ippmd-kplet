/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/

#include "ippmd_descriptor.hpp"
#include "ippmd_acmp.hpp"

#include <algorithm>

#include <cmath>
#include <cassert>


#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif


using namespace ippmd;


CorretctionVector::CorretctionVector(const unsigned char theta_180)
{
	const double rad_theta = (theta_180*2.0) *M_PI/180.0;
	const double rad_90    =  90         *M_PI/180.0;

	cos_theta_add_90 = cos(rad_theta)*cos(rad_90) - sin(rad_theta)*sin(rad_90);
	sin_theta_add_90 = sin(rad_theta)*cos(rad_90) + cos(rad_theta)*sin(rad_90);
}

XY
CorretctionVector::err_d2vec(const DistAngle da,  const char err_d_720) const
{
	// calc step length
	const double lerr  = sin((err_d_720 /2.0) *M_PI/180.0) * (da.v_dist/4.0);
	const double rad_a =     (da.v_angle/2.0) *M_PI/180.0;

	// get v1_angle ortogonal angle
	const double ort_sin = sin(rad_a)*cos_theta_add_90 + cos(rad_a)*sin_theta_add_90;
	const double ort_cos = cos(rad_a)*cos_theta_add_90 - sin(rad_a)*sin_theta_add_90;

	// calc step coordinate
	const short  dy = round( lerr * -ort_sin );
	const short  dx = round( lerr *  ort_cos );

	return {dx, dy};
}

void
Descriptor::distance_ordinate()
{
	std::sort(ev.begin(), ev.end(), [](const auto& a, const auto& b) {return a.v_dist < b.v_dist;});
}

void
Descriptor::fill_free_sector_and_angle_ordinate_from(const std::vector<DescriptorElement>& ev_full,
                                                     const short half_sector_720)
{
	assert(half_sector_720 >= 0 && half_sector_720 < 720);

	if (ev_full.empty())
		return;

	// check if destination descriptors, outside from half_sector_angle
	for(const auto& ef : ev_full)
		if (auto it = find_if(ev.begin(), ev.end(), [angle=ef.v_angle, half_angle=half_sector_720](const auto& e) {
					return abs(AngCmp::dir_diff(angle, e.v_angle)) <= half_angle;}); it == ev.end())
			ev.push_back(ef);

	std::sort(ev.begin(), ev.end(), [](const auto& a, const auto& b) {return a.v_angle < b.v_angle;});
}

DistAngle
Descriptor::dist_angle_by(int id) const
{
	for (const auto& e : ev)
		if (e.id == id)
			return {e.v_dist, e.v_angle};

	return {0,0};
}

short
Descriptor::dist_by(int id) const
{
	for (const auto& e : ev)
		if (e.id == id)
			return e.v_dist;
	return 0;
}

unsigned char
Descriptor::orient_by(int id) const
{
	for (const auto& e : ev)
		if (e.id == id)
			return e.v_orient;

	return 180;
}

bool
Descriptor::contain_id(const short id) const
{
	for (const auto& e : ev)
		if (e.id == id)
			return true;

	return false;
}

