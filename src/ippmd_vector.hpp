#pragma once
/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/


namespace ippmd
{
class Vector
{
public:
	short x;
	short y;

	Vector connection_to(short x, short y) const;

	bool is_null() const;

	short norm_x4()   const;
	short angle_720() const;
};
}

