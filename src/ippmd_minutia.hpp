#pragma once
/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/


namespace ippmd
{

class MinutiaRaw
{
public:
	unsigned char theta;
	short         x;
	short         y;

	short         id;

	short         dx;
	short         dy;
	char          dt;
};

class MPair
{
public:
	short id1;
	short id2;

	inline bool operator < (const MPair& rhs) const {
		if (id1 == rhs.id1)
			return id2 < rhs.id2;
		return id1 < rhs.id1;
	}

	inline bool operator == (const MPair& mp) const {
		return id1 == mp.id1 && id2 == mp.id2;
	}
};

class XY
{
public:
	short x;
	short y;
};

class idThetaXY
{
public:
	short id;
	char  d_theta;
	XY    d_xy;
};

class Minutia
{
public:
	unsigned char theta;
	short         x;
	short         y;

	short         id;
};

}

