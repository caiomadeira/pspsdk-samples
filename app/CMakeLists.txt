cmake_minimum_required(VERSION 3.5)

project(PSPGAME)

add_executable(${PROJECT_NAME} 
    main.c
    include/player.c
    include/audio.c
)

# include(FindPkgConfig)
# pkg_search_module(SDL3 REQUIRED sdl3)
# if(NOT PSP)
# 	pkg_search_module(GL REQUIRED gl)
# endif()

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_image_INCLUDE_DIRS}
    ${SDL3_ttf_INCLUDE_DIRS}
    $ENV{PSPDEV}/psp/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    $ENV{PSPDEV}/psp/lib
)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${SDL3_LIBRARIES}
    ${SDL3_image_LIBRARIES}
    ${SDL3_ttf_LIBRARIES}

    SDL3 
    SDL3_image
    SDL3_ttf

    # dependencias do SDL_mixer
    vorbis # aqui é dependencia vorbis
    ogg # aqui é arquivos .ogg
    modplug # para formatos de músicas tracker

    # DEPENDÊNCIAS DO SDL_TTF
    harfbuzz
    freetype

    png
    jpeg
    z
    bz2 # dependencia de SDL_TFF - bz2 eh a abreviação do linker para BZIP2

    pspctrl 
    pspgum 
    pspgu 
    pspdisplay 
    pspdebug
    m
)


if(NOT PSP)
	target_include_directories(${PROJECT_NAME} PRIVATE ${GL_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} PRIVATE ${GL_LIBRARIES})
endif()

if(PSP)
	create_pbp_file(
		TARGET ${PROJECT_NAME}
		ICON_PATH NULL
		BACKGROUND_PATH NULL
		PREVIEW_PATH NULL
		TITLE ${PROJECT_NAME}
	)
endif()