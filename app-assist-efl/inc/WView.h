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

#ifndef _WVIEW_H_
#define _WVIEW_H_

#include <functional>
#include <memory>
#include <Evas.h>
#include <Elementary.h>
#include "IWUiObject.h"
#include "WNaviframe.h"
#include "WWindow.h"

class WControl;

/**
* @class	WView
* @brief	This class means the view represented by an evas object, which can be pushed into naviframe elementary.
*
* The %WView class keeps an evas object and provides methods to handle the case when the view is pushed into naviframe.
*/
class WAPP_ASSIST_EXPORT WView : public IWUiObject
{
public:
	/**
	 * This is the default constructor for this class.
	 *
	 */
	WView();

	/**
	 * Constructor
	 *
	 * @param[in]	name	the instance name to be set
	 */
	explicit WView( const char* name );

	/**
	 * Gets the class name.
	 *
	 */
	virtual const char* getClassName();

public:
	/**
	 * Creates evas object which represents this view.
	 * "onCreate" will be called subsequently.
	 *
	 * @param[in]	parent	parent evas object of the view
	 * @param[in]	viewParam	user data
	 */
	bool create( Evas_Object* parent, void* viewParam );

	/**
	 * Destroys the class instance and deletes evas object.
	 * If it has been pushed into naviframe, it will be popped out without onPop method being called.
	 *
	 */
	void destroy();

	/**
	 * Gets the evas object of the class instance.
	 *
	 * @return the pointer to the evas object
	 */
	Evas_Object* getEvasObj();

	/**
	 * Sets the name of the class instance.
	 *
	 * @param[in]	name	instance name
	 *
	 * @see getName()
	 */
	void setName( const char* name );

	/**
	 * Gets the name of the class instance
	 *
	 * @see setName()
	 */
	const char* getName();

	/**
	 * Sets the title string of the view. Only on intial creation time, it is applied to the evas object.
	 *
	 * @param[in]	title	string of the title to be set
	 *
	 */
	void setTitle( const char* title );

	/**
	 * Gets the title string
	 *
	 * @return title string
	 */
	const char* getTitle();
	/**
	 * Returns weak pointer of this instance to refer it safe.
	 *
	 * @return weak pointer of this class instance
	 */
	WUiObjectPtr getWeakPtr();
	/**
	 * Attaches popup to the view.It means that it binds them to have same life-time.
	 * Internally create method of the pop-up will be called, if it is not yet created.
	 * If previous pop-up exists, it will be destroyed.
	 *
	 * @param[in]	popup pop-up instance pointer to be attached
	 *
	 * @see destroyPopup()
	 */
	void attachPopup( WControl* popup );
	/**
	 * Destroys pop-up. Use this method when you want to destroy pop-up explicitly.
	 * Pop-up will be deleted when the view is deleted, although this method is not called.
	 *
	 * @see attachPopup()
	 */
	void destroyPopup();

public:
	/**
	 * Sets the handler function pointer to override virtual function onCreate without defining a new inherited class.
	 * If it is set, onCreate method will not be called.
	 *
	 * @param[in]	handlerFunc	The pointer to handler function
	 * @see onCreate()
	 */
	void setOnCreate( const std::function<Evas_Object* (Evas_Object*, void*)>& handlerFunc );

	/**
	 * Sets the handler function pointer to override virtual function onPushed without defining a new inherited class.
	 * If it is set, onPushed method will not be called.
	 *
	 * @param[in]	handlerFunc	The pointer to handler function
	 * @see onPushed()
	 */
	void setOnPushed( const std::function<void (Elm_Object_Item*)>& handlerFunc );

protected:

	/**
	 * Called when create method is called.
	 * Override this method to create your own view.
	 *
	 * @param[in]	parent	parent evas object
	 * @param[in]	viewParam	user data
	 *
	 * @return the most parent evas object created in this method. It will be related to the class instance.
	 */
	virtual Evas_Object* onCreate( Evas_Object* parent, void* viewParam );
	/**
	 * Override this method to do something after evas object is created.
	 *
	 * @see create()
	 * @see onCreate()
	 */
	virtual void onCreated(){};
	/**
	 * Called when destroy method is called or evas object is deleted.
	 * Override this method to do something on destruction.
	 *
	 */
	virtual void onDestroy();

