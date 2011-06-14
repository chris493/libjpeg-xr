dnl IJG auto-configuration source file.
dnl Process this file with autoconf to produce a configure script.
#
# Configure script for IJG libjpeg
#
AC_PREREQ(2.59)
AC_INIT(jcmaster.c)

# Specify directory where m4 macros may be found.
AC_CONFIG_MACRO_DIR([m4])

# Directory where autotools helper scripts lives.
AC_CONFIG_AUX_DIR([config])

# Generate configuration headers.
AC_CONFIG_HEADERS([jconfig.h jconfig_api.h])

# Check system type
AC_CANONICAL_SYSTEM

# Initialize Automake
AM_INIT_AUTOMAKE([libjpeg], [6.1.0])

# Add configure option --enable-maintainer-mode which enables dependency
# checking and generation useful to package maintainers.  This is made an
# option to avoid confusing end users.
AM_MAINTAINER_MODE

# Check for programs
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
dnl
dnl
#
# Tests for Windows
#
AC_EXEEXT
AC_OBJEXT
dnl
dnl See if compiler supports prototypes.
AC_MSG_CHECKING(for function prototypes)
AC_CACHE_VAL(ijg_cv_have_prototypes,
[AC_TRY_COMPILE([
int testfunction (int arg1, int * arg2); /* check prototypes */
struct methods_struct {		/* check method-pointer declarations */
  int (*error_exit) (char *msgtext);
  int (*trace_message) (char *msgtext);
  int (*another_method) (void);
};
int testfunction (int arg1, int * arg2) /* check definitions */
{ return arg2[arg1]; }
int test2function (void)	/* check void arg list */
{ return 0; }
], [ ], ijg_cv_have_prototypes=yes, ijg_cv_have_prototypes=no)])
AC_MSG_RESULT($ijg_cv_have_prototypes)
if test $ijg_cv_have_prototypes = yes; then
  AC_DEFINE([HAVE_PROTOTYPES],[1],[Compiler supports function prototypes.])
else
  echo Your compiler does not seem to know about function prototypes.
  echo Perhaps it needs a special switch to enable ANSI C mode.
  echo If so, we recommend running configure like this:
  echo "   ./configure  CC='cc -switch'"
  echo where -switch is the proper switch.
