#pragma once
/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/


namespace ippmd
{
class AngCmp
{
private:
	static const signed   char*  orient_angle_diff_table;
	static const signed   short* direction_angle_diff_table;
	static const unsigned char*  direction_sector_table;
	static const signed   char*  direction_sector_shift_table;
public:
	static signed char  orient_diff(const unsigned char o1, const unsigned char o2);
	static signed short dir_diff   (const signed short  d1, const signed short  d2);

	static const signed short* dir_diff_table_ptr(const signed short d1);

	static unsigned char rotate_orient_by(const unsigned char o1, const unsigned char o2);
	static signed short  rotate_dir_by   (const signed short  d1, const signed short  d2);

	static signed short inv_and_rotate_dir_by(const signed short  d1, const signed short  d2);

	static unsigned char dir_sector_to    (const signed short d);
	static signed   char need_sector_shift(const signed short d);
};
}

