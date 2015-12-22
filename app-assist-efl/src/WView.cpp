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

#include "WView.h"
#include "WControl.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"
#include <app.h>
#include <Elementary.h>
#include <memory.h>
#include <string>


#define __CLASS_NAME	"WView"


class __WViewImpl
{
public:
	__WViewImpl();
	~__WViewImpl();
public:
	//
	Evas_Object* __obj;
	char* __name;
	char* __title;
	Elm_Object_Item* __naviItem;
	WWindow* __window;
	WNaviframe* __naviframe;
	//
	std::function<Evas_Object* (Evas_Object*, void*)> __createHandler;
	std::function<void (Elm_Object_Item*)> __pushedHandler;

	std::shared_ptr<IWUiObject>* __selfPtr;
	std::weak_ptr<IWUiObject>* __popupMonitor;

	static void __objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info);

};
__WViewImpl::__WViewImpl()
{
	__obj = NULL;
	__name = NULL;
	__title = NULL;
	__naviItem = NULL;
	__window = NULL;
	__naviframe = NULL;
	__createHandler = NULL;
	__pushedHandler = NULL;
	__selfPtr = NULL;
	__popupMonitor = NULL;
}

__WViewImpl::~__WViewImpl()
{
	if(__name)
		free(__name);

	if(__title)
		free(__title);

	//
	if( __popupMonitor )
	{
		if( auto p = __popupMonitor->lock() )
		{
			p->destroy();
		}
	}
	delete __popupMonitor;
	delete __selfPtr;
}

void __WViewImpl::__objDelCb( void* data, Evas* evas, Evas_Object* obj, void* event_info)
{
	WHIT();

	auto view = (WView*)data;

	view->__destroy();
}

WView::WView()
{
	WHIT();
	__pv = new __WViewImpl();
}

WView::WView( const char* name ) : __pv( new __WViewImpl() )
{
	WHIT();
	setName( name);
}

WView::~WView()
{
	if( __pv->__name)
	{
		WDEBUG( "name=%s", __pv->__name );
	}
	else
	{
		WHIT();
	}

	delete __pv;
}

const char* WView::getClassName()
{
	return __CLASS_NAME;
}


bool WView::create( Evas_Object* parent, void* viewParam )
{
	WHIT();

	if( __pv->__obj )
	{
		WDEBUG("Already created!");
		return true;
	}
	if( __pv->__createHandler )
		__pv->__obj = __pv->__createHandler( parent, viewParam );
	else
		__pv->__obj = onCreate( parent, viewParam );

	if( __pv->__obj == NULL)
		return false;

	if( __pv->__name ) // for easy debugging
	{
		evas_object_name_set( __pv->__obj, __pv->__name );
	}

	evas_object_data_set( __pv->__obj, WKEY_VIEW_INSTANCE, this );
	evas_object_event_callback_add( __pv->__obj, EVAS_CALLBACK_DEL, __WViewImpl::__objDelCb, this);
	//
	// Do not place this before "evas_object_event_callback_add",
	// The reason is that if additional del callback is added at child class, it should be called first to keep state the class object is still alive.
	onCreated();

	return true;
}

void WView::destroy()
{
	if(__pv->__obj)
	{
		if( getNaviframe() )
		{
			elm_object_item_del( __pv->__naviItem );
		}
		else
		{
			evas_object_del(__pv->__obj);
		}
		// Do not leave any code here.
		// After executing upper statement "evas_object_del", this object will be deleted at evas object deletion callback!
	}
	else
	{
		__destroy();
	}
}

Evas_Object* WView::getEvasObj()
{
	return __pv->__obj;
}

void WView::setName(const char* name)
{
	if( __pv->__name)
	{
		free( __pv->__name);
		__pv->__name = NULL;
	}
	if( name )
	{
		__pv->__name = (char*)malloc( strlen(name)+1);
		strcpy( __pv->__name, name);
	}
}

const char* WView::getName()
{
	return __pv->__name;
}

void WView::setTitle( const char* title )
{
	if( __pv->__title)
	{
		free( __pv->__title);
		__pv->__title = NULL;
	}
	if( title )
	{
		__pv->__title = (char*)malloc( strlen(title)+1);
		strcpy( __pv->__title, title);
	}
}
const char* WView::getTitle()
{
	return __pv->__title;
}

std::weak_ptr<IWUiObject> WView::getWeakPtr()
{
	if( __pv->__selfPtr )
	{
		return std::weak_ptr<IWUiObject>(*__pv->__selfPtr);
	}
	__pv->__selfPtr = new std::shared_ptr<IWUiObject>( this,[](IWUiObject* p){} );
	return std::weak_ptr<IWUiObject>(*__pv->__selfPtr);
}

void WView::setOnCreate( const std::function<Evas_Object* (Evas_Object*, void*)>& handlerFunc )
{
	__pv->__createHandler = handlerFunc;
}

void WView::setOnPushed( const std::function<void (Elm_Object_Item*)>& handlerFunc )
{
	__pv->__pushedHandler = handlerFunc;
}

