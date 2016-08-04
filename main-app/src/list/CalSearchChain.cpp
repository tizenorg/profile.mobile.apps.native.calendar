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

#include "CalSearchChain.h"
#include "CalCommon.h"

CalSearchChain::Node::Node() :
	__node(NULL)
{
}

CalSearchChain::Node::~Node()
{
}

CalSearchChain::Node *CalSearchChain::Node::getNext()
{
	GList *next = g_list_next(__node);
	return next ? (Node *)next->data : NULL;
}

CalSearchChain::Node *CalSearchChain::Node::getPrevious()
{
	GList *prev = g_list_previous(__node);
	return prev ? (Node *)prev->data : NULL;
}

CalSearchChain::CalSearchChain() :
	__head(NULL),
	__tail(NULL)
{
}

CalSearchChain::~CalSearchChain()
{
}

void CalSearchChain::append(Node *node)
{
	if (__tail == NULL) {
		__tail = g_list_append(__tail, node);
	} else {
		__tail = g_list_append(__tail, node);
		__tail = g_list_next(__tail);
	}
	node->__node = __tail;

	if (__head == NULL) {
		__head = __tail;
	}
}

void CalSearchChain::prepend(Node *node)
{
	node->__node = __head = g_list_prepend(__head, node);
	if (__tail == NULL) {
		__tail = __head;
	}
}
