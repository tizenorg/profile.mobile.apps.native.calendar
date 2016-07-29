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
#include "WDefine.h"
#include "CalSingleton.h"
#include "CalBook.h"
#include "CalEvent.h"

typedef void* cal_books_h;

/**
* @class	CalBookManager
* @brief	This class is class of container of %CalBook entities
*
*
* The %CalBookManager class is the class of a manager of calendar books.
* %CalBookManager serves to manage %CalBook entities
*/
class WAPP_ASSIST_EXPORT CalBookManager
{
SINGLETON_IDIOM(CalBookManager);

public:

	/**
	 * @brief Get book by index.
	 *
	 * @param[in] index         book index
	 *
	 * @return book object.
	 */
	std::shared_ptr<CalBook> getBook(int index);

	/**
	 * @brief Get book list.
	 *
	 * @param[out] bookMap         book list
	 *
	 */
	void getBooks(std::map<int, std::shared_ptr<CalBook>>& bookMap);

	/**
	 * @brief Update book.
	 *
	 * @param[in] book         book
	 *
	 * @return operation result.
	 *
	 */
	int updateBook(const std::shared_ptr<CalBook>& book);

protected:
	CalBookManager();
	virtual ~CalBookManager();

private:
	WDISABLE_COPY_AND_ASSIGN(CalBookManager);

	void __notify(CalEvent::Source source);
	static void __onChanged(const char* uri, void* userData);
	void __createBookMap();
	void __destroyBookMap();
	void __addLocalBooks(std::map<int, std::shared_ptr<CalBook>>& bookMap);
	void __fillColorlessBook(std::map<int, std::shared_ptr<CalBook>>& bookMap);

	std::map<int, std::shared_ptr<CalBook>> __bookMap;
};

#endif /* _CAL_BOOK_MANAGER_H_ */
