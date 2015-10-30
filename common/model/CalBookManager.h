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

#ifndef _CAL_BOOK_MANAGER_H_
#define _CAL_BOOK_MANAGER_H_

#include <memory>
#include <map>
#include <list>
#include "cal_books.h"
#include "WDefine.h"
#include "CalSingleton.h"
#include "CalBook.h"
#include "CalEvent.h"

class WAPP_ASSIST_EXPORT CalBookManager
{
SINGLETON_IDIOM(CalBookManager);
public:
	std::shared_ptr<CalBook> getBook(int index);
	std::shared_ptr<CalBook> findBook(const char* name);
	std::shared_ptr<CalBook> findBook(int r, int g, int b, int a);
	std::shared_ptr<CalBook> findBook(const char* name, int r, int g, int b, int a);
	bool isAllVisible();
	void getBooks(std::map<int, std::shared_ptr<CalBook>>& bookMap);
	int insertBook(const std::shared_ptr<CalBook>& book);
	int deleteBook(const std::shared_ptr<CalBook>& book);
	int updateBook(const std::shared_ptr<CalBook>& book);
protected:
	CalBookManager();
	virtual ~CalBookManager();
private:
	WDISABLE_COPY_AND_ASSIGN(CalBookManager);
private:
	void __notify(CalEvent::Source source);
	static void __onChanged(const char* uri, void* userData);
	void __createBookMap();
	void __destroyBookMap();
	void __addLocalBooks(std::map<int, std::shared_ptr<CalBook>>& bookMap);

private:
	std::map<int, std::shared_ptr<CalBook>> __bookMap;
	cal_books_h __books;
};

#endif /* _CAL_BOOK_MANAGER_H_ */
