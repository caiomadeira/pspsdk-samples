# Comece a partir da imagem original que você já usa
FROM pspdev/pspdev

# Mude para o usuário root TEMPORARIAMENTE para poder instalar coisas
USER root

# Atualize os pacotes e instale a SDL2 e suas bibliotecas auxiliares com os NOMES CORRETOS E DEFINITIVOS
# --noconfirm evita que o processo peça confirmação
RUN psp-pacman -Syu --noconfirm && \
    psp-pacman -S --noconfirm sdl2 psp-sdl-image psp-sdl-mixer psp-sdl-ttf && \
    psp-pacman -Scc --noconfirm

# IMPORTANTE: Volte para o usuário padrão da imagem para não afetar seu trabalho
USER dev