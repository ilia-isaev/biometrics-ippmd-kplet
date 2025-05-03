/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/

#include "ippmd_print.hpp"


namespace ippmd
{
std::ostream& operator<< (std::ostream& out, const DescriptorElement& e)
{
	return out << Print::fmt_str(std::string("[%5.1f %5.1f %3i  %3i]"),
		e.v_angle /2.f, // angle  0 - 720
		e.v_dist  /4.f, // dist  x4
		e.v_orient*2,   // orient 0 - 180
		e.id+1
		);
}

std::ostream& operator<< (std::ostream& out, const Descriptor& d)
{
	return out << Print::fmt_str(std::string("[%3i %3li]"),
		d.mb.id+1,
		d.ev.size()
		);
}

}

