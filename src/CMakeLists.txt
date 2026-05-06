add_executable(KGame)

target_sources(KGame
  PRIVATE main.c Being.c event.c function.c game.c Player.c Projectile.c render.c Scroll.c World.c
  PRIVATE FILE_SET HEADERS
)

target_link_libraries(KGame PRIVATE SDL3-static)

set_target_properties(KGame PROPERTIES UNITY_BUILD ON)