fi
dnl
dnl check header files
AC_CHECK_HEADERS(stddef.h stdlib.h)
AC_CHECK_HEADER(string.h, , AC_DEFINE([NEED_BSD_STRINGS],[1],[Compiler has <strings.h> rather than standard <string.h>.]))
dnl See whether type size_t is defined in any ANSI-standard places;
dnl if not, perhaps it is defined in <sys/types.h>.
AC_MSG_CHECKING(for size_t)
AC_TRY_COMPILE([
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdio.h>
#ifdef NEED_BSD_STRINGS
#include <strings.h>
#else
#include <string.h>
#endif
typedef size_t my_size_t;
], [ my_size_t foovar; ], ijg_size_t_ok=yes,
[ijg_size_t_ok="not ANSI, perhaps it is in sys/types.h"])
AC_MSG_RESULT($ijg_size_t_ok)
if test "$ijg_size_t_ok" != yes; then
AC_CHECK_HEADER(sys/types.h, [AC_DEFINE([NEED_SYS_TYPES_H],[1],[Need to include <sys/types.h> in order to obtain size_t.])
AC_EGREP_CPP(size_t, [#include <sys/types.h>],
[ijg_size_t_ok="size_t is in sys/types.h"], ijg_size_t_ok=no)],
ijg_size_t_ok=no)
AC_MSG_RESULT($ijg_size_t_ok)
if test "$ijg_size_t_ok" = no; then
  echo Type size_t is not defined in any of the usual places.
  echo Try putting '"typedef unsigned int size_t;"' in jconfig.h.
fi
fi
dnl
dnl check compiler characteristics
AC_MSG_CHECKING(for type unsigned char)
AC_TRY_COMPILE(, [ unsigned char un_char; ],
[AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_UNSIGNED_CHAR],[1],[Compiler supports 'unsigned char'.])], AC_MSG_RESULT(no))
dnl
AC_MSG_CHECKING(for type unsigned short)
AC_TRY_COMPILE(, [ unsigned short un_short; ],
[AC_MSG_RESULT(yes)
AC_DEFINE([HAVE_UNSIGNED_SHORT],[1],[Compiler supports 'unsigned short'.])], AC_MSG_RESULT(no))
dnl
AC_MSG_CHECKING(for type void)
AC_TRY_COMPILE([
/* Caution: a C++ compiler will insist on valid prototypes */
typedef void * void_ptr;	/* check void * */
#ifdef HAVE_PROTOTYPES		/* check ptr to function returning void */
typedef void (*void_func) (int a, int b);
#else
typedef void (*void_func) ();
#endif

#ifdef HAVE_PROTOTYPES		/* check void function result */
void test3function (void_ptr arg1, void_func arg2)
#else
void test3function (arg1, arg2)
     void_ptr arg1;
     void_func arg2;
#endif
{
  char * locptr = (char *) arg1; /* check casting to and from void * */
  arg1 = (void *) locptr;
  (*arg2) (1, 2);		/* check call of fcn returning void */
}
], [ ], AC_MSG_RESULT(yes), [AC_MSG_RESULT(no)
AC_DEFINE([void],[char],[Define 'void' as 'char' for archaic compilers that don't understand it.])])

AC_C_CONST
dnl check for non-broken inline under various spellings
AC_MSG_CHECKING(for inline)
ijg_cv_inline=""
AC_TRY_COMPILE(, [} __inline__ int foo() { return 0; }
int bar() { return foo();], ijg_cv_inline="__inline__",
AC_TRY_COMPILE(, [} __inline int foo() { return 0; }
int bar() { return foo();], ijg_cv_inline="__inline",
AC_TRY_COMPILE(, [} inline int foo() { return 0; }
int bar() { return foo();], ijg_cv_inline="inline")))
AC_MSG_RESULT($ijg_cv_inline)
AC_DEFINE_UNQUOTED([INLINE],[$ijg_cv_inline],[How to obtain function inlining.])
dnl we cannot check for bogus warnings, but at least we can check for errors
AC_MSG_CHECKING(for broken incomplete types)
AC_TRY_COMPILE([ typedef struct undefined_structure * undef_struct_ptr; ], ,
AC_MSG_RESULT(ok),
[AC_MSG_RESULT(broken)
AC_DEFINE([INCOMPLETE_TYPES_BROKEN],[1],[Compiler does not support pointers to unspecified structures.])])
dnl test whether global names are unique to at least 15 chars
AC_MSG_CHECKING(for short external names)
AC_TRY_LINK([
int possibly_duplicate_function () { return 0; }
int possibly_dupli_function () { return 1; }
], [ ], AC_MSG_RESULT(ok), [AC_MSG_RESULT(short)
AC_DEFINE([NEED_SHORT_EXTERNAL_NAMES],[1],[Linker requires that global names be unique in first 15 characters.])])
dnl
dnl run-time checks
AC_MSG_CHECKING(to see if char is signed)
AC_TRY_RUN([
#ifdef HAVE_PROTOTYPES
int is_char_signed (int arg)
#else
int is_char_signed (arg)
     int arg;
#endif
{
  if (arg == 189) {		/* expected result for unsigned char */
    return 0;			/* type char is unsigned */
  }
  else if (arg != -67) {	/* expected result for signed char */
    printf("Hmm, it seems 'char' is not eight bits wide on your machine.\n");
    printf("I fear the JPEG software will not work at all.\n\n");
  }
  return 1;			/* assume char is signed otherwise */
}
char signed_char_check = (char) (-67);
int main() {
  exit(is_char_signed((int) signed_char_check));
}], [AC_MSG_RESULT(no)
AC_DEFINE([CHAR_IS_UNSIGNED],[1],[Characters are unsigned])], AC_MSG_RESULT(yes),
[echo Assuming that char is signed on target machine.
echo If it is unsigned, this will be a little bit inefficient.
])
dnl
AC_MSG_CHECKING(to see if right shift is signed)
AC_TRY_RUN([
#ifdef HAVE_PROTOTYPES
int is_shifting_signed (long arg)
#else
int is_shifting_signed (arg)
     long arg;
#endif
/* See whether right-shift on a long is signed or not. */
{
  long res = arg >> 4;

  if (res == -0x7F7E80CL) {	/* expected result for signed shift */
    return 1;			/* right shift is signed */
  }
  /* see if unsigned-shift hack will fix it. */
  /* we can't just test exact value since it depends on width of long... */
  res |= (~0L) << (32-4);
  if (res == -0x7F7E80CL) {	/* expected result now? */
    return 0;			/* right shift is unsigned */
  }
  printf("Right shift isn't acting as I expect it to.\n");
  printf("I fear the JPEG software will not work at all.\n\n");
  return 0;			/* try it with unsigned anyway */
}
int main() {
  exit(is_shifting_signed(-0x7F7E80B1L));
}], [AC_MSG_RESULT(no)
AC_DEFINE([RIGHT_SHIFT_IS_UNSIGNED],[1],[Broken compiler shifts signed values as an unsigned shift.])], AC_MSG_RESULT(yes),
AC_MSG_RESULT(Assuming that right shift is signed on target machine.))
dnl
AC_MSG_CHECKING(to see if fopen accepts b spec)
AC_TRY_RUN([
#include <stdio.h>
int main() {
  if (fopen("conftestdata", "wb") != NULL)
    exit(0);
  exit(1);
}], AC_MSG_RESULT(yes), [AC_MSG_RESULT(no)
AC_DEFINE([DONT_USE_B_MODE],[1],[Don't open files in binary mode.])],
AC_MSG_RESULT(Assuming that it does.))

# Configure libtool
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Select memory manager depending on user input.
# If no "-enable-maxmem", use jmemnobs
MEMORYMGR='jmemnobs'
MAXMEM="no"
AC_ARG_ENABLE(maxmem,
[  --enable-maxmem[=N]     enable use of temp files, set max mem usage to N MB],
MAXMEM="$enableval")
dnl [# support --with-maxmem for backwards compatibility with IJG V5.]
dnl AC_ARG_WITH(maxmem, , MAXMEM="$withval")
if test "x$MAXMEM" = xyes; then
  MAXMEM=1
fi
if test "x$MAXMEM" != xno; then
  if test -n "`echo $MAXMEM | sed 's/[[0-9]]//g'`"; then
    AC_MSG_ERROR(non-numeric argument to --enable-maxmem)
  fi
  DEFAULTMAXMEM=`expr $MAXMEM \* 1048576`
AC_DEFINE_UNQUOTED([DEFAULT_MAX_MEM], [${DEFAULTMAXMEM}], [Maximum data space library will allocate.])
AC_MSG_CHECKING([for 'tmpfile()'])
AC_TRY_LINK([#include <stdio.h>], [ FILE * tfile = tmpfile(); ],
[AC_MSG_RESULT(yes)
MEMORYMGR='jmemansi'],
[AC_MSG_RESULT(no)
dnl if tmpfile is not present, must use jmemname.
MEMORYMGR='jmemname'

# Test for the need to remove temporary files using a signal handler (for cjpeg/djpeg)
AC_DEFINE([NEED_SIGNAL_CATCHER],[1],[Need signal handler to clean up temporary files.])
AC_MSG_CHECKING([for 'mktemp()'])
AC_TRY_LINK(, [ char fname[80]; mktemp(fname); ], AC_MSG_RESULT(yes),
[AC_MSG_RESULT(no)
AC_DEFINE([NO_MKTEMP],[1],[The mktemp() function is not available.])])])
fi
AC_SUBST(MEMORYMGR)

# Support definitons for cjpeg/djpeg
AC_DEFINE([BMP_SUPPORTED],[1],[cjpeg/djpeg support the BMP file format.])
AC_DEFINE([GIF_SUPPORTED],[1],[cjpeg/djpeg support the GIF file format.])
AC_DEFINE([PPM_SUPPORTED],[1],[cjpeg/djpeg support the PBMPLUS PPM/PGM file formats.])
dnl AC_DEFINE([RLE_SUPPORTED],[1],[cjpeg/djpeg support the Utah RLE file format.])
AC_DEFINE([TARGA_SUPPORTED],[1],[cjpeg/djpeg support the Targa file format.])

# Include PNG support in cjpeg/djpeg if appropriate.
AC_ARG_WITH(png,
[  --with-png              enable PNG support in cjpeg/djpeg],
[#], [with_png="maybe"])

# try to detect whether libpng is present
OLIBS=$LIBS
if test "x$with_png" = xmaybe ; then
  AC_CHECK_HEADER(png.h, , [with_png="no"])
fi
if test "x$with_png" = xmaybe ; then
  AC_CHECK_HEADER(zlib.h, , [with_png="no"])
fi
if test "x$with_png" = xmaybe ; then
  AC_CHECK_LIB(z, deflate, , [with_png="no"])
fi
if test "x$with_png" = xmaybe ; then
  AC_CHECK_LIB(png, png_create_info_struct,
	[with_png="yes"], [with_png="no"], [-lz -lm])
fi
LIBS=$OLIBS

if test "x$with_png" = xyes ; then
  AC_DEFINE([PNG_SUPPORTED],[1],[Libpng is available in order to support the PNG file format.])
  PNGLIBS='-lpng -lz -lm'
fi
AC_SUBST([PNGLIBS])

# Extract the library version ID from jpeglib.h.
AC_MSG_CHECKING([libjpeg version number])
[JPEG_LIB_VERSION=`sed -e '/^#define JPEG_LIB_VERSION/!d' -e 's/^[^0-9]*\([0-9][0-9]*\).*$/\1/' $srcdir/jpeglib.h`]
AC_MSG_RESULT([$JPEG_LIB_VERSION])
AC_SUBST([JPEG_LIB_VERSION])

dnl
AC_OUTPUT([Makefile])
