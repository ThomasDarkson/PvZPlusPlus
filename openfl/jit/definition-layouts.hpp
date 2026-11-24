#pragma once
                                                                 
namespace OpenPVZDefinitions {
struct LegacyLayout { const int* data; size_t fields; };
inline LegacyLayout legacyLayout(DefMap* map) {
    static constexpr int layout0[] = {56, 0, 4, 8, 12, 24, 32, 40, 48, 16};
    if (map == &gTrailDefMap) return {layout0, 9};
    static constexpr int layout1[] = {20, 0, 4, 12};
    if (map == &gParticleFieldDefMap) return {layout1, 3};
    static constexpr int layout2[] = {356, 0, 8, 4, 12, 16, 20, 24, 28, 36, 32, 44, 52, 60, 68, 76, 84, 92, 100, 108, 116, 140, 124, 132, 148, 172, 180, 188, 196, 204, 156, 164, 212, 220, 228, 236, 244, 252, 260, 268, 276, 284, 292, 300, 308, 316, 324, 332, 340, 348};
    if (map == &gEmitterDefMap) return {layout2, 49};
    static constexpr int layout3[] = {8, 0};
    if (map == &gParticleDefMap) return {layout3, 1};
    static constexpr int layout4[] = {44, 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40};
    if (map == &gReanimatorTransformDefMap) return {layout4, 11};
    static constexpr int layout5[] = {12, 0, 4};
    if (map == &gReanimatorTrackDefMap) return {layout5, 2};
    static constexpr int layout6[] = {16, 0, 8};
    if (map == &gReanimatorDefMap) return {layout6, 2};
    throw std::runtime_error("Unknown compiled definition schema");
}
}
