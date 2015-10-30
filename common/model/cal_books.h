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

#ifndef _C_CAL_BOOKS_H_
#define _C_CAL_BOOKS_H_

#include "cal_book.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void* cal_books_h;

cal_books_h cal_books_initialize();
void cal_books_finalize(cal_books_h books);
Eina_Bool cal_books_update_from_db(cal_books_h books);
void cal_books_commit_to_db(cal_books_h books);
Eina_List* cal_books_get_books_by_accound_id_p(cal_books_h books, int account_id);

int cal_books_insert_book(cal_books_h books, int account_id, const cal_book_h book);
const cal_book_h cal_books_get_book_p(cal_books_h books, int id);
int cal_books_update_book(cal_books_h books, const cal_book_h book);
int cal_books_delete_book(cal_books_h books, int id);
cal_book_h cal_books_insert_book_without_handle(int account_id, const cal_book_h book);
Eina_Bool cal_books_exist_name(cal_books_h books, int account_id, int store_type, const char* name);

#ifdef __cplusplus
}
#endif

#endif /* _C_CAL_BOOKS_H_ */
