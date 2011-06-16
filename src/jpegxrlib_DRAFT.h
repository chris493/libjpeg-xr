/*
 * jpegxrlib.h
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file defines the application interface for the JPEG library.
 * Most applications using the library need only include this file,
 * and perhaps jerror.h if they want to know the exact error codes.
 */
 
 #ifndef JPEGLIBXR_H
#define JPEGLIBXR_H

/*
 * First we include the configuration files that record how this
 * installation of the JPEG library is set up.  jconfig.h can be
 * generated automatically for many systems.  jmorecfg.h contains
 * manual configuration options that most people need not worry about.
 */

#ifndef JCONFIG_INCLUDED	/* in case jinclude.h already did */
#include "jconfig.h"		/* widely used configuration options */
#endif
#include "jmorecfg.h"		/* seldom changed options */


/* Version ID for the JPEG library.
 * Might be useful for tests like "#if JPEGXR_LIB_VERSION >= 60".
 */

#define JPEGXR_LIB_VERSION  11	/* Version 1a */







#endif /* JPEGLIBXR_H */
