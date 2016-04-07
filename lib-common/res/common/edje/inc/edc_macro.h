/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef COMMON_EDC_MACRO_H
#define COMMON_EDC_MACRO_H

//**************************************************************************//
// Generic Macros, EDJE description have 24 patterns
//**************************************************************************//
// ------------------
// | --> HERE
// ------------------
 #define DSC_FROM_L( STATE, PARAM ) \
	description { state: STATE; \
		fixed: 1 0; \
		align: 0.0 0.5; \
		rel2.relative: 0.0 1.0; \
		PARAM \
	}

// ------------------
//         HERE <-- |
// ------------------
#define DSC_FROM_R( STATE, PARAM ) \
	description { state: STATE; \
		fixed: 1 0; \
		align: 1.0 0.5; \
		rel1.relative: 1 0; \
		PARAM \
	}

// ----------
// |  HERE  |
// ----------
// |        |
// ----------
#define DSC_FROM_T( STATE, PARAM ) \
	description { state: STATE; \
		fixed: 0 1; \
		align: 0.5 0.0; \
		rel2.relative: 1.0 0.0; \
		PARAM \
}

// ----------
// |  HERE  |
// ----------
// |        |
// ----------
#define DSC_FROM_B( STATE, PARAM ) \
	description { state: STATE; \
		fixed: 0 1; \
		rel1.relative: 0.0 1.0; \
		align: 0.5 1.0; \
		PARAM \
}

// ------------------
// |  PART == HERE  |
// ------------------
 #define DSC_TO( STATE, PART, PARAM ) \
	description { state: STATE; \
		rel1 { \
			relative: 0 0; \
			to: PART; \
		} \
		rel2 { \
			relative: 1 1; \
			to: PART; \
		} \
		PARAM \
	}

// ---------------------
// |        TOP        |
// ---------------------
// LEFT |  HERE  | RIGHT
// ---------------------
// |      BOTTOM       |
// ---------------------
#define DSC_LRTB( STATE, LEFT, RIGHT, TOP, BOTTOM, PARAM ) \
	description { state: STATE; \
		rel1 { \
			relative: 1 1; \
			to_x: LEFT; \
			to_y: TOP; \
		} \
		rel2 { \
			relative: 0 0; \
			to_x: RIGHT; \
			to_y: BOTTOM; \
		} \
		PARAM \
	}

// ---------------------
// LEFT |--> HERE
// ---------------------
#define DSC_L( STATE, LEFT, PARAM ) \
	description { state: STATE; \
		align: 0 0.5; \
		fixed: 1 0; \
		rel1 { \
			relative: 1 0; \
			to_x: LEFT; \
		} \
		rel2 { \
			relative: 1 1; \
			to_x: LEFT; \
		} \
		PARAM \
	}

// ---------------------
//   HERE  <--| RIGHT
// ---------------------
#define DSC_R( STATE, RIGHT, PARAM ) \
	description { state: STATE; \
		align: 1 0.5; \
		fixed: 1 0; \
		rel1 { \
			relative: 0 0; \
			to_x: RIGHT; \
		} \
		rel2 { \
			relative: 0 1; \
			to_x: RIGHT; \
		} \
		PARAM \
	}

// --------------------------
// LEFT |--> HERE <--| RIGHT
// --------------------------
#define DSC_LR( STATE, LEFT, RIGHT, PARAM ) \
	description { state: STATE; \
		rel1 { \
			relative: 1 0; \
			to_x: LEFT; \
		} \
		rel2 { \
			relative: 0 1; \
			to_x: RIGHT; \
		} \
		PARAM \
	}

// |      TOP      |
// -----------------
// |      HERE  <--| RIGHT
// -----------------
// |     BOTTOM    |
#define DSC_RTB( STATE, RIGHT, TOP, BOTTOM, PARAM ) \
   description { state: STATE; \
		align: 1 0.5; \
		rel1 { \
			relative: 0 1; \
			to_x: RIGHT; \
			to_y: TOP; \
		} \
		rel2 { \
			relative: 0 0; \
			to_x: RIGHT; \
			to_y: BOTTOM; \
		} \
		PARAM \
	}

