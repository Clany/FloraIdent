#ifndef CLANY_MACROS_H
#define CLANY_MACROS_H

#define _CLANY_BEGIN	namespace clany {
#define _CLANY_END		}

#if defined CLANYAPI_EXPORTS
#  define CLANY_EXPORTS __declspec(dllexport)
#else
#  define CLANY_EXPORTS
#endif

#ifdef _MSC_VER
#  include <crtdbg.h>
#  define ASSERT _ASSERTE
#else
#  include <cassert>
#  define ASSERT assert
#endif

#ifndef NDEBUG
#  define DBGVAR( os, var ) \
     (os) << "Debug: " << __FILE__ << "(" << __LINE__ << ") "\
          << #var << " = [" << (var) << "]" << std::endl
#  define DBGMSG( os, msg ) \
     (os) << "Debug: " << __FILE__ << "(" << __LINE__ << ") " \
          << msg << std::endl
#  define VERIFY ASSERT
#else
#  define DBGVAR(os, var) ((void)0)
#  define DBGMSG(os, msg) ((void)0)
#  define VERIFY(expression) (expression)
#endif

typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long long llong;
typedef unsigned long long ullong;

_CLANY_BEGIN
using namespace std;
_CLANY_END

#endif // CLANY_MACROS_H
