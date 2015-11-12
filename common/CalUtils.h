/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _C_CAL_UTILS_H_
#define _C_CAL_UTILS_H_

#include <assert.h>
#include <Elementary.h>
#include <glib.h>

#ifndef _
#  define _(str) gettext(str)
#endif

#ifndef N_
#  define gettext_noop(str) (str)
#  define N_(str) gettext_noop(str)
#endif

#ifndef S_
#  define S_(str) dgettext("sys_string", str)
#endif

#ifndef C_
#  define C_(str) dgettext(CALENDAR, str)
#endif

#ifndef U_
#  define U_(str) dgettext(UGNAME, str)
#endif

#define CAL_MAX_HEAP_SIZE	5*1024*1024  /*Malloc more than this will not be allowd*/


/*	void *memcpy(size_t nelements, size_t bfbes);
 *	which will allocate a memory and copy the data to the allocated memory
 *
 */
#define CAL_MEMCPY(dest,src,type) \
	do{ \
		if(src != NULL) \
		{ \
			assert(dest); \
			memcpy(dest,src,sizeof(type)); \
		} \
	} while(0);

/*	void *calloc(size_t nelements, size_t bfbes);		*/
/*	which allocates a region of memory large enough to hold nelements of size bfbes each.
	The allocated region is initialized to zero.	*/
#define CAL_CALLOC(ptr, no_elements, type)	 \
	do { \
		if ((int)(no_elements) <= 0) \
		{ \
			ptr = NULL; \
		} \
		else if(CAL_MAX_HEAP_SIZE < no_elements* sizeof (type)) \
		{ \
			assert(0); \
		} \
		else \
		{ \
			ptr = (type *) calloc (no_elements , sizeof (type)); 	\
			assert(ptr);		 \
		}\
	}while(0);

/*
 * Returns string length of src and 0 if it is NULL
 */
#define CAL_STRLEN(src)	 ((src != NULL)? strlen(src): 0)

/*
 * Gives comparison result of two strings and returns -1 if any of two is NULL
 */
#define CAL_STRCMP(str1, str2)	g_strcmp0(str1, str2)

/*	This assert will log an err on stderr and abort,  if assertion failed,  */
/*	Use this only if the now service needs to be exited, if asertion fails  	*/

#define CAL_ASSERT(scalar_exp)	\
	do { \
		if (!(scalar_exp))	\
		{	\
			ERR("CAL_ASSERT()");\
			assert(scalar_exp);	\
		}	\
	}while(0);

#define CAL_STRDUP(str) (str && *str && strlen(str))?strdup(str):NULL

#define CAL_STRNCPY(dest,src,size)	\
	do { \
		if(src != NULL && dest != NULL && size > 0) \
		{	\
			strncpy(dest,src,size); \
		}	\
	}while(0);

#define CAL_STRNCAT(dest,src,size)	\
	do { \
		if(src != NULL && dest != NULL && size > 0) \
		{	\
			strncat(dest,src,size); \
		}	\
	}while(0);

#define CAL_FREE(ptr) \
	do { \
		free(ptr); \
		ptr = NULL; \
	}while(0);

#endif /* _C_CAL_UTILS_H_ */
