#include "hitchance.h"
#include "../core/globals.h"
#include "../core/interfaces.h"
#include "../valve/ienginetrace.h"

float degreesToRadians(float degrees) {
    const float PI = 3.14159265359f;

    return degrees * (PI / 180.f);
}


void AngleVectors(const CVector& angles, CVector* forward, CVector* right, CVector* up) {

    float cp = cos(degreesToRadians(angles.x));
    float sp = sin(degreesToRadians(angles.x));
    float cy = cos(degreesToRadians(angles.y));
    float sy = sin(degreesToRadians(angles.y));
    float cr = cos(degreesToRadians(angles.z));
    float sr = sin(degreesToRadians(angles.z));

    if (forward) {
        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    if (right) {
        right->x = -1 * sr * sp * cy + -1 * cr * -sy;
        right->y = -1 * sr * sp * sy + -1 * cr * cy;
        right->z = -1 * sr * cp;
    }

    if (up) {
        up->x = cr * sp * cy + -sr * -sy;
        up->y = cr * sp * sy + -sr * cy;
        up->z = cr * cp;
    }
}


CVector CalculateSpread(int seed, float inaccuracy, float spread) {

    CEntity* activeWeapon = globals::localPlayer->GetActiveWeapon();

    float recoil_index, r1, r2, r3, r4, s1, c1, s2, c2;

    auto randomSeedFn = reinterpret_cast<void(__cdecl*)(int)>(interfaces::randomSeed);
    auto randomFloatFn = reinterpret_cast<float(__cdecl*)(float, float)>(interfaces::randomFloat);

    if (!activeWeapon) {
        return {};
    }

    int weaponType = activeWeapon->GetWeaponType();

    // Seed random
    randomSeedFn(seed & 0xff);

    float inaccuracy = activeWeapon->GetInaccuracy();
    float spread = activeWeapon->GetSpread();

    float r1 = randomFloatFn(0.0f, 1.0f);
    float r2 = randomFloatFn(0.0f, 3.1415926535897932384f * 2);

    // get needed sine / cosine values.
    c1 = std::cos(r2);
    c2 = std::cos(r4);
    s1 = std::sin(r2);
    s2 = std::sin(r4);

    // calculate spread vector.
    return {
        (c1 * (r1 * inaccuracy)) + (c2 * (r3 * spread)),
        (s1 * (r1 * inaccuracy)) + (s2 * (r3 * spread)),
        0.f
    };
}

bool hacks::CheckHitchance(CEntity* player, const CVector& angle) {

    // Define direction vectors
    CVector fwd, right, up;

    CTrace tr;

    // Get eye position
    CVector start;
    globals::localPlayer->GetEyePosition(start);

    // Define max hitchance value
    constexpr float HITCHANCE_MAX = 100.f;

    // Define max seed value 
    constexpr int SEED_MAX = 255;

    // Get needed hits
    int neededHits = (int)std::ceil(globals::hitchance * SEED_MAX / HITCHANCE_MAX);

    // Get direction vectors
    AngleVectors(angle, &fwd, &right, &up);


    // Get inaccuracy and spread
    float inaccuracy = globals::localPlayer->GetActiveWeapon()->GetInaccuracy();
    float spread = globals::localPlayer->GetActiveWeapon()->GetSpread();

    // Trace each possible bullet path
    for (int i = 0; i <= SEED_MAX; i++) {

        // Calculate spread
        wep_spread = globals::localPlayer->GetActiveWeapon()->CalculateSpread(i, inaccuracy, spread);

        // Calculate direction with spread
        dir = (fwd + right * wep_spread.x + up * wep_spread.y).Normalized();

        // Calculate end position 
        end = start + dir * globals::localPlayer->GetActiveWeapon()->GetRange();

        // Trace
        interfaces::engineTrace->ClipRayToEntity(Ray(start, end), MASK_SHOT, player, &tr);

        // Check if hit valid player
        if (tr.m_entity == player && game::IsValidHitgroup(tr.m_hitgroup)) {
            totalHits++;
        }

        // Check if we have enough hits
        if (totalHits >= neededHits) {
            return true;
        }

        // Can't possibly hit enough
        if (SEED_MAX - i + totalHits < neededHits) {
            return false;
        }
    }

    return false;
}

