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
	enum SyncDataType {
		SYNC_DATA_1 = 1,
		SYNC_DATA_2,
		SYNC_DATA_3,
		SYNC_DATA_4
	};
	CalBook();
	virtual ~CalBook();
private:
	CalBook(const CalBook&);
	const CalBook& operator=(const CalBook&);
public:
	//get
	int getIndex() const;
	const char* getName() const;
	void getColor(int& r, int& g, int& b, int& a) const;
	const char* getLocation() const;
	bool getVisibility() const;
	int getAccountId() const;
	int getStoreType() const;
	const char* getSyncData(SyncDataType type) const;

	int getMode() const;
	//set
	void setUid(const char* uid);
	void setName(const char* name);
	void setDescription(const char* description);
	void setColor(int r, int g, int b, int a);
	void setLocation(const char* location);
	void setVisibility(bool visibility);
	void setAccountId(int accountId);
	void setStoreType(int storeType);
	void setSyncData(SyncDataType type, const char *syncData);
	void setMode(int mode);

	bool isReadOnly() const;
protected:
	friend class CalBookManager;
	explicit CalBook(const cal_book_h book);
	const cal_book_h getBook() const;
	void updateDbRecord();
private:
	CalBook* cloneBook() const;
	void destroyBook();
	void parseColorString(char *color, int &r, int &g, int &b, int &a) const;
	cal_book_h __book;
};

#endif /* _CAL_BOOK_H_ */
