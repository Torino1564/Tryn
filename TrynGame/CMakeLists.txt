file(GLOB_RECURSE TRYNGAME_SOURCES
    CONFIGURE_DEPENDS
    *.cpp
    *.h
    *.hpp
)

set(SOURCES ${TRYNGAME_SOURCES})

add_executable(TrynGame WIN32 ${SOURCES})

target_link_libraries(TrynGame PRIVATE
    Core
    TrynGlobalSettings
)