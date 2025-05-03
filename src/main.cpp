#include "../config.h"

#include "kplet.hpp"

#include "ippmd_neighborhood.hpp"
#include "ippmd_print.hpp"

#include <iostream>
#include <fstream>

#include <cstdio>
#include <cstring>

int
check_file_extension(const char* fileext, const char* extension)
{
    return strncmp(fileext, extension, 3);
}

int
incits378_minutiae_block(unsigned char **minutiae_block, void *buffer_in)
{
	unsigned char *buffer = (unsigned char*)buffer_in;

	const int len_short = (buffer[8]<<8)+buffer[9];

	if (len_short == 32)
		return 0;

	const int add_4_byte = len_short == 0 ? 4 : 0;

	unsigned char number_finger_views = buffer[24+add_4_byte];
	if (number_finger_views == 0)
		return 0;

	*minutiae_block = buffer+30+add_4_byte;

	return buffer[29+add_4_byte];
}

ippmd::Neighborhood
createAppmdNeighborhoodFrom(const char* name)
{
	std::ifstream ifbuff(name);

	ifbuff.seekg(0, ifbuff.end);
	size_t length = ifbuff.tellg();
	ifbuff.seekg(0, ifbuff.beg);

	std::vector<unsigned char> buf(length);

	ifbuff.read((char*)&buf[0], length);

	unsigned char *mblock = NULL;

	int mnum   = incits378_minutiae_block(&mblock, &buf[0]);

	if (mnum > MAX_STAR_MINUTIAE_AMOUNT)
		mnum = MAX_STAR_MINUTIAE_AMOUNT;

	ippmd::Neighborhood neib;

	neib.load_minutia(mnum, [=](int idx) -> ippmd::MinutiaRaw {
			int ipos = idx*6;
			const short         x     = ((mblock[ipos  ] & 0x3F) << 8) + mblock[ipos+1];
			const short         y     = ((mblock[ipos+2] & 0x3F) << 8) + mblock[ipos+3];
			const unsigned char theta = mblock[ipos+4];

			return {theta, x, y, (short)idx};
		});

	neib.create_descriptors();

	return neib;
}

// --- Convert ippmd descriptor to kplet ---
RAY_ELEMENT
ippmd2re(ippmd::DescriptorElement e)
{
	return (RAY_ELEMENT){
		.neighbour_minutiae=          e.id,
		.rel_dist          =(int32_t)(e.v_dist /4.f),
		.rel_angle         =(int32_t)(e.v_angle/2.f),
		.rel_theta         =          e.v_orient*2
	};
}

std::vector<MINUTIA_ELEMENT>
neib2me_vec(const ippmd::Neighborhood &neib, const unsigned int num_limit)
{
	std::vector<MINUTIA_ELEMENT> me_vec;

	for (const auto &desc : neib.descriptor_set())
	{
		std::vector<ippmd::DescriptorElement> de_q1;
		std::vector<ippmd::DescriptorElement> de_q2;
		std::vector<ippmd::DescriptorElement> de_q3;
		std::vector<ippmd::DescriptorElement> de_q4;

		for (const auto &de : desc.ev)
		{
			if (     de.v_angle/2.f >=   0 && de.v_angle/2.f <  90) // angle 0 - 720
				de_q1.push_back(de);
			else if (de.v_angle/2.f >=  90 && de.v_angle/2.f < 180) // angle 0 - 720
				de_q2.push_back(de);
			else if (de.v_angle/2.f >= 180 && de.v_angle/2.f < 270) // angle 0 - 720
				de_q3.push_back(de);
			else if (de.v_angle/2.f >= 270 && de.v_angle/2.f < 360) // angle 0 - 720
				de_q4.push_back(de);
		}

		std::sort(de_q1.begin(), de_q1.end(), [](const auto& a, const auto& b) {return a.v_dist > b.v_dist;});
		std::sort(de_q2.begin(), de_q2.end(), [](const auto& a, const auto& b) {return a.v_dist > b.v_dist;});
		std::sort(de_q3.begin(), de_q3.end(), [](const auto& a, const auto& b) {return a.v_dist > b.v_dist;});
		std::sort(de_q4.begin(), de_q4.end(), [](const auto& a, const auto& b) {return a.v_dist > b.v_dist;});

		// create ray elemnt
		std::vector<RAY_ELEMENT> re;

		for (size_t i = 0, quadrant_sel = 1; i < desc.ev.size() && re.size() < MAX_RAY_AMOUNT; ++i)
		{
			if (quadrant_sel == 1)
			{
				quadrant_sel = 2;
				if (!de_q1.empty())
				{
					re.push_back(ippmd2re(de_q1.back()));
					de_q1.pop_back();
					continue;
				}
			}
			if (quadrant_sel == 2)
			{
				quadrant_sel = 3;
				if (!de_q2.empty())
				{
					re.push_back(ippmd2re(de_q2.back()));
					de_q2.pop_back();
					continue;
				}
			}
			if (quadrant_sel == 3)
			{
				quadrant_sel = 4;
				if (!de_q3.empty())
				{
					re.push_back(ippmd2re(de_q3.back()));
					de_q3.pop_back();
					continue;
				}
			}
			if (quadrant_sel == 4)
			{
				quadrant_sel = 1;
				if (!de_q4.empty())
				{
					re.push_back(ippmd2re(de_q4.back()));
					de_q4.pop_back();
					continue;
				}
			}
		}

		if (!re.empty() && me_vec.size() < num_limit)
		{
			MINUTIA_ELEMENT me = {.ray_amount=(uint32_t)re.size()};

			std::memcpy(me.rays, &(re[0]), sizeof(RAY_ELEMENT) * re.size());

			me_vec.push_back(me);
		}
	}

	return me_vec;
}

