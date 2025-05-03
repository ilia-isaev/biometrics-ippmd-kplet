#pragma once
/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/

#include "ippmd_minutia.hpp"

#include <vector>


namespace ippmd
{

class DistAngle
{
public:
	short v_dist;
	short v_angle;
};

class CorretctionVector
{
private:
	double cos_theta_add_90;
	double sin_theta_add_90;
public:
	CorretctionVector(const unsigned char theta_180);

	XY err_d2vec(const DistAngle da,  const char err_d_720) const;
};

class IntersectionElement
{
public:
	short id1;
	short id2;
	char  err_d; /**< dirrection  angle error (diff v_angle)  */
	char  err_o; /**< orientation angle error (diff v_orient) */
};

class DescriptorElement
{
public:
	short         v_angle;
	unsigned char v_orient;
	short         id;
	short         v_dist;
};

class Descriptor
{
public:
	Minutia                        mb;
	std::vector<DescriptorElement> ev;

	void distance_ordinate();

	void fill_free_sector_and_angle_ordinate_from(const std::vector<DescriptorElement>& ev_full,
	                                                                const short half_sector_720);

	DistAngle     dist_angle_by(int id) const;
	short         dist_by      (int id) const;
	unsigned char orient_by    (int id) const;

	bool contain_id(const short id) const;
};

}

