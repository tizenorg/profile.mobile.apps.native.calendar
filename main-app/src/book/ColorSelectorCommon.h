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

#ifndef _CAL_COLOR_SELECTOR_COMMON_H_
#define _CAL_COLOR_SELECTOR_COMMON_H_

// standard 16-color palette
const unsigned int colors[] =
{
    0x000000FF,
    0xC0C0C0FF,
    0x808080FF,
    0xFFA500FF,
    0x800000FF,
    0xFF0000FF,
    0x800080FF,
    0xFF00FFFF,
    0x008000FF,
    0x00FF00FF,
    0x808000FF,
    0xFFFF00FF,
    0x000080FF,
    0x0000FFFF,
    0x008080FF,
    0x00FFFFFF
};

static const int colors_size = sizeof(colors)/sizeof(unsigned int);

#define R_MASK(val) (((val) & 0xff000000) >> 24)
#define G_MASK(val) (((val) & 0x00ff0000) >> 16)
#define B_MASK(val) (((val) & 0x0000ff00) >> 8)
#define A_MASK(val) (((val) & 0x000000ff))

#endif

