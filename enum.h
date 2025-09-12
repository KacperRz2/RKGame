#ifndef ENUM_H_
#define ENUM_H_

enum texture_num{
    tx_pc,
    tx_compass,
    tx_projectile,
    tx_being,
    tx_viewfinder,
    tx_pc_blade,
    tx_nesw,
    tx_arrow,
    tx_being_blade,
    tx_h_projectile,
    tx_terrain,
    tx_gunsightpart,
    tx_wall,
    tx_portal,
    tx_door,
    tx_box,
    tx_barrier,
    tx_map
};

enum control{
    forward = 1 << 0,
    back = 1 << 1,
    right = 1 << 2,
    left = 1 << 3,
    dodge = 1 << 4,
    run = 1 << 5,
    tmp0 = 1 << 6,
    attack = 1 << 7,
    block = 1 << 8,
    action = 1 << 9,
    range_mode = 1 << 10
};

enum being_status{
    idle,
    follow,
    shoot,
    strike,
    walk,
    dead
};

enum menu_option{
    menu_unknown,
    menu_start,
    menu_quit
};

enum box_content_type{
    box_mp,
    box_coins,
    box_scroll,
    box_key,
    box_map
};

#endif