// LEFT |  HERE  | RIGHT
// ---------------------
//      | BOTTOM |
#define DSC_LRB( STATE, LEFT, RIGHT, BOTTOM, PARAM ) \
	description { state: STATE; \
		align: 0.5 1; \
		rel1 { \
			relative: 1 0; \
			to_x: LEFT; \
			to_y: BOTTOM; \
		} \
		rel2 { \
			relative: 0 0; \
			to_x: RIGHT; \
			to_y: BOTTOM; \
		} \
		PARAM \
	}

//      |   TOP  |
// ---------------------
// LEFT |  HERE  | RIGHT
#define DSC_LRT( STATE, LEFT, RIGHT, TOP, PARAM ) \
	description { state: STATE; \
		align: 0.5 0; \
		rel1 { \
			relative: 1 1; \
			to_x: LEFT; \
			to_y: TOP; \
		} \
		rel2 { \
			relative: 0 1; \
			to_x: RIGHT; \
			to_y: TOP; \
		} \
		PARAM \
	}

//      |    TOP     |
// ---------------------
// LEFT | -->  HERE  |
#define DSC_LT( STATE, LEFT, TOP, PARAM ) \
	description { state: STATE; \
		align: 0 0; \
		rel1 { \
			relative: 1 1; \
			to_x: LEFT; \
			to_y: TOP; \
		} \
		rel2 { \
			relative: 1 1; \
			to_x: LEFT; \
			to_y: TOP; \
		} \
		PARAM \
	}

// LEFT |  --> HERE  |
// ---------------------
//      |     BOTTOM |
#define DSC_LB( STATE, LEFT, BOTTOM, PARAM ) \
	description { state: STATE; \
		align: 0 1; \
		rel1 { \
			relative: 1 0; \
			to_x: LEFT; \
			to_y: BOTTOM; \
		} \
		rel2 { \
			relative: 1 0; \
			to_x: LEFT; \
			to_y: BOTTOM; \
		} \
		PARAM \
	}

//      |    TOP   |
// ---------------------
// LEFT |--> HERE  |
// ---------------------
//      |   BOTTOM |
#define DSC_LTB( STATE, LEFT, TOP, BOTTOM, PARAM ) \
	description { state: STATE; \
		align: 0 0.5; \
		rel1 { \
			relative: 1 1; \
			to_x: LEFT; \
			to_y: TOP; \
		} \
		rel2 { \
			relative: 1 0; \
			to_x: LEFT; \
			to_y: BOTTOM; \
		} \
		PARAM \
	}

//     |    TOP   |
// ---------------------
//     |  HERE <--| RIGHT
#define DSC_RT( STATE, RIGHT, TOP, PARAM ) \
	description { state: STATE; \
		align: 1 0; \
		rel1 { \
			relative: 0 1; \
			to_x: RIGHT; \
			to_y: TOP; \
		} \
		rel2 { \
			relative: 0 1; \
			to_x: RIGHT; \
			to_y: TOP; \
		} \
		PARAM \
	}

//  |   HERE <-- | RIGHT
// ---------------------
//  |   BOTTOM   |
#define DSC_RB( STATE, RIGHT, BOTTOM, PARAM ) \
	description { state: STATE; \
		align: 1 1; \
		rel1 { \
			relative: 0 0; \
			to_x: RIGHT; \
			to_y: BOTTOM; \
		} \
		rel2 { \
			relative: 0 0; \
			to_x: RIGHT; \
			to_y: BOTTOM; \
		} \
		PARAM \
	}

// |   TOP  |
// ----------
// |  HERE  |
// ----------
// | BOTTOM |
#define DSC_TB( STATE, TOP, BOTTOM, PARAM ) \
	description { state: STATE; \
		align: 0.5 0.5; \
		rel1 { \
				relative: 0 1; \
				to_y: TOP; \
		} \
		rel2 { \
			relative: 1 0; \
			to_y: BOTTOM; \
		} \
		PARAM \
	}

