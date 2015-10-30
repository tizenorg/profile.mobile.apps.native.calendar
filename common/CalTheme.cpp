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

#include "CalTheme.h"

#include "cal_defines.h"


Elm_Theme* CalTheme::__theme = nullptr;

CalTheme::CalTheme()
{
}

CalTheme::~CalTheme()
{
}

void CalTheme::initialize()
{
    setExtension(nullptr);
}

void CalTheme::finalize()
{
    elm_theme_extension_del(__theme, CAL_EDJE);
    elm_theme_extension_add(__theme, CAL_THEME_EDJE);

    if (__theme)
    {
        elm_theme_free(__theme);
        __theme = nullptr;
    }
}

void CalTheme::initializeForUg()
{
    if (__theme)
    {
        return;
    }

    __theme = elm_theme_new();
    elm_theme_ref_set(__theme, nullptr);

    setExtension(__theme);
}

void CalTheme::setToObject(Evas_Object* object)
{
    if (!__theme)
    {
        setExtension(__theme);
    }

    elm_object_theme_set(object, __theme);
}

void CalTheme::setExtension(Elm_Theme* theme)
{
    elm_theme_extension_add(theme, CAL_EDJE);
    elm_theme_extension_add(theme, CAL_THEME_EDJE);
}
