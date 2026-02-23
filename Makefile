TARGET = MagicArena.exe
CXX = g++

CXXFLAGS = -O2 -std=c++17

INCLUDE_DIR = -I SDL/x86_64-w64-mingw32/include
LIB_DIR = -L SDL/x86_64-w64-mingw32/lib
LIBS = -lSDL3

SRCS = src/main.cpp \
       src/core/Game.cpp \
       src/core/Window.cpp \
       src/entities/Player.cpp \
       src/entities/Projectile.cpp \
       src/entities/Platform.cpp \
       src/entities/Item.cpp \
       src/systems/AmmoSystem.cpp \
       src/systems/RenderSystem.cpp \
       src/systems/CollisionSystem.cpp \
       src/systems/PhysicsSystem.cpp \
       src/entities/WindColumn.cpp \


all:
	$(CXX) $(SRCS) $(CXXFLAGS) $(INCLUDE_DIR) $(LIB_DIR) $(LIBS) -o $(TARGET)

clean:
	del $(TARGET)