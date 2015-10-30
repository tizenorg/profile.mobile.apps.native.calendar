SET(MO_REQUIRED_FILES
    ar
    cs
    da
    de
    el_GR
    en
    en_PH
    en_US
    es_ES
    es_US
    et
    eu
    fi
    fr
    fr_CA
    ga
    gl
    hi
    hr
    hu
    hy
    is
    it_IT
    ja_JP
    ka
    kk
    ko_KR
    lt
    lv
    mk
    nb
    nl
    pl
    pt_BR
    pt_PT
    ro
    ru_RU
    sk
    sl
    sr
    sv
    tr_TR
    uk
    uzca
    zh_CN
    zh_HKaz
    zh_TWbg
)

FILE(GLOB PO_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.po")
SET(MSGFMT "/usr/bin/msgfmt")

MESSAGE(STATUS "po files ---")
FOREACH(pofile ${PO_FILES})
	SET(pofile ${CMAKE_CURRENT_SOURCE_DIR}/${pofile})
	MESSAGE(STATUS "  ${pofile}")
	GET_FILENAME_COMPONENT(absPofile ${pofile} ABSOLUTE)
	GET_FILENAME_COMPONENT(lang ${absPofile} NAME_WE)
	SET(moFile ${CMAKE_CURRENT_BINARY_DIR}/${lang}.mo)
	ADD_CUSTOM_COMMAND(
		OUTPUT ${moFile}
		COMMAND ${MSGFMT} -o ${moFile} ${absPofile}
		DEPENDS ${absPofile}
	)
	INSTALL(FILES ${moFile}
		DESTINATION ${LOCALEDIR}/${lang}/LC_MESSAGES RENAME ${PROJECT_NAME}.mo)
	SET(moFiles ${moFiles} ${moFile})
ENDFOREACH(pofile)

#install
FOREACH(mofile ${MO_REQUIRED_FILES})
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${mofile}.po)
		INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/en.mo
		DESTINATION ${LOCALEDIR}/${mofile}/LC_MESSAGES RENAME ${PROJECT_NAME}.mo)
	endif()
ENDFOREACH(mofile)

ADD_CUSTOM_TARGET(po ALL DEPENDS ${moFiles})
