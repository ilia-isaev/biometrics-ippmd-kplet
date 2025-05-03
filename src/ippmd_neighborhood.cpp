/*******************************************************
* Copyright (C) 2024-2025 Ilia Isaev <dev@isaev.com.br>
* This file is part of ippmd_xxx
* ippmd_xxx can not be copied and/or distributed without the express
* permission of Ilia Isaev
*******************************************************/

#include "ippmd_neighborhood.hpp"
#include "ippmd_acmp.hpp"
#include "ippmd_vector.hpp"

#include <cassert>


#define HALF_DIRECTION_SECTOR_720   24 // 12 grad
#define HALF_ORIENTATION_SECTOR_180  7 // 14 grad


//#define DEBUG

#ifdef DEBUG
#pragma message "DEBUG print neighborhood creation"
#include <iostream>
#define DEBUG_MSG(str) do { std::cerr << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do {  } while( false )
#endif


using namespace ippmd;


void
Neighborhood::load_minutia(int mnum, const std::function<MinutiaRaw(int idx)>& f)
{
	assert(mnum > 0);

	_raw.clear();

	for (int i = 0; i < mnum; ++i)
		_raw.push_back(f(i));
}

void
Neighborhood::create_descriptors()
{
	_dset     .clear();
	_dset_full.clear();

	auto apply_correction = [](const MinutiaRaw& m) -> Minutia {
		const short x = m.x+m.dx;
		const short y = m.y+m.dy;

		assert(m.theta >= 0 && m.theta < 180);

		const auto theta = m.dt ? AngCmp::rotate_orient_by((m.dt < 0 ? -m.dt : 180-m.dt), m.theta) : m.theta;

		assert(theta >= 0 && theta < 180); // <= ?

		return {theta, x, y, m.id};
	};

	// fill descriptor set with base minutia
	for (auto const& m : _raw)
	{
		const Descriptor nd = {apply_correction(m), {}};

		_dset     .push_back(nd);
		_dset_full.push_back(nd);
	}

	for (auto it_i = std::begin(_dset_full); it_i != std::end(_dset_full); ++it_i)
	{
		// descriptor base: id, orientation and coordinate 
		const auto   id_i    = it_i->mb.id;
		const auto   theta_i = it_i->mb.theta;
		const Vector base    = {it_i->mb.x, it_i->mb.y};

		for (auto it_n = it_i+1; it_n != std::end(_dset_full); ++it_n)
		{
			const auto vec = base.connection_to(it_n->mb.x, it_n->mb.y);
			if (vec.is_null())
				continue;

			const auto dist_x4 = vec.norm_x4();
			if (dist_x4 < 10) // minutia too close (< 2px)
				continue;

			const auto id_n    = it_n->mb.id;
			const auto theta_n = it_n->mb.theta;
			const auto dir_720 = vec.angle_720();

			// direct element i -> n
			it_i->ev.push_back({
				AngCmp::rotate_dir_by   (theta_i*4, dir_720), // angle    to minutia in this "view"
				AngCmp::rotate_orient_by(theta_i,   theta_n), // theta    of minutia in this "view"
				id_n,                                         // minutia index in template
				dist_x4                                       // distance to minutia in this "view"
				});

			// mirrored element n -> i
			it_n->ev.push_back({
				AngCmp::inv_and_rotate_dir_by(theta_n*4, dir_720), // angle    to minutia in this "view"
				AngCmp::rotate_orient_by     (theta_n,   theta_i), // theta    of minutia in this "view"
				id_i,                                              // minutia index in template
				dist_x4                                            // distance to minutia in this "view"
				});
		}
	}

	// in every full descriptor set near minutia fist
	for (auto& d : _dset_full)
		d.distance_ordinate();

	for (size_t i = 0; i < _dset_full.size(); ++i)
		_dset.at(i).fill_free_sector_and_angle_ordinate_from(_dset_full.at(i).ev, HALF_DIRECTION_SECTOR_720);
}

const Descriptor&
Neighborhood::descriptor(int idx) const
{
	assert(idx >= 0 && !_dset.empty() && (size_t)idx < _dset.size());

	return _dset.at(idx);
}

const std::vector<Descriptor>&
Neighborhood::descriptor_set() const
{
	return _dset;
}

