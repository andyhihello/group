/**
 * enemy_attack.c - Implementation of enemy attack system
 * Theme: "Nothing can go wrong..."
 */

#include "enemy_attack.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Definition of mathematical constant PI (if not defined in standard libraries)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Calculate the distance between two points
 */
float distance(Vector2 p1, Vector2 p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

/**
 * Normalize a vector
 */
Vector2 normalizeVector(Vector2 v) {
    Vector2 result = {0};
    float length = sqrt(v.x * v.x + v.y * v.y);
    
    // Prevent division by zero
    if (length > 0) {
        result.x = v.x / length;
        result.y = v.y / length;
    }
    
    return result;
}

/**
 * Initialize a security drone
 */
void initSecurityDrone(SecurityDrone* drone, float x, float y) {
    if (drone == NULL) return;
    
    drone->position.x = x;
    drone->position.y = y;
    drone->health = 100.0f;
    drone->attackCooldown = 2.0f;  // Fire laser every 2 seconds
    drone->timeSinceLastAttack = 0.0f;
    drone->type = SECURITY_DRONE;
    
    // Initialize laser
    drone->laser.damage = 25.0f;   // Laser damage
    drone->laser.width = 5.0f;     // Laser width
    drone->laser.duration = 0.5f;  // Laser duration 0.5 seconds
    drone->laser.active = false;
    drone->laser.timeActive = 0.0f;
}

/**
 * Initialize a robot soldier
 */
void initRobotSoldier(RobotSoldier* soldier, float x, float y) {
    if (soldier == NULL) return;
    
    soldier->position.x = x;
    soldier->position.y = y;
    soldier->health = 120.0f;
    soldier->attackCooldown = 3.0f;  // Fire scatter shot every 3 seconds
    soldier->timeSinceLastAttack = 0.0f;
    soldier->type = ROBOT_SOLDIER;
    
    // Set damage distances
    soldier->maxDamageDistance = 100.0f;  // Maximum damage distance
    soldier->minDamageDistance = 500.0f;  // Minimum damage distance
    soldier->maxDamage = 20.0f;           // Maximum damage
    soldier->minDamage = 5.0f;            // Minimum damage
    
    // Initialize bullets (initially inactive)
    for (int i = 0; i < 3; i++) {
        soldier->bullets[i].active = false;
        soldier->bullets[i].speed = 300.0f;  // Bullet speed
        soldier->bullets[i].damage = soldier->maxDamage;  // Base damage, adjusted later by distance
    }
}

/**
 * Fire laser from a security drone
 */
void securityDroneFireLaser(SecurityDrone* drone, Vector2 playerPosition) {
    if (drone == NULL) return;
    
    // Set laser start point (from drone position)
    drone->laser.start = drone->position;
    
    // Calculate direction vector (from drone to player)
    Vector2 direction = {
        playerPosition.x - drone->position.x,
        playerPosition.y - drone->position.y
    };
    
    // Normalize the direction vector
    direction = normalizeVector(direction);
    
    // Set laser end point (long enough distance)
    float laserLength = 1000.0f;  // Laser length
    drone->laser.end.x = drone->position.x + direction.x * laserLength;
    drone->laser.end.y = drone->position.y + direction.y * laserLength;
    
    // Activate the laser
    drone->laser.active = true;
    drone->laser.timeActive = 0.0f;
}

/**
 * Fire scatter bullets from a robot soldier
 */
void robotSoldierFireScatterShot(RobotSoldier* soldier, Vector2 playerPosition) {
    if (soldier == NULL) return;
    
    // Calculate base direction (from soldier to player)
    Vector2 baseDirection = {
        playerPosition.x - soldier->position.x,
        playerPosition.y - soldier->position.y
    };
    
    // Normalize base direction
    baseDirection = normalizeVector(baseDirection);
    
    // Calculate base angle
    float baseAngle = atan2(baseDirection.y, baseDirection.x);
    
    // Fire three bullets, slightly spread
    for (int i = 0; i < 3; i++) {
        // Angle offset (left, center, right)
        float angleOffset = (i - 1) * 0.1f;  // -0.1, 0, 0.1 radians
        float finalAngle = baseAngle + angleOffset;
        
        // Set bullet position and direction
        soldier->bullets[i].position = soldier->position;
        soldier->bullets[i].direction.x = cos(finalAngle);
        soldier->bullets[i].direction.y = sin(finalAngle);
        soldier->bullets[i].active = true;
    }
}

/**
 * Check if the laser hits the player
 */
bool checkLaserHit(LaserShot* laser, Vector2 playerPosition, float playerRadius) {
    if (laser == NULL || !laser->active) return false;
    
    // Calculate distance from point to line segment
    Vector2 lineVector = {
        laser->end.x - laser->start.x,
        laser->end.y - laser->start.y
    };
    
    Vector2 pointVector = {
        playerPosition.x - laser->start.x,
        playerPosition.y - laser->start.y
    };
    
    float lineLength = sqrt(lineVector.x * lineVector.x + lineVector.y * lineVector.y);
    
    // Avoid division by zero
    if (lineLength < 0.001f) {
        return distance(laser->start, playerPosition) <= playerRadius + laser->width / 2;
    }
    
    // Normalize line vector
    lineVector.x /= lineLength;
    lineVector.y /= lineLength;
    
    // Projection length of point vector onto line vector
    float projection = pointVector.x * lineVector.x + pointVector.y * lineVector.y;
    
    // If projection is outside the segment, check distance to endpoints
    if (projection < 0) {
        return distance(laser->start, playerPosition) <= playerRadius + laser->width / 2;
    } else if (projection > lineLength) {
        return distance(laser->end, playerPosition) <= playerRadius + laser->width / 2;
    }
    
    // Calculate closest point on the line
    Vector2 closestPoint = {
        laser->start.x + lineVector.x * projection,
        laser->start.y + lineVector.y * projection
    };
    
    // Check if distance from closest point to player is within radius
    return distance(closestPoint, playerPosition) <= playerRadius + laser->width / 2;
}

/**
 * Check if the bullet hits the player
 */
bool checkBulletHit(ScatterBullet* bullet, Vector2 playerPosition, float playerRadius) {
    if (bullet == NULL || !bullet->active) return false;
    
    // Simple circle collision detection
    return distance(bullet->position, playerPosition) <= playerRadius;
}

/**
 * Calculate distance-based damage
 */
float calculateDistanceBasedDamage(ScatterBullet* bullet, Vector2 playerPosition, RobotSoldier* soldier) {
    if (bullet == NULL || soldier == NULL) return 0.0f;
    
    float dist = distance(bullet->position, playerPosition);
    
    // Linearly interpolate damage based on distance
    if (dist <= soldier->maxDamageDistance) {
        return soldier->maxDamage;
    } else if (dist >= soldier->minDamageDistance) {
        return soldier->minDamage;
    } else {
        float t = (dist - soldier->maxDamageDistance) / (soldier->minDamageDistance - soldier->maxDamageDistance);
        return soldier->maxDamage + t * (soldier->minDamage - soldier->maxDamage);
    }
}

/**
 * Update the status of a security drone
 */
void updateSecurityDrone(SecurityDrone* drone, Vector2 playerPosition, float deltaTime) {
    if (drone == NULL) return;
    
    // Update attack cooldown timer
    drone->timeSinceLastAttack += deltaTime;
    
    // Fire laser if cooldown is finished
    if (drone->timeSinceLastAttack >= drone->attackCooldown) {
        securityDroneFireLaser(drone, playerPosition);
        drone->timeSinceLastAttack = 0.0f;
    }
    
    // Update laser status
    if (drone->laser.active) {
        drone->laser.timeActive += deltaTime;
        
        // Deactivate laser if duration expires
        if (drone->laser.timeActive >= drone->laser.duration) {
            drone->laser.active = false;
        }
    }
}

/**
 * Update the status of a robot soldier
 */
void updateRobotSoldier(RobotSoldier* soldier, Vector2 playerPosition, float deltaTime) {
    if (soldier == NULL) return;
    
    // Update attack cooldown timer
    soldier->timeSinceLastAttack += deltaTime;
    
    // Fire scatter shot if cooldown is finished
    if (soldier->timeSinceLastAttack >= soldier->attackCooldown) {
        robotSoldierFireScatterShot(soldier, playerPosition);
        soldier->timeSinceLastAttack = 0.0f;
    }
    
    // Update bullet positions
    for (int i = 0; i < 3; i++) {
        if (soldier->bullets[i].active) {
            // Update bullet position
            soldier->bullets[i].position.x += soldier->bullets[i].direction.x * soldier->bullets[i].speed * deltaTime;
            soldier->bullets[i].position.y += soldier->bullets[i].direction.y * soldier->bullets[i].speed * deltaTime;
            
            // Check if bullet goes out of screen (assuming screen size is 1000x1000)
            // Adjust this value according to your game world
            float screenBoundary = 1000.0f;
            if (soldier->bullets[i].position.x < -screenBoundary || 
                soldier->bullets[i].position.x > screenBoundary ||
                soldier->bullets[i].position.y < -screenBoundary || 
                soldier->bullets[i].position.y > screenBoundary) {
                soldier->bullets[i].active = false;
            }
        }
    }
}
