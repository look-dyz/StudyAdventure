cmake_minimum_required(VERSION 3.16)

project(StudyAdventure
    VERSION 0.1.0
    DESCRIPTION "Study Adventure: The Informatics Girl in Another World"
    HOMEPAGE_URL "https://github.com/your-team/StudyAdventure"
    LANGUAGES CXX
)

# ===== C++ 标准 =====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Qt 自动化 =====
set(CMAKE_AUTOMOC ON)   # 自动处理 Q_OBJECT 宏
set(CMAKE_AUTORCC ON)   # 自动编译 .qrc 资源
set(CMAKE_AUTOUIC ON)   # 自动编译 .ui 文件

# ===== 构建类型默认 Release =====
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ===== 查找 Qt 6 =====
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
)

message(STATUS "")
message(STATUS "════════════════════════════════════════")
message(STATUS "  Study Adventure ${PROJECT_VERSION}")
message(STATUS "  Qt version    : ${Qt6_VERSION}")
message(STATUS "  Qt directory  : ${Qt6_DIR}")
message(STATUS "  Build type    : ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ compiler  : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "════════════════════════════════════════")
message(STATUS "")

# ===== 源文件 =====
set(PROJECT_SOURCES
    # 入口
    src/main.cpp

    # 公共基类
    src/common/MiniGame.h
    src/common/Subject.h
    src/common/Subject.cpp
    src/common/Constants.h

    # 核心系统（成员 C）
    src/core/Player.h
    src/core/Player.cpp
    src/core/GameManager.h
    src/core/GameManager.cpp
    src/core/SaveManager.h
    src/core/SaveManager.cpp

    # 剧情系统（成员 A）
    src/story/StoryEngine.h
    src/story/StoryEngine.cpp
    src/story/DialogWindow.h
    src/story/DialogWindow.cpp
    src/story/EndingJudge.h
    src/story/EndingJudge.cpp
    src/story/Subjects.h
    src/story/Subjects.cpp

    # 界面（成员 B）
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/MainMenu.h
    src/ui/MainMenu.cpp
    src/ui/StatusBar.h
    src/ui/StatusBar.cpp

    # 地图（成员 B）
    src/map/MapScene.h
    src/map/MapScene.cpp
    src/map/LocationItem.h
    src/map/LocationItem.cpp

    # 小游戏（成员 C）
    src/games/MinesweeperGame.h
    src/games/MinesweeperGame.cpp
    src/games/BlackjackGame.h
    src/games/BlackjackGame.cpp
    src/games/TicTacToeGame.h
    src/games/TicTacToeGame.cpp
    src/games/MatrixGame.h
    src/games/MatrixGame.cpp
    src/games/MazeGame.h
    src/games/MazeGame.cpp
    src/games/CalculusGame.h
    src/games/CalculusGame.cpp
)

# ===== 资源文件 =====
set(PROJECT_RESOURCES assets/resources.qrc)

# ===== 可执行文件 =====
qt_add_executable(StudyAdventure
    ${PROJECT_SOURCES}
    ${PROJECT_RESOURCES}
)

# ===== 链接 Qt 库 =====
target_link_libraries(StudyAdventure PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
)

# ===== 包含目录 =====
target_include_directories(StudyAdventure PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ===== 平台特定配置 =====
if(WIN32)
    # Windows 下隐藏控制台窗口（Release 模式）
    set_target_properties(StudyAdventure PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(APPLE)
    # macOS 下打包为 .app
    set_target_properties(StudyAdventure PROPERTIES MACOSX_BUNDLE TRUE)
endif()

# ===== 编译警告 =====
if(MSVC)
    target_compile_options(StudyAdventure PRIVATE /W4 /utf-8)
else()
    target_compile_options(StudyAdventure PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ===== 安装规则（可选）=====
install(TARGETS StudyAdventure
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
)
