
#ifndef _libid_config_h_
#define _libid_config_h_

/*
 * This header file defines various configurable attributes of id-mgmt lib
 *
 * The header-file defines default values.
 *
 * TODO: Enhancement to expose overriding option for lib-user based on context
 */

#define GC_TIMEOUT  300 // Number of seconds of Garbage collection

#define CHUNK_SIZE  32 // Fixed based on allocation_flag length

#endif
