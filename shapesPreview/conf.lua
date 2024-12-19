additionalW = 200
additionalH = 100


function love.conf(t)
    t.window.title = "PSP SHape draw Preview calc"
    t.window.width = 480 + additionalW
    t.window.height = 272 + additionalH
    t.window.resizable = false
    t.window.fullscreen = false
end