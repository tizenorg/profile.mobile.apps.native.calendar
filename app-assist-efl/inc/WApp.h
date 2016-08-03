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

#ifndef _WAPP_H_
#define _WAPP_H_

#include <app.h>
#include "WWindow.h"
#include "WDefine.h"

/**
* @class WApp.
*
* @brief This class is the base class of an application.
*
* The %WApp class is the base class of an application. An application class must be inherited from this class.
* @code
*
* // main.cpp
* WAPP_ASSIST_EXPORT int main( int argc, char* argv[] )
* {
*   MyApp app;
*   int ret = app.start( argc, argv );
*   return ret;
* }
* @endcode
*/
class WAPP_ASSIST_EXPORT WApp
{
public:
	/**
	 * @brief This is the default constructor for this class.
	 */
	WApp();

	/**
	 * @brief This is the destructor for this class.
	 */
	virtual ~WApp();

	/**
	 * @brief Starts the application.
	 *
	 * @param[in]   argc   The argument count.
	 * @param[in]   argv   The argument vector.
	 *
	 * @return result
	 */
	int start( int argc, char* argv[] );

protected:
	/**
	 * @brief Called when the application started.
	 *
	 * @param[in]   argc   The argument count.
	 * @param[in]   argv   The argument vector.
	 *
	 * @return result.
	 */
	virtual int onStart( int argc, char* argv[] );

	/**
	 * @brief Called when the application created.
	 *
	 * @return True if the application created successfully, else false.
	 */
	virtual bool onCreate(){return true;};

	/**
	 * @brief Called when the application is terminated.
	 */
	virtual void onTerminate(){};

	/**
	 * @brief Called when the application is paused.
	 */
	virtual void onPause(){};

	/**
	 * @brief Called when the application is resumed.
	 */
	virtual void onResume(){};

	/**
	 * @brief Called when some request comes to the application.
	 *
	 * @param[in]   request       Handle of request information.
	 * @param[in]   firstLaunch   True, if it is called after creation.
	 */
	virtual void onAppControl(app_control_h request, bool firstLaunch );

	/**
	 * @brief Gets the window class instance of the application.
	 *
	 * @return The pointer to the window.
	 */
	WWindow* getWindow();

	/**
	 * @brief Attaches a window to the application.
	 *
	 * @param[in]   win   A pointer to the window to attach.
	 * @return True if the window attached successfully, else false.
	 */
	bool attachWindow(WWindow* win);

private:
	WDECLARE_PRIVATE_IMPL(WApp);
	WDISABLE_COPY_AND_ASSIGN(WApp);

};

#endif /* _WAPP_H_ */
