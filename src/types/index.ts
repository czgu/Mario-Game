/** Movement struct equivalent from original C++ code */
export interface Movement {
  xPos: number;
  yPos: number;
  life: number;
  move: number;   // x velocity
  height: number;
  width: number;
  down: number;   // y velocity (positive = upward)
  death: boolean;
  deathCounter: number;
}

/** Floating score display */
export interface ScoreDisplay {
  xPos: number;
  yPos: number;
  timeCounter: number;
  score: number;
  life: number;
}

/** Per-level design metadata */
export interface LevelDesign {
  flagPosition: number;
  castlePosition: number;
  ladderPosition: number;
  backgroundType: number;   // 0 = normal bricks, 1 = blue/night bricks
  heightSpawn: number;
  numberSpawn: number;
  numberGoldSpawn: number;
}

/** Object type indices matching original code */
export enum ObjectType {
  BOWSER = 0,
  GOOMBA = 1,
  TURTLE = 2,
  METEOR = 3,
  FLOWER = 4,
  COIN = 5,
  CANNON = 6,
  BUTTON = 7,
  MUSHROOM = 8,
  FLAG_POLE = 9,
}

/** Animation type indices matching original code */
export enum AnimationType {
  COIN_ANIM = 0,
  BRICK_ANIM = 1,
  GOOMBA_SQUASH = 2,
  GOOMBA_DEAD = 3,
  TURTLE_DEAD = 4,
  EXPLOSION = 5,
  ONE_UP = 6,
  PRINCESS = 7,
  END_MESSAGE = 8,
  FLAG = 9,
  BIG_EXPLOSION = 10,
  DEAD_CANNON = 11,
}

/** Tile types in the map */
export enum TileType {
  SKY = 0,
  GROUND_SOIL = 1,
  GROUND_GRASS = 2,
  BRICK = 3,
  QUESTION_BOX = 4,
  TUBE_EXT_LEFT = 5,
  TUBE_EXT_RIGHT = 6,
  TUBE_TOP_LEFT = 7,
  TUBE_TOP_RIGHT = 8,
  BLOCK = 9,
  EMPTY_BOX = 10,
  POLE = 11,
  POLE_HEAD = 12,
  SHROOM_BOX = 13,
  UP_BRICK = 14,
}

/** Mario life states */
export enum MarioState {
  DEAD = 0,
  SMALL = 1,
  BIG = 2,
  FIRE = 3,
}

/** Sound effect types */
export enum SoundEffect {
  CANT_BRICK = 0,
  COIN = 1,
  SHELL_KICK = 2,
  ENEMY_STOMP = 3,
  SMALL_JUMP = 4,
  BIG_JUMP = 5,
  POWER_UP = 6,
  MARIO_HURT = 7,
  MUSHROOM_SPAWN = 8,
  BRICK_BREAK = 9,
  FIREBALL_THROW = 10,
  ONE_UP_SOUND = 11,
  PAUSE = 12,
}

/** Game state passed between scenes */
export interface GameState {
  score: number;
  goldCoin: number;
  lives: number;
  gameLevel: number;
  marioLife: MarioState;
}

export function createDefaultMovement(): Movement {
  return {
    xPos: 0, yPos: 0, life: 0, move: 0,
    height: 0, width: 0, down: 0,
    death: false, deathCounter: 0,
  };
}

export function createDefaultScoreDisplay(): ScoreDisplay {
  return { xPos: 0, yPos: 0, timeCounter: 0, score: 0, life: 0 };
}