// --- printing ---
typedef struct {
	int from; // index from to start sequencia print
	int to;   // last index to print in sequencia
	int num;  // number of print element in line
} print_range_s;

print_range_s
get_print_range(int *param_array, const int param_sz, const int sz)
{
	const int from = param_sz > 0 ? param_array[0]-1 : 0;
	const int to   = param_sz > 1 ? param_array[1]-1 : sz-1;
	const int num  = param_sz > 2 ? param_array[2]   : 4;

	return (print_range_s){.from=from, .to=(to>=sz ? sz-1 : to), .num=num};
}

void
print_neighborhood(const ippmd::Neighborhood& neib, print_range_s range, int mnum)
{
	for (int i = range.from; i < range.to+1; ++i)
	{
		int    n = 0;
		size_t k = 0;
		std::cout << neib.descriptor(i) << "  ";
		for (auto e : neib.descriptor(i).ev)
		{
			std::cout << e << " ";
			if (++n >= range.num && k != neib.descriptor(i).ev.size()-1)
			{
				std::cout << std::endl << "           ";
				n = 0;
			}
			k++;
		}
		std::cout << std::endl;
		if (i == range.to)
			std::cout << std::endl;
	}
}

void
print_kplet_minutia_element(std::vector<MINUTIA_ELEMENT> me_vec, print_range_s range, int mnum)
{
	for (int i = range.from; i < range.to+1; ++i)
	{
		int    n = 0;
		size_t k = 0;

		std::cout << ippmd::Print::fmt_str("[%3i %3i]  ", i+1, me_vec[i].ray_amount);

		//for (auto e : me_vec[i].rays)
		for (unsigned int e = 0; e < me_vec[i].ray_amount; ++e)
		{
			const auto r = me_vec[i].rays[e];

			std::cout << ippmd::Print::fmt_str("[%3i %5.1f %3i %3i] ",
				r.rel_angle, r.rel_dist/1.f, r.rel_theta, r.neighbour_minutiae+1);

			if (++n >= range.num && k != me_vec[i].ray_amount-1)
			{
				std::cout << std::endl << "           ";
				n = 0;
			}
			k++;
		}
		std::cout << std::endl;
		if (i == range.to)
			std::cout << std::endl;
	}
}

