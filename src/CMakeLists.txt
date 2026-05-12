add_executable(KGame)

target_sources(KGame
  PRIVATE main.c Being.c event.c function.c game.c Player.c Projectile.c render.c Scroll.c World.c
  PRIVATE FILE_SET HEADERS
)

target_link_libraries(KGame PRIVATE SDL3-static enet)
target_include_directories(KGame PRIVATE ../vendored/enet/include)

set_target_properties(KGame PROPERTIES UNITY_BUILD ON)
