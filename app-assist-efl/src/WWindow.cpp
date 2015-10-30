/*
 * app-assist-efl
 *
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd. All rights reserved.
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

#include <Elementary.h>
#include <app.h>
#include "WWindow.h"
#include "IWUiObject.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"
#include <string>

#define __CLASS_NAME	"WWindow"

class __WWindowImpl
{
public:
	__WWindowImpl(const char* inName, Elm_Win_Type inWinType);
	__WWindowImpl(Evas_Object* win, Evas_Object* conform, Evas_Object* baseLayout);
	~__WWindowImpl();
public:
	//
	Evas_Object* __win;
	Evas_Object* __baseLayout;
	Evas_Object* __conformant;
	IWUiObject* __baseObj;
	std::string __name;
	Elm_Win_Type __winType;
	std::function<Evas_Object* ()> __createWinHandler;
	std::function<Evas_Object* (Evas_Object*, Evas_Object** )> __createBaseLayoutHandler;
	bool __owner;

	static void __delReqCb( void* data, Evas_Object* obj, void* event_info);
	static void __objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info);
};

__WWindowImpl::__WWindowImpl(const char* name, Elm_Win_Type winType):
	__win(NULL),
	__baseLayout(NULL),
	__conformant(NULL),
	__baseObj(NULL),
	__name(),
	__winType(winType),
	__createWinHandler(NULL),
	__createBaseLayoutHandler(NULL),
	__owner(true)
{
	WASSERT( name );
	__name = name;
}

__WWindowImpl::__WWindowImpl(Evas_Object* win, Evas_Object* conform, Evas_Object* baseLayout):
	__win(win),
	__baseLayout(baseLayout),
	__conformant(conform),
	__baseObj(NULL),
	__name(),
	__winType(ELM_WIN_UNKNOWN),
	__createWinHandler(NULL),
	__createBaseLayoutHandler(NULL),
	__owner(false)
{
	if(win)
	{
		__winType = elm_win_type_get(win);
		const char *title = elm_win_title_get(win);
		if(title)
			__name = title;
		evas_object_data_set( __win, WKEY_WINDOW_INSTANCE, this ); // ?
	}
}

__WWindowImpl::~__WWindowImpl()
{
	if(__win && !__owner)
		evas_object_data_del( __win, WKEY_WINDOW_INSTANCE);
}


void __WWindowImpl::__delReqCb( void* data, Evas_Object* obj, void* event_info)
{
	WHIT();
	elm_exit();
}
void __WWindowImpl::__objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	WHIT();

	auto win = (WWindow*)data;

	win->onDestroy();
	delete win;
}


WWindow::WWindow(const char* name, Elm_Win_Type winType)
{
	__pv = new __WWindowImpl(name, winType);
}

WWindow::WWindow(Evas_Object* win, Evas_Object* conform, Evas_Object* baseLayout)
{
	__pv = new __WWindowImpl(win, conform, baseLayout);
}

WWindow::~WWindow()
{
	WHIT();
	delete __pv;
}

const char* WWindow::getClassName()
{
	return __CLASS_NAME;
}

bool WWindow::create()
{
	if(__pv->__win || !__pv->__owner)
	{
		WDEBUG("Already created!");
		return true;
	}

	Evas_Object* win = NULL;
	if( __pv->__createWinHandler )
		win = __pv->__createWinHandler();
	else
		win = onCreateWin();
	__pv->__win = win;
	if( win )
	{
		Evas_Object* cf = NULL;
		if(__pv->__createBaseLayoutHandler)
			__pv->__baseLayout = __pv->__createBaseLayoutHandler(win,&cf);
		else
			__pv->__baseLayout = onCreateBaseLayout(win, &cf);
		__pv->__conformant = cf;
		evas_object_data_set( win, WKEY_WINDOW_INSTANCE, this );
		evas_object_smart_callback_add( win, "delete,request", __WWindowImpl::__delReqCb, this);
		evas_object_event_callback_add( win, EVAS_CALLBACK_DEL, __WWindowImpl::__objDelCb, this);

		evas_object_show( win );

		return true;
	}
	else
	{
		return false;
	}
}

void WWindow::destroy()
{
	if(__pv->__owner)
	{
		if( __pv->__win)
		{
			evas_object_del(__pv->__win);
			// Do not leave any code here.
			// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!
		}
		else
		{
			onDestroy();
			delete this;
		}
	}
	else
	{
		delete this;
	}
}

bool WWindow::isOwner() const
{
	return __pv->__owner;
}

Evas_Object* WWindow::getEvasObj()
{
	return __pv->__win;
}

Evas_Object* WWindow::getBaseLayoutEvasObj()
{
	return __pv->__baseLayout;
}

Evas_Object* WWindow::getConformantEvasObj()
{
	return __pv->__conformant;
}

bool WWindow::attachBaseUiObject( IWUiObject* baseObj, void* creationParam )
{

	WASSERT( __pv->__baseObj == NULL);

	if( baseObj->create( __pv->__baseLayout, creationParam) == false )
	{
		return false;
	}

	__pv->__baseObj = baseObj;

	evas_object_data_set( baseObj->getEvasObj(), WKEY_WINDOW_INSTANCE, this );

	return true;
}

IWUiObject* WWindow::getBaseUiObject()
{
	return __pv->__baseObj;
}

IWUiObject* WWindow::detachBaseUiObject()
{
	IWUiObject* obj = __pv->__baseObj;
	__pv->__baseObj = NULL;
	return obj;
}

void WWindow::destroyBaseUiObject()
{
	if( __pv->__baseObj )
		__pv->__baseObj->destroy();

	__pv->__baseObj = NULL;
}

const char* WWindow::getName()
{
	return __pv->__name.c_str();
}
Elm_Win_Type WWindow::getWinType()
{
	return __pv->__winType;
}

void WWindow::setOnCreateWin( const std::function<Evas_Object* ()>& handlerFunc )
{
	__pv->__createWinHandler = handlerFunc;
}
void WWindow::setOnCreateBaseLayout( const std::function<Evas_Object* (Evas_Object*, Evas_Object** )>& handlerFunc )
{
	__pv->__createBaseLayoutHandler = handlerFunc;

}

Evas_Object* WWindow::onCreateWin()
{
	Evas_Object* win = elm_win_add( NULL, getName(), getWinType() );
	if( win == NULL)
	{
		WERROR("Fail to create win!");
		return NULL;
	}

	elm_win_title_set( win, getName() );
	elm_win_conformant_set( win, EINA_TRUE);
	elm_win_autodel_set( win, EINA_TRUE);
	if( elm_win_wm_rotation_supported_get(win))
	{
		int rotation[4] = {0,90,180,270};
		elm_win_wm_rotation_available_rotations_set( win, (const int*)(&rotation), 4);

	}
	elm_win_indicator_mode_set( win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_OPAQUE);


	return win;
}

Evas_Object* WWindow::onCreateBaseLayout( Evas_Object* win, Evas_Object** outConformant )
{
	Evas_Object* bg = elm_bg_add( win );

	evas_object_size_hint_weight_set( bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add( win, bg);
	evas_object_show(bg);

	// If window has no conformant object, ug creation is failed.
	Evas_Object* cf = elm_conformant_add( win);

	evas_object_size_hint_weight_set( cf, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add( win, cf);
	evas_object_show(cf);

	Evas_Object* cf_bg = elm_bg_add( cf);
	elm_object_style_set( cf_bg, "indicator/headerbg");
	elm_object_part_content_set( cf, "elm.swallow.indicator_bg", cf_bg);
	evas_object_show(cf_bg);


	Evas_Object* layout = elm_layout_add( cf);
	elm_layout_theme_set( layout, "layout", "application", "default");
	evas_object_size_hint_weight_set( layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set( cf, layout);

	evas_object_show(layout);

	*outConformant = cf;

	return layout;
}

void WWindow::onDestroy()
{

}

