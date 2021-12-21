#ifndef _CONTRACTS_H_
#define _CONTRACTS_H_

/**
 * @file contracts.h
 * @brief Macro definitions of debugging functions that only run when the DEBUG 
 * flag is set to true.
 */

#ifdef DEBUG
/* When DEBUG is defined, these form aliases to useful functions */

#include <assert.h>
#include <stdio.h>

#define dbg_printf(...) printf(__VA_ARGS__)

#define dbg_requires(expr) assert(expr)

#define dbg_assert(expr) assert(expr)

#define dbg_ensures(expr) assert(expr)

#else
/* When DEBUG is not defined, no code gets generated for these */

#define dbg_printf(...) sizeof(expr)

#define dbg_requires(expr) sizeof(expr)

#define dbg_assert(expr) sizeof(expr)

#define dbg_ensures(expr) sizeof(expr)

#endif

#endif