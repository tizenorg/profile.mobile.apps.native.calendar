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

#ifndef _CAL_BOOK_H_
#define _CAL_BOOK_H_

#include "WDefine.h"

typedef void* cal_book_h;

/**
* @class	CalBook
* @brief	This class is class of calendar book information
*
*
* The %CalBook class contains information about calendar book with event.
* It serves to save changes in database. It is wrapper of %calendar_record_h of book entity.
*/
class WAPP_ASSIST_EXPORT CalBook
{
public:

	/**
	 * @brief Generic data type enum.
	 */
	enum SyncDataType {
		SYNC_DATA_1 = 1,
		SYNC_DATA_2,
		SYNC_DATA_3,
		SYNC_DATA_4
	};

	CalBook();
	virtual ~CalBook();

public:

	/**
	 * @brief Get book ID.
	 *
	 * @return book id
	 *
	 */
	int getIndex() const;

	/**
	 * @brief Get book name.
	 *
	 * @return book getName
	 *
	 */
	const char* getName() const;

	/**
	 * @brief Get book color.
	 *
	 * @param[out] r        Red
	 * @param[out] g        Green
	 * @param[out] b        Blue
	 * @param[out] a        Alpha
	 *
	 */
	void getColor(int& r, int& g, int& b, int& a) const;

	/**
	 * @brief Get book location.
	 *
	 * @return book location.
	 *
	 */
	const char* getLocation() const;

	/**
	 * @brief Get book visibility.
	 *
	 * @return book visibility.
	 *
	 */
	bool getVisibility() const;

	/**
	 * @brief Get account ID.
	 *
	 * @return account ID
	 *
	 */
	int getAccountId() const;

	/**
	 * @brief Get store ID.
	 *
	 * @return store ID
	 *
	 */
	int getStoreType() const;

	/**
	 * @brief getSyncData
	 *
	 * @param[in] type        generic data type
	 *
	 * @return generic data
	 *
	 */
	const char* getSyncData(SyncDataType type) const;

	/**
	 * @brief Get book mode.
	 *
	 * @return book mode (read/read-write).
	 *
	 * @see calendar_book_mode_e
	 *
	 */
	int getMode() const;

	/**
	 * @brief Set Unique identifier to the book.
	 *
	 * @param[in] uid        unique identifier
	 *
	 */
	void setUid(const char* uid);

	/**
	 * @brief Set name to the book.
	 *
	 * @param[in] name        book name
	 *
	 */
	void setName(const char* name);

	/**
	 * @brief Set description to the book.
	 *
	 * @param[in] description        book description
	 *
	 */
	void setDescription(const char* description);

	/**
	 * @brief Set color to the book.
	 *
	 * @param[in] r        Red
	 * @param[in] g        Green
	 * @param[in] b        Blue
	 * @param[in] a        Alpha
	 *
	 */
	void setColor(int r, int g, int b, int a);

	/**
	 * @brief Set location to the book.
	 *
	 * @param[in] location        book location
	 *
	 */
	void setLocation(const char* location);

	/**
	 * @brief Set visibility to the book.
	 *
	 * @param[in] visibility        book visibility
	 *
	 */
	void setVisibility(bool visibility);

	/**
	 * @brief Set account ID to the book.
	 *
	 * @param[in] accountId        book account ID
	 *
	 */
	void setAccountId(int accountId);

	/**
	 * @brief Set store type ID to the book.
	 *
	 * @param[in] storeType        book store type ID
	 *
	 */
	void setStoreType(int storeType);

	/**
	 * @brief Set generic sync data.
	 *
	 * @param[in] type            type of sync data
	 * @param[in] syncData        generic data
	 *
	 */
	void setSyncData(SyncDataType type, const char *syncData);

	/**
	 * @brief Set book mode.
	 *
	 * @param[in] mode            book mode (read/read-write)
	 *
	 * @see calendar_book_mode_e
	 *
	 */
	void setMode(int mode);

	/**
	 * @brief Check whether is read only mode.
	 *
	 * @return is readable.
	 *
	 */
	bool isReadOnly() const;

protected:
	friend class CalBookManager;
	explicit CalBook(const cal_book_h book);
	const cal_book_h getBook() const;
	void updateDbRecord();

private:
	CalBook(const CalBook&);
	const CalBook& operator=(const CalBook&);

	CalBook* cloneBook() const;
	void destroyBook();
	void parseColorString(char *color, int &r, int &g, int &b, int &a) const;
	cal_book_h __book;
};

#endif /* _CAL_BOOK_H_ */
