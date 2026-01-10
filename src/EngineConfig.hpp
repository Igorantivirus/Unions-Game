#pragma once

namespace engine::cfg
{
// Physics constants
constexpr float gravityY = 980.0f;
constexpr float linearDamping = 0.96f;
constexpr float angularDamping = 0.96f;

constexpr float restitution = 0.1f;
constexpr float staticFriction = 0.6f;
constexpr float dynamicFriction = 0.45f;
constexpr float bounceStopSpeed = 40.0f; // below this normal speed we treat impact as inelastic

// Sleeping / stabilization
constexpr float sleepLinearThreshold = 10.0f;
constexpr float sleepAngularThreshold = 0.8f;
constexpr float sleepTimeToSleep = 0.6f;

// Positional correction
constexpr float correctionSlop = 0.2f;
constexpr float correctionPercent = 0.2f;
constexpr float correctionMax = 4.0f;

// Iterations / tolerances
constexpr int gjkMaxIterations = 32;
constexpr int epaMaxIterations = 64;
constexpr float epaTolerance = 0.001f;
constexpr float epsilon = 1e-8f;

// Level geometry helpers
constexpr float wallThickness = 40.0f;
} // namespace engine::cfg
