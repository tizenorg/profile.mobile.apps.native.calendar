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

#ifndef _C_CAL_BOOK_H_
#define _C_CAL_BOOK_H_

#include <Eina.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void* cal_book_h;

typedef enum {
	CAL_BOOK_DIRTY_NONE = 0,
	CAL_BOOK_DIRTY_INSERT,
	CAL_BOOK_DIRTY_DELETE,
	CAL_BOOK_DIRTY_UPDATE,
	CAL_BOOK_DIRTY_CHECKED
} cal_book_dirty_e;

typedef struct {
	int red;
	int green;
	int blue;
	int alpha;
} cal_book_rgb_color_s;

cal_book_h cal_book_create();
cal_book_h cal_book_create_with_record(void* record);
void cal_book_destroy(cal_book_h book);
cal_book_h cal_book_clone(const cal_book_h book);

void cal_book_set_record(cal_book_h book, void* record);
void cal_book_set_uid(cal_book_h book, const char* uid);
void cal_book_set_name(cal_book_h book, const char* name);
void cal_book_set_description(cal_book_h book, const char* description);
void cal_book_set_color(cal_book_h book, const char* color);
void cal_book_set_color_rgb(const cal_book_h book, int r, int g, int b, int a);
void cal_book_set_color_rgb_struct(const cal_book_h book, cal_book_rgb_color_s rgb_color);
void cal_book_set_location(cal_book_h book, const char* location);
void cal_book_set_visibility(cal_book_h book, Eina_Bool visibility);
void cal_book_set_account_id(cal_book_h book, int account_id);
void cal_book_set_store_type(cal_book_h book, int store_type);
void cal_book_set_sync_data1(cal_book_h book, const char* sync_data1);
void cal_book_set_sync_data2(cal_book_h book, const char* sync_data2);
void cal_book_set_sync_data3(cal_book_h book, const char* sync_data3);
void cal_book_set_sync_data4(cal_book_h book, const char* sync_data4);
void cal_book_set_mode(cal_book_h book, int mode);
void cal_book_set_dirty(cal_book_h book, cal_book_dirty_e dirty);

const void* cal_book_get_record(cal_book_h book);
int cal_book_get_id(const cal_book_h book);
const char* cal_book_get_uid(const cal_book_h book);
const char* cal_book_get_name(const cal_book_h book);
const char* cal_book_get_description(const cal_book_h book);
const char* cal_book_get_color(const cal_book_h book);
void cal_book_get_color_rgb(const cal_book_h book, int* r, int* g, int* b, int* a);
cal_book_rgb_color_s cal_book_get_color_rgb_struct(const cal_book_h book);
const char* cal_book_get_location(const cal_book_h book);
Eina_Bool cal_book_get_visibility(const cal_book_h book);
int cal_book_get_account_id(const cal_book_h book);
int cal_book_get_store_type(const cal_book_h book);
const char* cal_book_get_sync_data1(const cal_book_h book);
const char* cal_book_get_sync_data2(const cal_book_h book);
const char* cal_book_get_sync_data3(const cal_book_h book);
const char* cal_book_get_sync_data4(const cal_book_h book);
int cal_book_get_mode(const cal_book_h book);
cal_book_dirty_e cal_book_get_dirty(cal_book_h book);

#ifdef __cplusplus
}
#endif

#endif /* _C_CAL_BOOK_H_ */
