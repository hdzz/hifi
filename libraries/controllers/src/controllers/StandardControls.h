//
//  Created by Bradley Austin Davis 2015/10/09
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once

namespace controller {

    // Needs to match order and values of SDL_GameControllerButton
    enum StandardButtonChannel {
        // Button quad
        A = 0,
        B,
        X,
        Y,

        // Center buttons
        BACK,
        GUIDE,
        START,

        // Stick press
        LS,
        RS,

        // Bumper press
        LB,
        RB,

        // DPad
        DU,
        DD,
        DL,
        DR,

        // These don't map to SDL types
        LEFT_PRIMARY_THUMB,
        LEFT_SECONDARY_THUMB,
        LEFT_PRIMARY_THUMB_TOUCH,
        LEFT_SECONDARY_THUMB_TOUCH,
        LS_TOUCH,
        LEFT_THUMB_UP,

        RIGHT_PRIMARY_THUMB,
        RIGHT_SECONDARY_THUMB,
        RIGHT_PRIMARY_THUMB_TOUCH,
        RIGHT_SECONDARY_THUMB_TOUCH,
        RS_TOUCH,
        RIGHT_THUMB_UP,

        LEFT_PRIMARY_INDEX,
        LEFT_SECONDARY_INDEX,
        LEFT_PRIMARY_INDEX_TOUCH,
        LEFT_SECONDARY_INDEX_TOUCH,
        LEFT_INDEX_POINT,
        RIGHT_PRIMARY_INDEX,
        RIGHT_SECONDARY_INDEX,
        RIGHT_PRIMARY_INDEX_TOUCH,
        RIGHT_SECONDARY_INDEX_TOUCH,
        RIGHT_INDEX_POINT,

        LEFT_GRIP,
        LEFT_GRIP_TOUCH,
        RIGHT_GRIP,
        RIGHT_GRIP_TOUCH,

        NUM_STANDARD_BUTTONS
    };

    // Needs to match order and values of SDL_GameControllerAxis
    enum StandardAxisChannel {
        // Left Analog stick
        LX = 0,
        LY,
        // Right Analog stick
        RX,
        RY,
        // Triggers
        LT,
        RT,
        NUM_STANDARD_AXES,
        LZ = LT,
        RZ = RT
    };

    // No correlation to SDL
    enum StandardPoseChannel {
        HIPS_ROOT = 0,
        HIPS,
        RIGHT_UP_LEG,
        RIGHT_LEG,
        RIGHT_FOOT,
        LEFT_UP_LEG,
        LEFT_LEG,
        LEFT_FOOT,
        SPINE,
        SPINE1,
        SPINE2,
        SPINE3,
        NECK,
        HEAD,
        RIGHT_SHOULDER,
        RIGHT_ARM,
        RIGHT_FORE_ARM,
        RIGHT_HAND,
        RIGHT_HAND_THUMB1,
        RIGHT_HAND_THUMB2,
        RIGHT_HAND_THUMB3,
        RIGHT_IN_HAND_INDEX,
        RIGHT_HAND_INDEX1,
        RIGHT_HAND_INDEX2,
        RIGHT_HAND_INDEX3,
        RIGHT_IN_HAND_MIDDLE,
        RIGHT_HAND_MIDDLE1,
        RIGHT_HAND_MIDDLE2,
        RIGHT_HAND_MIDDLE3,
        RIGHT_IN_HANDRING,
        RIGHT_HAND_RING1,
        RIGHT_HAND_RING2,
        RIGHT_HAND_RING3,
        RIGHT_IN_HAND_PINKY,
        RIGHT_HAND_PINKY1,
        RIGHT_HAND_PINKY2,
        RIGHT_HAND_PINKY3,
        LEFT_SHOULDER,
        LEFT_ARM,
        LEFT_FORE_ARM,
        LEFT_HAND,
        LEFT_HAND_THUMB1,
        LEFT_HAND_THUMB2,
        LEFT_HAND_THUMB3,
        LEFT_IN_HAND_INDEX,
        LEFT_HAND_INDEX1,
        LEFT_HAND_INDEX2,
        LEFT_HAND_INDEX3,
        LEFT_IN_HAND_MIDDLE,
        LEFT_HAND_MIDDLE1,
        LEFT_HAND_MIDDLE2,
        LEFT_HAND_MIDDLE3,
        LEFT_IN_HAND_RING,
        LEFT_HAND_RING1,
        LEFT_HAND_RING2,
        LEFT_HAND_RING3,
        LEFT_IN_HAND_PINKY,
        LEFT_HAND_PINKY1,
        LEFT_HAND_PINKY2,
        LEFT_HAND_PINKY3,
        NUM_STANDARD_POSES
    };

    enum StandardCounts {
        TRIGGERS = 2,
        ANALOG_STICKS = 2,
        POSES = 2, // FIXME 3?  if we want to expose the head?
    };
}
