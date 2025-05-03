AM_CPPFLAGS     = -Wall -Werror


lib_LTLIBRARIES = libippmd.la
libippmd_la_SOURCES = \
		ippmd_print.cpp         ippmd_print.hpp \
		ippmd_acmp.cpp          ippmd_acmp.hpp \
		ippmd_vector.cpp        ippmd_vector.hpp \
		ippmd_minutia.hpp \
		ippmd_descriptor.cpp    ippmd_descriptor.hpp \
		ippmd_neighborhood.cpp  ippmd_neighborhood.hpp


bin_PROGRAMS  = kplet
kplet_SOURCES = \
		main.cpp \
		kplet.cpp kplet.hpp

kplet_LDADD = libippmd.la

