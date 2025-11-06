#ifndef ENUM_H_
#define ENUM_H_

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
    cast = 1 << 11,
    tmp1 = 1 << 13
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
    box_map,
    box_clear
};

enum scroll_type{
    scroll_0,
    scroll_1,
    scroll_2,
    scroll_3,
    scroll_4,
    scroll_5,
    scroll_6,
    scroll_empty = 34
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
    V,W,s10,G,H,N,U,s11,s12,n3,n8,b,dot,
    h,l,I,M,O,P,R,Y,s14,ł,n1,n2,n6,n7,a,
    k,A,F,K,X,ź,ż,n0,n4,n5,n9,d,ń,ć,ó,i,
    ś,q,g,p,y,ą,ę,s15,t,z,m,n,o,r,s,v,w,
    c,e,u,x,s16,s17,s18,s19,s20,s21,s22,s23,s24,s25,s26,s27,s28,s29,sp,
    char_end
};

#endif