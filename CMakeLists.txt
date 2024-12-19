cmake_minimum_required(VERSION 3.5)

project(psp-test)

# Adiciona uma regra para gerar font.c a partir de font.raw
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/font.c
    COMMAND bin2c ${CMAKE_SOURCE_DIR}/assets/font.raw ${CMAKE_SOURCE_DIR}/src/font.c font
    DEPENDS ${CMAKE_SOURCE_DIR}/assets/font.raw
    COMMENT "Generating font.c from font.raw"
)

# Adiciona um alvo intermediário para a geração de font.c
add_custom_target(generate_font_c DEPENDS ${CMAKE_SOURCE_DIR}/src/font.c)

# Define o executável principal do projeto
add_executable(${PROJECT_NAME} 
    src/main.c
    src/common.c
    src/graphic.c
    src/font.c
    src/util.c
)

# Faz com que o alvo principal dependa de generate_font_c
add_dependencies(${PROJECT_NAME} generate_font_c)

# Vincula as bibliotecas necessárias
target_link_libraries(${PROJECT_NAME} PRIVATE
    pspgu
    pspge
    pspdisplay
    pspgum
    pspdebug
    pspctrl
)

# Cria o arquivo EBOOT.PBP
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)