	/**
	 * Called when this view instance evas object should be pushed into naviframe.
	 * Override this method to do something and push the view into naviframe by itself.
	 *
	 * @param[in]	naviFrame	naviframe evas object
	 *
	 * @return navi item resulted from pushing the view to the naviframe.
	 * @see %WNaviframe::push()
	 * @see onPushed()
	 */
	virtual Elm_Object_Item* onPushToNaviframe( Evas_Object* naviFrame);

	/**
	 * Called when this view instance evas object is pushed into naviframe.
	 * Override this method to do something with navi item.
	 *
	 * @param[in]	naviItem	navi item resulted from naviframe push
	 * @see %WNaviframe::push()
	 * @see onPushToNaviframe()
	 */
	virtual void onPushed(Elm_Object_Item* naviItem);

	/**
	 * Called when the view evas object is popped from naviframe.
	 * Override this method to do something before it is popped out.
	 *
	 * @return false, if you want to keep the view instead of being popped out.
	 */
	virtual bool onPop();
	/**
	 * Gets the WWindow class instance.
	 *
 	 * @return The pointer to the window. If the view is the part of UG module, NULL will be returned.
	 */
	WWindow* getWindow();

	/**
	 * Gets the WNaviframe class instance.
	 *
	 * @return the pointer to the WNaviframe class instance. If this view instance is not pushed into naviframe, it will be NULL.
	 */
	WNaviframe* getNaviframe();

	/**
	 * Gets a navi item.
	 *
	 * @return The pointer to navi item. If this view instance is not pushed into naviframe, it will be NULL.
	 */
	Elm_Object_Item* getNaviItem();

	/**
	 * Pops out this view instance evas object from naviframe.
	 *
	 * @return true, if succeeded
	 */
	bool popOut();
	/**
	 * Create more button on navi item.
	 *
	 * @param[in]	naviItem
	 * @param[in]	clickedCb
	 * @param[in]	userData
	 * @see %WNaviframe::push()
	 * @see onPushed()
	 */
	bool enableMoreButton( Elm_Object_Item* naviItem, Evas_Smart_Cb clickedCb, void* userData );

	//
protected:
	/**
	 * This is the destructor for this class.
	 *
	 */
	virtual ~WView();

private:
	bool __callPushedHandlerFunc(Elm_Object_Item* naviItem);
	void __destroy();

private:
	WDECLARE_PRIVATE_IMPL(WView);
	WDISABLE_COPY_AND_ASSIGN(WView);
	friend class WNaviframe;
	friend Evas_Object* WView_getWindowBaseLayoutEvasObj( WView* view );
	friend Evas_Object* WView_getWindowEvasObj( WView* view );
	friend void WView_setContainerProperties( WView* view, WWindow* win, WNaviframe* naviframe );
	friend void WView_setContainerNaviItemProperty( WView* view, Elm_Object_Item* item );
};


/**
 * Get the view class instance related to evas object.
 *
 * @param[in]	obj	evas object
 *
 * @return NULL, if no view class instance is related.
 */
WAPP_ASSIST_EXPORT WView* WView_getInstanceFromEvasObj(Evas_Object* obj);
WAPP_ASSIST_EXPORT Evas_Object* WView_getWindowBaseLayoutEvasObj( WView* view );
WAPP_ASSIST_EXPORT Evas_Object* WView_getWindowEvasObj( WView* view );
/**
 * Sets the container class properties to the view.
 * It will be used only to create your own container view class and set its properties to child views, like WPageSlideView.
 *
 * @param[in]	view	WView instance to set these properties
 * @param[in]	win		WWindow instance
 * @param[in]	naviframe	WNaviframe instance
 *
 */
WAPP_ASSIST_EXPORT void WView_setContainerProperties( WView* view, WWindow* win, WNaviframe* naviframe );
/**
 * Sets the container class navi item property to the view.
 * It will be used only to create your own container view class and set its properties to child views, like WPageSlideView.
 *
 * @param[in]	view	WView instance to set these properties
 * @param[in]	item	navi item pointer of container view which is pushed into naviframe.
 *
 */
WAPP_ASSIST_EXPORT void WView_setContainerNaviItemProperty( WView* view, Elm_Object_Item* item );


#endif /* _WVIEW_H_ */
