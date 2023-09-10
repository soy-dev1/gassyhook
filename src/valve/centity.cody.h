```cpp
#include "cvector.h"
#include "cmath"

__forceinline CVector CalculateSpread(int seed, float inaccuracy, float spread, bool revolver2 = false) {
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
```
