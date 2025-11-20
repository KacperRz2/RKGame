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
    tmp = 1 << 12
};

enum being_status{
    being_follow,
    being_shoot,
    being_strike,
    being_follow_being,
    being_shoot_being,
    being_strike_being,
    being_idle,
    being_walk,
    being_dead,
    being_stunned,
    being_fly,
    being_search,
    being_attack_being,
    being_void
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
    box_map,
    box_clear
};

enum scroll_type{
    scroll_0,
    scroll_1,
    scroll_2,
    scroll_3,
    scroll_slow,
    scroll_5,
    scroll_6,
    scroll_empty = 34
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
    being_no_effect
};

enum pc_effect{
    pc_effect_slow,
    pc_effect_hpregen,
    pc_no_effect
};

enum game_effect{
    game_effect_horde_attack,
    game_effect_1,
    game_no_effect
};

enum visual_effect_type{
    visual_effect_static,
    visual_effect_moving,
    visual_effect_2
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

enum game_flags{
    gamef_horde_attack = 1 << 0
};

enum event{
    event_manage_scrolls = 2
};

enum game_update{
    update_ok,
    update_defeated
};

enum character{
    Ż,Ź,Ś,Ć,s0,Ń,Ó,J,Q,s1,Z,s2,Ą,L,
    Ę,Ł,S,C,D,E,s3,s4,j,T,s5,s6,s7,s8,f,B,s9,
    V,W,s10,G,H,N,U,exclam_m,s12,n3,n8,b,dot,
    h,l,I,M,O,P,R,Y,s14,ł,n1,n2,n6,n7,a,
    k,A,F,K,X,ź,ż,n0,n4,n5,n9,d,ń,ć,ó,i,
    ś,q,g,p,y,ą,ę,s15,t,z,m,n,o,r,s,v,w,
    c,e,u,x,s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,sp,
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
    tx_h_projectile1,
    tx_map,
    tx_background,
    txs_num
};

#endif