int main(int argc, char* argv[])
{
	int help_opt = 0;
	int vers_opt = 0;
	int vrbs_opt = 0;

	int neib_opt = 0;
	int kplt_opt = 0;

    int   cnt_tag   = 0;
	int   tmpl_cnt  = 0;
	int   param_cnt = 0;

	int   param[argc];
	char *file_tmpl[argc];
    char *tag_list[argc];

	/* Promgramm option check */
	for (int i = 1; i < argc; ++i)
	{
		const size_t len = strlen (argv[i]);
		if (argv[i][0] == '-' && atoi(argv[i]+1) == 0)
		{
			for (size_t n = 1; n < len; ++n)
			{
				const char opt = argv[i][n];
				if ('V' == opt)
					vrbs_opt = 1;
				else if ('v' == opt)
					vers_opt = 1;
				else if ('h' == opt)
					help_opt = 1;
				else if ('k' == opt)
					kplt_opt = 1;
				else if ('n' == opt)
					neib_opt = 1;
				else if (' ' == opt || '-' == opt)
					continue;
				else
				{
					fprintf(stdout, "Bad option:'%s'", argv[i]);
					return 1;
				}
			}
		} else {
			const int prm_opt = neib_opt || kplt_opt; // || insc_opt || adjc_opt || Adjc_opt || crot_opt || cpos_opt;

			if (check_file_extension(argv[i] + len - 3, "378") == 0) {
				file_tmpl[tmpl_cnt++] = argv[i];
			}
			else if (prm_opt)
				param[param_cnt++] = atoi(argv[i]);
			else if (!prm_opt) {
				tag_list[cnt_tag++] = argv[i];
			}
		}
	}

	/* Standart imformation output */ 
	if (help_opt)
		fprintf (stdout,
			"Usage: %s [-Vh] t1.incits378 t2.incits378 ...\n"
			"Usage: %s [-Vh] t1.incits378 t2.incits378 tag \n"
			"Isaev kplet/ippmd test tool\n"
			"Test ippm/kplet descriptor criation\n"
			"   -n [from] [to]   neigborhood ippmd descriptor printing\n"
			"   -v               version print\n"
			"   -V               verbose output\n"
			"   -h               show this help\n"
			, PACKAGE_NAME, PACKAGE_NAME);
	if (vers_opt)
		fprintf (stdout,
			"   %s tool\n"
			"version:  %s\n"
			"date:     12 Set 2024\n", PACKAGE_NAME, PACKAGE_VERSION);
	if (vers_opt || help_opt)
		return 0;

	if (vrbs_opt)
	{
		for (int i = 0; i < param_cnt; ++i)
			std::cout << "prm:" << i+1 << " is:" <<  param[i] << std::endl;
	}

	if (cnt_tag)
	{
		for (int i = 0, tag_idx = 0; i < tmpl_cnt; i+=2, ++tag_idx)
		{
			const char *name_p = file_tmpl[i];
			const char *name_g = file_tmpl[i+1];

			const char *tag = cnt_tag > tag_idx ? tag_list[tag_idx] : "x";

			auto neib_p = createAppmdNeighborhoodFrom(name_p);
			auto neib_g = createAppmdNeighborhoodFrom(name_g);

			auto mev_p = neib2me_vec(neib_p, MAX_STAR_MINUTIAE_AMOUNT);
			auto mev_g = neib2me_vec(neib_g, MAX_STAR_MINUTIAE_AMOUNT);

			FP_FEATURE_VECTOR fv_p = {
				.minutiae_amount=(uint32_t)mev_p.size(),
				.minutiae_data  = &(mev_p[0])
			};

			FP_FEATURE_VECTOR fv_g = {
				.minutiae_amount=(uint32_t)mev_g.size(),
				.minutiae_data  = &(mev_g[0])
			};

			auto score = PerformMatching(&fv_p, &fv_g);
			std::cout << tag << "," << score;
			if (vrbs_opt)
				std::cout << ",\t" << name_p << " " << name_g;
			std::cout << std::endl << std::flush;
		}
		return 0;
	}
	for (int i = 0; i < tmpl_cnt; ++i)
	{
		const char *name = file_tmpl[i];
		if (vrbs_opt)
			std::cout << "open file: " << name << std::endl;

		auto      neib = createAppmdNeighborhoodFrom(name);
		const int mnum = neib.descriptor_set().size();

		if (neib_opt)
		{
			if (vrbs_opt)
				std::cout << "num minucias:..." << mnum << "    [Mt sz]  [dir dist orient  Mv] ..." << std::endl;
			print_neighborhood(neib, get_print_range(param, param_cnt, mnum), mnum);
			continue;
		}


		std::vector<MINUTIA_ELEMENT> me_vec = neib2me_vec(neib, MAX_STAR_MINUTIAE_AMOUNT);

		if (kplt_opt)
		{
			if (vrbs_opt)
				std::cout << "num minucias:..." << me_vec.size() << "    [Mt sz]  [dir dist orient  Mv] ..." << std::endl;
			print_kplet_minutia_element(me_vec, get_print_range(param, param_cnt, me_vec.size()), me_vec.size());
			continue;
		}

		if (i+1 < tmpl_cnt)
		{
			const char *name_g = file_tmpl[i+1];
			if (vrbs_opt)
				std::cout << "open file: " << name_g << std::endl;

			auto      neib_g = createAppmdNeighborhoodFrom(name_g);
			//const int mnum_g = neib_g.descriptor_set().size();

			std::vector<MINUTIA_ELEMENT> me_g_vec = neib2me_vec(neib_g, MAX_STAR_MINUTIAE_AMOUNT);

			FP_FEATURE_VECTOR fv_p = {
				.minutiae_amount=(uint32_t)me_vec.size(),
				.minutiae_data  = &(me_vec[0])
			};
			//std::memcpy(fv_p.minutiae_data, &(me_vec[0]), sizeof(MINUTIA_ELEMENT)*me_vec.size());

			FP_FEATURE_VECTOR fv_g = {
				.minutiae_amount=(uint32_t)me_g_vec.size(),
				.minutiae_data  = &(me_g_vec[0])
			};
			//std::memcpy(fv_g.minutiae_data, &(me_g_vec[0]), sizeof(MINUTIA_ELEMENT)*me_g_vec.size());

			auto score = PerformMatching(&fv_p, &fv_g);

			std::cout << "score: " << score << std::endl;
		}
	}
}