// |   TOP  |
// ----------
// |  HERE  |
// ----------
// | BOTTOM |
#define DSC_TB( STATE, TOP, PARAM ) \
	description { state: STATE; \
		align: 0.5 1.0; \
		rel1 { \
				relative: 0 1; \
				to_y: TOP; \
		} \
		PARAM \
	}

// |   TOP  |
// ----------
// |  HERE  |
// ----------
#define DSC_T( STATE, TOP, PARAM ) \
	description { state: STATE; \
		align: 0.5 0.0; \
		rel1 { \
			relative: 0 1; \
			to_y: TOP; \
		} \
		rel2 { \
			relative: 1 1; \
			to_y: TOP; \
		} \
		PARAM \
	}

// |   HERE   |
// ------------
// |  BOTTOM  |
// ------------
#define DSC_B( STATE, BOTTOM, PARAM ) \
	description { state: STATE; \
		align: 0.5 1.0; \
		rel1 { \
			relative: 0 0; \
			to_y: BOTTOM; \
		} \
		rel2 { \
			relative: 1 0; \
			to_y: BOTTOM; \
		} \
		PARAM \
	}

// PART
#define PART(TYPE, NAME, DESCRIPION) \
part { name: NAME; \
	scale: 1; \
	type: TYPE; \
	DESCRIPION \
}

#define PADDING_LEFT_NAME "elm.padding.left"
#define PADDING_RIGTH_NAME "elm.padding.right"
#define PADDING_TOP_NAME "elm.padding.top"
#define PADDING_BOTTOM_NAME "elm.padding.bottom"

//pading
#define PART_LIST_PADDINGS(LEFT_SIZE, RIGHT_SIZE, TOP_SIZE, BOTTOM_SIZE) \
PART(SPACER, PADDING_LEFT_NAME, \
	DSC_FROM_L("default" 0.0, \
		min: LEFT_SIZE 0; \
		max: LEFT_SIZE -1; \
	) \
) \
PART(SPACER, PADDING_RIGTH_NAME, \
	DSC_FROM_R("default" 0.0, \
		min: RIGHT_SIZE 0; \
		max: RIGHT_SIZE -1; \
	) \
) \
PART(SPACER, PADDING_TOP_NAME, \
	DSC_FROM_T("default" 0.0, \
		min: 0 TOP_SIZE; \
		max: -1 TOP_SIZE; \
	) \
) \
PART(SPACER, PADDING_BOTTOM_NAME, \
	DSC_FROM_B("default" 0.0,\
		min: 0 BOTTOM_SIZE; \
		max: -1 BOTTOM_SIZE; \
	) \
)

//**************************************************************************//
// Genlist Macros
//**************************************************************************//
#define LIST_PART_BG \
	PART(RECT, "bg", \
	description { state: "default" 0.0; \
			color: 250 250 250 255; /*(Changeable UI) color_class: "B0211";*/ \
		} \
		description { state: "pressed" 0.0; \
			inherit: "default" 0.0; \
			color: 240 240 240 255; /*(Changeable UI) color_class: "B0211P";*/ \
		} \
		description { state: "disabled" 0.0; \
			color: 250 250 250 255; /*(Changeable UI) color_class: "B0211";*/ \
		} \
		description { state: "reorder" 0.0; \
			color: 0 0 0 0; \
		} \
	)

#define LIST_PART_BG_NOCLASS \
	PART(RECT, "bg", \
		description { state: "default" 0.0; \
			color: 255 255 255 255; \
		} \
		description { state: "pressed" 0.0; \
			inherit: "default" 0.0; \
			color: 165 197 210 255; \
		} \
		description { state: "disabled" 0.0; \
			color: 255 255 255 255; \
		} \
		description { state: "reorder" 0.0; \
			color: 0 0 0 0; \
		} \
	)

