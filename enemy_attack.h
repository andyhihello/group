/**
 * enemy_attack.h - Header file for enemy attack system
 * Theme: "Nothing can go wrong..."
 */

#ifndef ENEMY_ATTACK_H
#define ENEMY_ATTACK_H

#include <stdbool.h>
#include <time.h>

/**
 * Enemy type enumeration
 */
typedef enum {
    SECURITY_DRONE,
    ROBOT_SOLDIER
} EnemyType;

/**
 * Attack mode enumeration
 */
typedef enum {
    LASER_SHOT,
    SCATTER_SHOT
} AttackMode;

/**
 * Coordinate structure
 */
typedef struct {
    float x;
    float y;
} Vector2;

/**
 * Laser attack structure
 */
typedef struct {
    Vector2 start;          // Laser start point
    Vector2 end;            // Laser end point
    float damage;           // Damage value
    float width;            // Laser width
    float duration;         // Laser duration time
    bool active;            // Whether laser is active
    float timeActive;       // Time laser has been active
} LaserShot;

/**
 * Scatter bullet structure
 */
typedef struct {
    Vector2 position;       // Bullet position
    Vector2 direction;      // Bullet direction
    float speed;            // Bullet speed
    float damage;           // Base damage value
    bool active;            // Whether bullet is active
} ScatterBullet;

/**
 * Security drone structure
 */
typedef struct {
    Vector2 position;       // Drone position
    float health;           // Drone health
    float attackCooldown;   // Attack cooldown time (seconds)
    float timeSinceLastAttack; // Time since last attack
    LaserShot laser;        // Laser attack
    EnemyType type;         // Enemy type
} SecurityDrone;

/**
 * Robot soldier structure
 */
typedef struct {
    Vector2 position;       // Robot soldier position
    float health;           // Robot soldier health
    float attackCooldown;   // Attack cooldown time (seconds)
    float timeSinceLastAttack; // Time since last attack
    ScatterBullet bullets[3]; // Scatter bullets (fires 3 at once)
    float maxDamageDistance; // Maximum damage distance
    float minDamageDistance; // Minimum damage distance
    float maxDamage;        // Maximum damage value
    float minDamage;        // Minimum damage value
    EnemyType type;         // Enemy type
} RobotSoldier;

/**
 * Initialize a security drone
 * @param drone Pointer to the drone to initialize
 * @param x Initial X coordinate
 * @param y Initial Y coordinate
 */
void initSecurityDrone(SecurityDrone* drone, float x, float y);

/**
 * Initialize a robot soldier
 * @param soldier Pointer to the soldier to initialize
 * @param x Initial X coordinate
 * @param y Initial Y coordinate
 */
void initRobotSoldier(RobotSoldier* soldier, float x, float y);

/**
 * Update the security drone's state
 * @param drone Pointer to the drone to update
 * @param playerPosition Player position
 * @param deltaTime Time delta (seconds)
 */
void updateSecurityDrone(SecurityDrone* drone, Vector2 playerPosition, float deltaTime);

/**
 * Update the robot soldier's state
 * @param soldier Pointer to the soldier to update
 * @param playerPosition Player position
 * @param deltaTime Time delta (seconds)
 */
void updateRobotSoldier(RobotSoldier* soldier, Vector2 playerPosition, float deltaTime);

/**
 * Fire a laser from a security drone
 * @param drone Pointer to the drone firing the laser
 * @param playerPosition Player position
 */
void securityDroneFireLaser(SecurityDrone* drone, Vector2 playerPosition);

/**
 * Fire scatter bullets from a robot soldier
 * @param soldier Pointer to the soldier firing bullets
 * @param playerPosition Player position
 */
void robotSoldierFireScatterShot(RobotSoldier* soldier, Vector2 playerPosition);

/**
 * Check if a laser hits the player
 * @param laser Pointer to the laser
 * @param playerPosition Player position
 * @param playerRadius Player collision radius
 * @return True if the laser hits the player, otherwise false
 */
bool checkLaserHit(LaserShot* laser, Vector2 playerPosition, float playerRadius);

/**
 * Check if a scatter bullet hits the player
 * @param bullet Pointer to the bullet
 * @param playerPosition Player position
 * @param playerRadius Player collision radius
 * @return True if the bullet hits the player, otherwise false
 */
bool checkBulletHit(ScatterBullet* bullet, Vector2 playerPosition, float playerRadius);

/**
 * Calculate distance-based damage
 * @param bullet Pointer to the bullet
 * @param playerPosition Player position
 * @param soldier Pointer to the robot soldier
 * @return Damage value based on distance
 */
float calculateDistanceBasedDamage(ScatterBullet* bullet, Vector2 playerPosition, RobotSoldier* soldier);

/**
 * Calculate distance between two points
 * @param p1 First point
 * @param p2 Second point
 * @return Distance between two points
 */
float distance(Vector2 p1, Vector2 p2);

/**
 * Normalize a vector
 * @param v Vector to normalize
 * @return Normalized vector
 */
Vector2 normalizeVector(Vector2 v);

#endif // ENEMY_ATTACK_H
