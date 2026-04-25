add_executable(KGame main.c Being.c event.c function.c game.c Player.c Projectile.c render.c Scroll.c World.c)

target_link_libraries(KGame PRIVATE SDL3::SDL3-static)

target_include_directories(KGame PRIVATE .)