bool WView::popOut()
{
	if( getEvasObj() == NULL)
	{
		WWARNING("Not created view! No Evas Object!");
		return false;
	}
	if( getNaviframe() == NULL)
	{
		WWARNING("Not pushed to naviframe!");
		return false;
	}
	if( __pv->__naviItem != elm_naviframe_top_item_get(getNaviframe()->getEvasObj()) )
	{
		WASSERT_EX(0,"This view is not on Top of Naviframe!");
		return false;
	}

	destroyPopup(); // Before popping out view, pop-up is destroyed, if it has.Because pop-up is disappeared too late.

	elm_naviframe_item_pop(getNaviframe()->getEvasObj());
	return true;
}


WWindow* WView::getWindow()
{
	return __pv->__window;
}

bool WView::onPop()
{
	return true;
}

WNaviframe* WView::getNaviframe()
{
	if( __pv->__naviframe == NULL)
	{
		WWARNING("naviframe does not exist!");
	}

	return __pv->__naviframe;
}

Elm_Object_Item* WView::getNaviItem()
{
	if( __pv->__naviItem == NULL)
	{
		WWARNING("naviItem does not exist!");
	}

	return __pv->__naviItem;
}

void WView::onPushed(Elm_Object_Item* naviItem)
{

}

Elm_Object_Item* WView::onPushToNaviframe( Evas_Object* naviFrame)
{
	return NULL;
}


Evas_Object* WView::onCreate( Evas_Object* parent, void* viewParam )
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	char* path = app_get_resource_path();
	if( path == NULL )
	{
		WERROR("Failed to get resource path=NULL");
		return layout;
	}
	std::string edjPath(path);
	free(path);
	edjPath += "edje/app-assist-efl.edj";

	if( elm_layout_file_set(layout, edjPath.c_str(), "default-view") == EINA_TRUE)
	{
		Evas_Object* rect = evas_object_rectangle_add(evas_object_evas_get(layout));
		evas_object_color_set(rect, rand() % 256, rand() % 256, rand() % 256, 255);
		evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(rect, EVAS_HINT_FILL, EVAS_HINT_FILL);

		elm_object_part_content_set(layout, "rect", rect);
		if( getName() != NULL )
			elm_object_part_text_set(layout, "name", getName());

	}
	else
	{
		WERROR("Fail to set layout. Check EDJ file(%s)", edjPath.c_str());
	}

	evas_object_show(layout);

	return layout;
}

void WView::onDestroy()
{

}

bool WView::enableMoreButton( Elm_Object_Item* naviItem, Evas_Smart_Cb clickedCb, void* userData )
{
	if( getNaviframe() == NULL )
	{
		WERROR("naviframe is not set");
		return false;
	}
	Evas_Object *btn = elm_button_add( getNaviframe()->getEvasObj() );
	if (!btn) return false;
	// TODO: return back when default style is working again
	//elm_object_style_set(btn, "naviframe/more/default");
	evas_object_show(btn);
	evas_object_smart_callback_add(btn, "clicked", clickedCb, userData);
	elm_object_item_part_content_set(naviItem, "toolbar_more_btn", btn);
	return true;
}

void WView::attachPopup( WControl* popup )
{
	// If the pop-up has been already created, the following statement will just return without creating pop-up.
	Evas_Object* parent = WView_getWindowBaseLayoutEvasObj( this );
	if( parent == NULL )
	{
		WERROR("Parent is NULL! Fail to attach popup");
		return;
	}
	// Base layout should be parent.
	// If the window is parent, the pop-up is displayed bottom of the screen and keyboard overlaps and hides it, if the pop-up has entry field.
	popup->create( parent, NULL );

	// destroy old popup after new popup created, because of edit field of the view re-focusing issue related to IME.
	destroyPopup();

	if( __pv->__popupMonitor == NULL)
		__pv->__popupMonitor = new std::weak_ptr<IWUiObject>;
	*__pv->__popupMonitor = popup->getWeakPtr();
}

void WView::destroyPopup()
{
	if( __pv->__popupMonitor == NULL ) return;

	if( auto p = __pv->__popupMonitor->lock() )
	{
		p->destroy();
		__pv->__popupMonitor->reset();
	}
}

bool WView::__callPushedHandlerFunc(Elm_Object_Item* naviItem)
{
	if( __pv->__pushedHandler )
	{
		__pv->__pushedHandler(naviItem);
		return true;
	}
	return false;
}

void WView::__destroy()
{
	onDestroy();
	delete this;
}

WView* WView_getInstanceFromEvasObj(Evas_Object* obj)
{
	return (WView*)evas_object_data_get(obj, WKEY_VIEW_INSTANCE);
}

Evas_Object* WView_getWindowBaseLayoutEvasObj( WView* view )
{
	if( view->getWindow() )
	{
		return view->getWindow()->getBaseLayoutEvasObj();
	}
	else
	{
		WERROR("Fail to get base layout");
		return NULL;
	}
}

Evas_Object* WView_getWindowEvasObj( WView* view )
{
	if( view->getWindow() )
	{
		return view->getWindow()->getEvasObj();
	}
	else
	{
		WERROR("Fail to get window evas object");
		return NULL;
	}
}

void WView_setContainerProperties( WView* view, WWindow* win, WNaviframe* naviframe)
{
	view->__pv->__window = win;
	view->__pv->__naviframe = naviframe;
}


void WView_setContainerNaviItemProperty( WView* view, Elm_Object_Item* item )
{
	view->__pv->__naviItem = item;
}

