cmake_minimum_required(VERSION 3.10)
project(noise)

# Fonte
add_executable(${PROJECT_NAME}
    main.c
)

set(PSPDEV $ENV{PSPDEV})
message(STATUS "PSPDEV is set to: ${PSPDEV}")

# Inclui os headers do PSPSDK
include_directories(
    ${PSPDEV}/psp/include
    ${PSPDEV}/psp/sdk/include  
    ${PSPDEV}/psp/include/SDL
)
# Linka bibliotecas necessárias (incluindo SDL)
target_link_libraries(${PROJECT_NAME}
    SDL
    pspgum
    pspdebug
    pspdisplay
    pspaudio
    pspirdrm
    pspge
    pspgu
    psprtc
    pspctrl
    m
)

# Gera EBOOT
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "EBOOT")

# Instala como EBOOT.PBP
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND pack-pbp EBOOT.PBP ../PARAM.SFO NULL NULL NULL NULL NULL EBOOT NULL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH ../PARAM.SFO # <-- Caminho correto!
    TITLE "noise"
    VERSION 01.00
)