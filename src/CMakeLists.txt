add_executable(welt
        world.cpp
        FlatVector.h
        world.h
        Ientity.h
        Iworld.h
        main.cpp
        material.h
        tile.cpp
        tile.h
        universal.h
        ltimer.h
        ltimer.cpp
        LTexture.h
        LTexture.cpp
        SpriteSet.cpp
        SpriteSet.h
        DisplayIDdef.h
        Sheep.cpp
        Sheep.h
        Wolf.cpp
        Wolf.h ColorList.cpp
        ColorList.h
        resource.cpp
        resource.h
        SpriteInteractionsList.cpp
        SpriteInteractionsList.h
        Iitem.h
        TileMap.cpp
        TileMap.h ItemTestStick.cpp ItemTestStick.h)

target_link_libraries(welt ${SDL2_LIBS} ${SDL2_IMAGE_LIBRARIES})