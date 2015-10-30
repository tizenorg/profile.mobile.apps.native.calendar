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
#include "cal_book.h"

class WAPP_ASSIST_EXPORT CalBook
{
public:
	CalBook();
	virtual ~CalBook();
	CalBook(const CalBook&);
	const CalBook& operator=(const CalBook&);
public:
	//get
	int getIndex() const;
	const char* getUid() const;
	const char* getName() const;
	const char* getDescription() const;
	void getColor(int& r, int& g, int& b, int& a) const;
	const char* getLocation() const;
	bool getVisibility() const;
	int getAccountId() const;
	int getStoreType() const;
	const char* getSyncData1() const;
	const char* getSyncData2() const;
	const char* getSyncData3() const;
	const char* getSyncData4() const;
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
	void setSyncData1(const char* syncData);
	void setSyncData2(const char* syncData);
	void setSyncData3(const char* syncData);
	void setSyncData4(const char* syncData);
	void setMode(int mode);

	bool isReadOnly() const;
protected:
	friend class CalBookManager;
	CalBook(const cal_book_h book);
	const cal_book_h getBook() const;
private:
	cal_book_h __book;
};

#endif /* _CAL_BOOK_H_ */
