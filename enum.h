#ifndef ENUM_H_
#define ENUM_H_

enum control{
    forward = 1 << 0,
    back = 1 << 1,
    right = 1 << 2,
    left = 1 << 3,
    dodge = 1 << 4,
    run = 1 << 5,
    attack = 1 << 6,
    cast = 1 << 7,
    block = 1 << 8,
    action = 1 << 9,
    range_mode = 1 << 10,
    stunned = 1 << 11,
    map_look = 1 << 12,
    dodge_time = 1 << 13,
    tmp = 1 << 14
};

enum being_status{
    being_follow,
    being_shoot,
    being_strike,
    being_attack_being,
    being_follow_being,
    being_shoot_being,
    being_strike_being,
    being_walk,
    being_search,
    being_fly,
    being_idle,
    being_stunned,
    being_in_void,
    being_dead
};

enum menu_option{
    menu_start,
    menu_load,
    menu_settings,
    menu_credits,
    menu_quit,
    menu_unknown
};

enum menu_position{
    menu_p_settings,
    menu_p_load,
    menu_p_continue,
    menu_p_save,
    menu_p_quit
};

enum box_content_type{
    box_mp,
    box_coins,
    box_scroll,
    box_key,
    box_map,
    box_clear
};

enum scroll_type{
    scroll_0,
    scroll_1,
    scroll_2,
    scroll_3,
    scroll_slow,
    scroll_fire,
    scroll_thunderbolt,
    scroll_7,
    scroll_empty// = 34
};

enum being_type{
    being_weak,
    being_ordinary,
    being_ranger,
    being_elite,
    being_elite_ranger,
    being_commander,
    being_warlock,
    ally_ordinary
};

enum being_effect{
    being_effect_slow,
    being_effect_commander,
    being_effect_bonus,
    being_effect_open_portal,
    being_effect_burn,
    being_effect_thunderbolt,
    being_no_effect
};

enum pc_effect{
    pc_effect_slow,
    pc_effect_hpregen,
    pc_effect_weak,
    pc_effect_dodge,
    pc_no_effect
};

enum game_effect{
    game_effect_horde_attack,
    game_effect_1,
    game_no_effect
};

enum visual_effect_type{
    visual_effect_t0,
    visual_effect_t1,
    visual_effect_t2,
    visual_effect_t2_timer,
    visual_effect_bolt
};

enum projectile_type{
    projectile_penetrat,
    projectile_hostile,
    projectile_special
};

enum special_projectile_effect{
    projectile_warlock,
    projectile_fire
};

enum segment_flags{
    segment_in_sight = 1 << 0,
    segment_known = 1 << 1
};

enum game_flags{
    gamef_horde_attack = 1 << 0,
    gamef_morale_break = 1 << 1
};

enum key_info{
    key_location_unknown,
    key_location_known,
    key_owned
};

enum event{
    event_quit_game = 1,
    event_menu,
    event_manage_scrolls
};

enum game_update{
    update_ok,
    update_defeated,
    update_victory,
    update_shop
};

enum shop_item{
    shop_item_invalid = scroll_empty,
    shop_item_mp10,
    shop_item_mp100,
    shop_item_key,
    shop_item_key_location,
    shop_item_armour,
    shop_item_dodge_speed
};

enum character{
    Ż,Ź,Ś,Ć,s0,Ń,Ó,J,Q,s1,Z,s2,Ą,L,
    Ę,Ł,S,C,D,E,s3,s4,j,T,s5,s6,s7,s8,f,B,s9,
    V,W,s10,G,H,N,U,exclam_m,s12,n3,n8,b,dot,
    h,l,I,M,O,P,R,Y,s14,ł,n1,n2,n6,n7,a,
    k,A,F,K,X,ź,ż,n0,n4,n5,n9,d,ń,ć,ó,i,
    ś,q,g,p,y,ą,ę,s15,t,z,m,n,o,r,s,v,w,
    c,e,u,x,s16,s17,s_plus,s19,s20,s21,s22,s23,s24,s25,s_minus,s27,s28,s29,sp,
    char_end
};

enum texture_num{
    tx_icon,
    tx_viewfinder,
    tx_pc_blade,
    tx_icons,
    tx_arrow,
    tx_being_blade,
    tx_terrain,
    tx_gunsightpart,
    tx_damage_test,
    tx_portal,
    tx_door,
    tx_box,
    tx_barrier,
    tx_stun,
    tx_scroll,
    tx_weapon,
    tx_creation_point,
    tx_bar,
    tx_chars,
    tx_menu_ptr,
    tx_weapon1,
    tx_bonus_effect,
    tx_curse_effect,
    tx_owned,
    tx_void,
    tx_pc,
    tx_being_weak,
    tx_being_ordinary,
    tx_being_ranger,
    tx_being_elite,
    tx_being_elite_ranger,
    tx_being_commander,
    tx_being_warlock,
    tx_being_ally_ordinary,
    tx_projectile,
    tx_h_projectile,
    tx_pixel,
    tx_map,
    tx_background,
    txs_num
};

#endif