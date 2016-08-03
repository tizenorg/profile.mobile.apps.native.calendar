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

#ifndef _WWINDOW_H_
#define _WWINDOW_H_

#include <functional>
#include <Evas.h>
#include <Elementary.h>
#include "WDefine.h"

class IWUiObject;

/**
* @class WWindow
*
* @brief This class represents window and base layout evas object.
*
* The %WWindow manages window evas object and common base evas objects.
*/
class WAPP_ASSIST_EXPORT WWindow
{
public:
	/**
	 * @brief Constructor.
	 * Initialize this instance of WWindow with the specified value.
	 *
	 * @param[in]   name      The window name.
	 * @param[in]   winType   The type of the window.
	 */
	WWindow(const char* name, Elm_Win_Type winType);

	/**
	 * @brief Constructor.
	 * Initialize this instance of WWindow with the specified value.
	 *
	 * @param[in]   win          Pointer to evas object window.
	 * @param[in]   conform      Pointer to evas object conformant.
	 * @param[in]   baseLayout   Pointer to evas object base layout.
	 */
	WWindow(Evas_Object* win, Evas_Object* conform, Evas_Object* baseLayout);

	/**
	 * @brief Get the class name of this class.
	 *
	 * @return The class name.
	 */
	virtual const char* getClassName();

	/**
	 * @brief Create the windows
	 * "onCreateWin" and "onCreateBaseLayout" will be called subsequently.
	 *
	 * @return True if the windows created successfully, else false.
	 *
	 * @see onCreateWin()
	 * @see onCreateBaseLayout()
	 */
	bool create();

	/**
	 * @brief Destros the window class instance and delete window evas object.
	 * "onDestroy" will be called subsequently.
	 *
	 * @see onDestroy()
	 */
	void destroy();

	/**
	 * @brief Check the object to owner the window handle.
	 *
	 * @return True if object is owner, false otherwise.
	 */
	bool isOwner() const;

	/**
	 * @brief Get the evas object of the window.
	 *
	 * @return The pointer to the evas object of the window.
	 *
	 * @see onCreateWin()
	 */
	Evas_Object* getEvasObj();

	/**
	 * @brief Get the base layout evas object.
	 *
	 * @return The pointer to the evas object of the layout.
	 *
	 * @see onCreateBaseLayout()
	 */
	Evas_Object* getBaseLayoutEvasObj();

	/**
	 * @brief Get the conformant evas object.
	 *
	 * @return The pointer to the evas object of the conformant.
	 */
	Evas_Object* getConformantEvasObj();

	/**
	 * @brief Attache base UI object, which can be WNaviframe or WView instance.
	 *
	 * @param[in]   baseObj         IWUiObject instance to be attached
	 * @param[in]   creationParam   Parameter to be passed as parameter of
	 *                              create method of the IWUiObject class.
	 *
	 * @return True if the base ui object attached successfully, else false.
	 *
	 * @see getBaseUiObject()
	 */
	bool attachBaseUiObject( IWUiObject* baseObj, void* creationParam = NULL );

	/**
	 * @brief Get the base UI object attached.
	 *
	 * @return The pointer to the base ui object.
	 *
	 * @see attachBaseUiObject()
	 */
	IWUiObject* getBaseUiObject();

	/**
	 * @brief Detach the base UI object.
	 *
	 * @return The base UI object, which should be deleted explicitly later.
	 */
	IWUiObject* detachBaseUiObject();

	/**
	 * @brief Destroy the base UI object.
	 */
	void destroyBaseUiObject();

	/**
	 * @brief Get the window name.
	 *
	 * @return The name of the window.
	 */
	const char* getName();

	/**
	 * @brief Get the type of the window.
	 *
	 * @return The type of the window.
	 */
	Elm_Win_Type getWinType();

	/**
	 * @brief Set the handler function pointer to override virtual function
	 * onCreateWin without defining a new inherited class.
	 * If it is set, onCreateWin method will not be called.
	 *
	 * @param[in]   handlerFunc   The pointer to handler function.
	 *
	 * @see onCreateWin()
	 */
	void setOnCreateWin( const std::function<Evas_Object* ()>& handlerFunc );

	/**
	 * @brief Set the handler function pointer to override virtual function
	 * onCreateBaseLayout without defining a new inherited class.
	 * If it is set, onCreateBaseLayout method will not be called.
	 *
	 * @param[in]   handlerFunc   The pointer to handler function.
	 *
	 * @see onCreateBaseLayout()
	 */
	void setOnCreateBaseLayout( const std::function<Evas_Object* (Evas_Object*, Evas_Object** outConformant)>& handlerFunc );

protected:
	/**
	 * @brief Called when create method is called to create window
	 * Override this method to create your own window evas object.
	 *
	 * @return The pointer to the evas object of the window created.
	 *
	 * @see create()
	 */
	virtual Evas_Object* onCreateWin();

	/**
	 * @brief Called when create method is called to create base layout.
	 * Override this method to create your own base layout and conformant evas objects.
	 *
	 * @param[in]    win             A pointer to evas object of the window
	 * @param[out]   outConformant   The pointer to the evas object of the conformant created.
	 *
	 * @return The pointer to the evas object of base layout created.
	 */
	virtual Evas_Object* onCreateBaseLayout(Evas_Object* win, Evas_Object** outConformant);

	/**
	 * @brief Called when the window is destroyed
	 * Override this method to do something on destruction.
	 */
	virtual void onDestroy();

protected:
	/**
	 * @brief This is the destructor for this class.
	 */
	virtual ~WWindow();

private:
	WWindow();
	WDECLARE_PRIVATE_IMPL(WWindow);
	WDISABLE_COPY_AND_ASSIGN(WWindow);
};
#endif /* _WWINDOW_H_ */
