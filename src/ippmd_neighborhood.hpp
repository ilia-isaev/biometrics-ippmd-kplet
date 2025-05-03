#pragma once
/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/


#include "ippmd_minutia.hpp"
#include "ippmd_descriptor.hpp"

#include <vector>
#include <functional>


namespace ippmd
{
class Neighborhood
{
private:
	std::vector<MinutiaRaw> _raw;
	std::vector<Descriptor> _dset;
	std::vector<Descriptor> _dset_full;
public:
	void load_minutia(int mnum, const std::function<MinutiaRaw(int idx)>& f);

	void create_descriptors();

	const             Descriptor & descriptor    (int idx) const;
	const std::vector<Descriptor>& descriptor_set()        const;
};
}

