/*!
 * \file
 * System determination for mDataRepresentation module.
 *
 * \remarks
 *
 * \authors
 * lpapp
 */

#pragma once

//-----------------------------------------------------------------------------

// Compiler determination.
#if defined( __GNUC__ )
#	define CC_GCC			( __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ )
#	ifdef __clang__
#		define CC_CLANG		( __clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__ )
#	endif
#elif defined( _MSC_VER )
#	define CC_MSVC			_MSC_VER
#else
#	error This compiler is not supported!
#endif

// Operating system and ISA determination.
#if defined( __linux__ ) || defined( __linux )
#	define HOST_LINUX
#	if !defined( __x86_64__ )
#		error This architecture is not supported!
#	endif
#elif defined( _WIN32 )
#	define HOST_WINDOWS
#	if !defined( _M_AMD64 ) && !defined( __x86_64__ )
#		error This architecture is not supported!
#	endif
#else
#	error This platform is not supported!
#endif

// OpenMP function replacements in case it is not enabled.
#include <cmath>
#if defined( USE_OPENMP )
#	if ( CC_GCC >= 40300 ) || ( CC_CLANG >= 30700 ) || ( CC_MSVC >= 1400 )
#		include <omp.h>
#	else
static inline int omp_get_max_threads() { return 1; }
static inline int omp_get_thread_num() { return 0; }
#	endif
#endif

#if defined( CC_MSVC )
#include <malloc.h>
#elif defined( CC_GCC )
#include <alloca.h>
#include <cstdlib>
#endif

// Include storage-class information definitions.
#if defined( CC_MSVC )
#	define __noinline  __declspec( noinline )
#elif defined( CC_GCC )
#	define __forceinline  __attribute__( ( always_inline ) )
#	define __noinline     __attribute__( ( noinline ) )
#endif

// Address alignment.
#if defined( CC_MSVC )
#	define __align( x )  __declspec( align( x ) )
#elif defined( CC_GCC )
#	define __align( x )  __attribute__( ( aligned( x ) ) )
#endif

// Memory allocation on stack and code size shrink.
#if defined( CC_MSVC )
#	define stack_alloc( size )                      _alloca( size )
#	define aligned_malloc( alignment, size )        _aligned_malloc( size, alignment )
#	define aligned_realloc( ptr, alignment, size )  _aligned_realloc( ptr, size, alignment )
#	define aligned_free( ptr )                      _aligned_free( ptr )
#elif defined( CC_GCC )
#	define stack_alloc( size )                      __builtin_alloca( size )
#	define aligned_malloc( alignment, size )        ::aligned_alloc( alignment, size )
#	define aligned_free( ptr )                      ::free( ptr )

void *aligned_realloc( void* aPtr, size_t aAlignment, size_t aSize );
#endif
