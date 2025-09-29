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
    tx_stun,
    tx_scroll,
    tx_being1,
    tx_weapon,
    tx_being2,
    tx_terrain1,
    tx_void,
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
    range_mode = 1 << 10,
    cast = 1 << 11
};

enum being_status{
    being_follow,
    being_shoot,
    being_strike,
    being_idle,
    being_walk,
    being_dead,
    being_stunned,
    being_fly,
    being_search,
    being_attack_being
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

enum scroll_type{
    scroll_0,
    scroll_1,
    scroll_2,
    scroll_3,
    scroll_4,
    scroll_5,
    scroll_6,
    scroll_empty
};

enum being_type{
    being_0,
    being_1,
    being_2,
    being_3,
    being_4,
    being_5,
    being_6,
    being_7
};

enum being_effect{
    being_effect_0,
    being_effect_1,
    being_no_effect
};

enum pc_effect{
    pc_effect_0,
    pc_effect_1,
    pc_no_effect
};

enum game_effect{
    game_effect_0,
    game_effect_1,
    game_no_effect
};

enum projectile_type{
    projectile_penetrat,
    projectile_hostile,
    projectile_special
};

enum segment_flags{
    segment_in_sight = 1 << 0,
    segment_known = 1 << 1
};

#endif