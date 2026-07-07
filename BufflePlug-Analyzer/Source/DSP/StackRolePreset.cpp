#include "StackRolePreset.h"

namespace buffle::align
{
StackRoleSettings getStackRoleSettings (StackRole role) noexcept
{
    switch (role)
    {
        case StackRole::doubleTight:
            return { 0.88f, 0.28f, 0.72f, 0.0f, 0.72f };

        case StackRole::choirNatural:
            return { 0.48f, 0.78f, 0.38f, 0.08f, 0.24f };

        case StackRole::rapStack:
            return { 0.76f, 0.46f, 0.86f, 0.0f, 0.58f };

        case StackRole::adrLoose:
            return { 0.36f, 0.84f, 0.24f, 0.12f, 0.18f };

        case StackRole::manual:
        default:
            return {};
    }
}

const char* getStackRoleName (StackRole role) noexcept
{
    switch (role)
    {
        case StackRole::doubleTight: return "Double Tight";
        case StackRole::choirNatural: return "Choir Natural";
        case StackRole::rapStack: return "Rap Stack";
        case StackRole::adrLoose: return "ADR Loose";
        case StackRole::manual:
        default: return "Manual";
    }
}
}
