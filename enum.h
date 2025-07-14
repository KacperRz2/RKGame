#ifndef ENUM_H_
#define ENUM_H_

enum texture_num {
    tx_pc = 1,
    tx_compass,
    tx_projectiole,
    tx_being,
    tx_viewfinder,
    tx_pc_blade,
    tx_nesw,
    tx_arrow,
    tx_being_blade,
    tx_h_projectile
};

enum control {
    forward = 1 << 0,
    back = 1 << 1,
    right = 1 << 2,
    left = 1 << 3,
    dodge = 1 << 4,
    run = 1 << 5,
    tmp0 = 1 << 6,
    attack = 1 << 7,
    fire = 1 << 8
};

#endif