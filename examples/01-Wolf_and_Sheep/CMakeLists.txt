add_executable(example-01-Wolf_and_Sheep
        ../../src/world.cpp
        ../../include/FlatVector.h
        ../../src/world.h
        ../../src/Ientity.h
        ../../src/Iworld.h
        main.cpp
        ../../src/material.h
        ../../src/tile.cpp
        ../../src/tile.h
        ../../src/universal.h
        ltimer.h
        ltimer.cpp
        LTexture.h
        LTexture.cpp
        SpriteSet.cpp
        SpriteSet.h
        ../../src/DisplayIDdef.h
        Sheep.cpp
        Sheep.h
        Wolf.cpp
        Wolf.h ../../src/ColorList.cpp
        ../../src/ColorList.h
        resource.cpp
        resource.h
        SpriteInteractionsList.cpp
        SpriteInteractionsList.h
        ../../src/Iitem.h
        ../../src/TileMap.cpp
        ../../src/TileMap.h
        ItemTestStick.cpp
        ItemTestStick.h
        ../../src/IObjectSearch.h
        ../../src/ObjectAndData.h
        ../../src/ObjectSearchCircle.h)

target_link_libraries(example-01-Wolf_and_Sheep ${SDL2_LIBS} ${SDL2_IMAGE_LIBRARIES})