#define LIST_PART_BG_GROUP \
	PART(RECT, "bg", \
		description { state: "default" 0.0; \
			color: 192 213 222 255; \
		} \
		description { state: "pressed" 0.0; \
			inherit: "default" 0.0; \
			color: 165 197 210 255; \
		} \
		description { state: "disabled" 0.0; \
			color: 255 255 255 255; \
		} \
		description { state: "reorder" 0.0; \
			color: 0 0 0 0; \
		} \
	)

#define LIST_PROGRAM_DEFAULT( TARGET ) \
	program { name: "default"; \
		signal: "elm,state,default"; \
		source: "elm"; \
		action: STATE_SET "default" 0.0; \
		TARGET \
	} \

#define LIST_PROGRAM_DISABLED( TARGET ) \
	program { name: "enabled"; \
		signal: "elm,state,enabled"; \
		source: "elm"; \
		action: STATE_SET "default" 0.0; \
		TARGET \
	} \
	program { name: "disabled"; \
		signal: "elm,state,disabled"; \
		source: "elm"; \
		action: STATE_SET "disabled" 0.0; \
		TARGET \
	}

#define LIST_PROGRAM_SELECT( TARGET ) \
	program { name: "pressed"; \
		signal: "elm,state,selected"; \
		source: "elm"; \
		action: STATE_SET "pressed" 0.0; \
		TARGET \
	} \
	program { name: "unselected"; \
		signal: "elm,state,unselected"; \
		source: "elm"; \
		action: STATE_SET "default" 0.0; \
		TARGET \
	}

#define LIST_PROGRAM_REORDER( TARGET ) \
program { name: "reorder_enabled"; \
	signal: "elm,state,reorder,enabled"; \
	source: "elm"; \
	action: STATE_SET "reorder" 0.0; \
	TARGET \
} \
program { name: "reorder_disabled"; \
	signal: "elm,state,reorder,disabled"; \
	source: "elm"; \
	action: STATE_SET "default" 0.0; \
	TARGET \
}

#define LIST_PROGRAM_SOUND \
program { name: "play_sound"; \
	signal: "elm,state,clicked"; \
	source: "elm"; \
	script { \
	   run_program(PROGRAM:"touch_sound"); \
	} \
} \
program { name: "touch_sound"; \
	action: RUN_PLUGIN "touch_sound"; \
}

#define LIST_PROGRAMS( TARGET ) \
programs { \
	LIST_PROGRAM_DEFAULT( TARGET \
		target: "bg"; \
	) \
	LIST_PROGRAM_DISABLED( TARGET \
		target: "bg"; \
	) \
	LIST_PROGRAM_SELECT( TARGET \
		target: "bg"; \
	) \
	LIST_PROGRAM_REORDER( TARGET \
		target: "bg"; \
	) \
	LIST_PROGRAM_SOUND \
}

#define PARAM_TEXT(STYLE) \
text { \
	min: 0 1; \
	style: STYLE; \
}

#define LIST_PROGRAM_PAD \
program { name: "left_default"; \
	signal: "elm,pad,left,default"; \
	source: ""; \
	action: STATE_SET "default" 0.0; \
	target: "elm.padding.center.left"; \
} \
program { name: "left_enabled"; \
	signal: "elm,pad,left,enabled"; \
	source: ""; \
	action: STATE_SET "enabled" 0.0; \
	target: "elm.padding.center.left"; \
} \
program { name: "right_default"; \
	signal: "elm,pad,right,default"; \
	source: ""; \
	action: STATE_SET "default" 0.0; \
	target: "elm.padding.center.right"; \
} \
program { name: "right_enabled"; \
	signal: "elm,pad,right,enabled"; \
	source: ""; \
	action: STATE_SET "enabled" 0.0; \
	target: "elm.padding.center.right"; \
}

#define STYLE_TAGS \
   tag:  "br" "\n";\
   tag:  "ps" "ps";\
   tag:  "tab" "\t";\
   tag:  "b" "+ font_weight=Bold";

#endif /* COMMON_EDC_MACRO_H */
