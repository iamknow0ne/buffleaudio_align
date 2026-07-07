#pragma once

namespace buffle::align
{
enum class StackRole
{
    manual = 0,
    doubleTight = 1,
    choirNatural = 2,
    rapStack = 3,
    adrLoose = 4
};

struct StackRoleSettings
{
    float tightness = 0.72f;
    float naturalness = 0.42f;
    float consonantLevel = 0.68f;
    float guideBlend = 0.0f;
    float stereoFocus = 0.35f;
};

StackRoleSettings getStackRoleSettings (StackRole role) noexcept;
const char* getStackRoleName (StackRole role) noexcept;
}
