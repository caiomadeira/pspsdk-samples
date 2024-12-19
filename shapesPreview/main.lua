-- Configuração inicial
function love.load()
    -- Menu
    menu = {
        x = 0,
        y = 0,
        width = 150, -- Largura do menu lateral
        height = love.graphics.getHeight(),
        fields = {
            { label = "X", value = "", y = 30 },
            { label = "Y", value = "", y = 80 },
            { label = "Width", value = "", y = 130 },
            { label = "Height", value = "", y = 180 },
        },
        button = { x = 25, y = 230, width = 100, height = 30, label = "Add Shape" }
    }

    -- Configuração do espaço de desenho (retângulo principal)
    mainCanvas = {
        x = menu.width + 10,
        y = 10,
        width = 480,
        height = 272
    }

    shapes = {} -- Armazena formas adicionadas
    selectedField = nil -- Campo selecionado para entrada
    font = love.graphics.newFont(12) -- Fonte para o texto
    love.graphics.setFont(font)
end

-- Atualizar lógica do jogo
function love.update(dt)
    -- Nenhuma lógica dinâmica necessária por enquanto
end

-- Desenhar na tela
function love.draw()
    -- Desenhar menu lateral
    love.graphics.setColor(0.2, 0.2, 0.2) -- Cor de fundo do menu
    love.graphics.rectangle("fill", menu.x, menu.y, menu.width, menu.height)

    -- Desenhar campos de texto
    for _, field in ipairs(menu.fields) do
        love.graphics.setColor(1, 1, 1)
        love.graphics.print(field.label .. ":", 10, field.y)
        love.graphics.setColor(0.5, 0.5, 0.5)
        love.graphics.rectangle("fill", 50, field.y, 80, 20)
        love.graphics.setColor(1, 1, 1)
        love.graphics.rectangle("line", 50, field.y, 80, 20)
        love.graphics.print(field.value, 55, field.y + 3)
    end

    -- Desenhar botão
    love.graphics.setColor(0.4, 0.8, 0.4)
    love.graphics.rectangle("fill", menu.button.x, menu.button.y, menu.button.width, menu.button.height)
    love.graphics.setColor(1, 1, 1)
    love.graphics.rectangle("line", menu.button.x, menu.button.y, menu.button.width, menu.button.height)
    love.graphics.print(menu.button.label, menu.button.x + 10, menu.button.y + 8)

    -- Desenhar o retângulo principal (sub-sistema cartesiano)
    love.graphics.setColor(0.3, 0.3, 0.3) -- Cor de fundo do sistema cartesiano
    love.graphics.rectangle("fill", mainCanvas.x, mainCanvas.y, mainCanvas.width, mainCanvas.height)
    love.graphics.setColor(1, 1, 1) -- Borda branca
    love.graphics.rectangle("line", mainCanvas.x, mainCanvas.y, mainCanvas.width, mainCanvas.height)

    -- Desenhar formas dentro do retângulo principal
    for _, shape in ipairs(shapes) do
        -- Ajustar posição para o espaço cartesiano
        love.graphics.setColor(0.6, 0.6, 1)
        love.graphics.rectangle(
            "fill",
            mainCanvas.x + shape.x, -- Posicionar relativo ao retângulo principal
            mainCanvas.y + shape.y,
            shape.width,
            shape.height
        )
    end
end

-- Captura teclas pressionadas para campos de texto
function love.textinput(t)
    if selectedField then
        selectedField.value = selectedField.value .. t
    end
end

-- Detecta backspace
function love.keypressed(key)
    if key == "backspace" and selectedField then
        local value = selectedField.value
        selectedField.value = value:sub(1, #value - 1)
    end
end

-- Detecta cliques do mouse
function love.mousepressed(x, y, button, istouch, presses)
    if button == 1 then
        -- Verificar se clicou em um campo de texto
        selectedField = nil
        for _, field in ipairs(menu.fields) do
            if x >= 50 and x <= 130 and y >= field.y and y <= field.y + 20 then
                selectedField = field
            end
        end

        -- Verificar se clicou no botão
        if x >= menu.button.x and x <= menu.button.x + menu.button.width and
           y >= menu.button.y and y <= menu.button.y + menu.button.height then
            -- Adicionar forma
            local xVal = tonumber(menu.fields[1].value) or 0
            local yVal = tonumber(menu.fields[2].value) or 0
            local widthVal = tonumber(menu.fields[3].value) or 50
            local heightVal = tonumber(menu.fields[4].value) or 50

            -- Limitar valores dentro do sistema cartesiano
            xVal = math.max(0, math.min(xVal, mainCanvas.width - widthVal))
            yVal = math.max(0, math.min(yVal, mainCanvas.height - heightVal))

            table.insert(shapes, { x = xVal, y = yVal, width = widthVal, height = heightVal })
        end
    end
end
