#pragma once
/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/


#include "ippmd_descriptor.hpp"

#include <string>
#include <memory>
#include <ostream>


namespace ippmd
{

class Print
{
public:
	template<typename... Args>
	static std::string fmt_str(const std::string& format, Args... args)
	{
		const auto size   = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
		const auto buffer = std::make_unique<char[]>(size);

		std::snprintf(buffer.get(), size, format.c_str(), args...);

		return std::string(buffer.get(), buffer.get() + size - 1);
	}

};

std::ostream& operator<< (std::ostream& out, const DescriptorElement& e);
std::ostream& operator<< (std::ostream& out, const Descriptor& d);

}

