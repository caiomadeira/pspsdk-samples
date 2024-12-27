cmake_minimum_required(VERSION 3.5)

project(psp-test)

# Adiciona uma regra para gerar font.c a partir de font.raw
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/font.c
    COMMAND bin2c ${CMAKE_SOURCE_DIR}/assets/font.raw ${CMAKE_SOURCE_DIR}/src/font.c font
    DEPENDS ${CMAKE_SOURCE_DIR}/assets/font.raw
    COMMENT "Generating font.c from font.raw"
)

# Adiciona uma regra para gerar logo.o a partir de logo.raw
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/logo.o
    COMMAND bin2o -i ${CMAKE_SOURCE_DIR}/assets/logo.raw ${CMAKE_SOURCE_DIR}/src/logo.o logo
    DEPENDS ${CMAKE_SOURCE_DIR}/assets/logo.raw
    COMMENT "Generating logo.o from logo.raw"
)


# Adiciona alvos intermediários para a geração de font.c e logo.o
add_custom_target(generate_font_c DEPENDS ${CMAKE_SOURCE_DIR}/src/font.c)
add_custom_target(generate_logo_o DEPENDS ${CMAKE_SOURCE_DIR}/src/logo.o)

# Define o executável principal do projeto
add_executable(${PROJECT_NAME} 
    src/main.c
    src/common.c
    src/graphic.c
    src/font.c
    src/menu.c
    src/complexgraphics.c
    src/logo.o  # Adiciona logo.o como parte do executável
)

# Faz com que o alvo principal dependa de generate_font_c e generate_logo_o
add_dependencies(${PROJECT_NAME} generate_font_c generate_logo_o)

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