import Phaser from 'phaser';
import {
  SCREEN_WIDTH, SCREEN_HEIGHT, TILE_SIZE, MAP_ROWS, MAP_COLS,
  JUMP_FORCE, WALK_SPEED, RUN_SPEED, INERTIA_NORMAL, INERTIA_RUN, INERTIA_ICE,
  FIREBALL_SPEED, FIREBALL_BOUNCE, FIREBALL_COOLDOWN,
  MARIO_SMALL_WIDTH, MARIO_SMALL_HEIGHT, MARIO_BIG_WIDTH, MARIO_BIG_HEIGHT, MARIO_DUCK_HEIGHT,
  GOOMBA_WIDTH, GOOMBA_HEIGHT, TURTLE_WIDTH, TURTLE_WALK_HEIGHT, TURTLE_SHELL_HEIGHT,
  BOWSER_WIDTH, BOWSER_HEIGHT, MUSHROOM_SIZE, FLOWER_WIDTH, FLOWER_HEIGHT,
  CANNON_WIDTH, CANNON_HEIGHT, BUTTON_SIZE, FLAG_POLE_WIDTH, FLAG_POLE_HEIGHT, BULLET_SIZE,
  SHELL_SPEED, TURTLE_REGEN_TIME, INVULN_FRAMES,
  BOWSER_JUMP_INTERVAL, BOWSER_TURTLE_INTERVAL, BOWSER_JUMP_FORCE,
  BOWSER_MIN_X, BOWSER_MAX_X, BOWSER_SPAWN_X, BOWSER_TRIGGER_X,
  BOSS_SCROLL_MIN, BOSS_SCROLL_MAX,
  LEVEL_TIME, MAX_OBJECTS, MAX_BULLETS, MAX_SCORE_DISPLAYS,
  SCORE_GOOMBA_STOMP, SCORE_GOOMBA_FIRE, SCORE_TURTLE_FIRE,
  SCORE_CANNON_STOMP, SCORE_SHELL_KICK, SCORE_SHELL_KILL,
  SCORE_MUSHROOM, SCORE_FLOWER, SCORE_COIN, SCORE_BRICK_COIN,
} from '../config/Constants.js';
import { LEVEL_DESIGNS } from '../config/LevelDesigns.js';
import {
  Movement, ScoreDisplay, GameState, MarioState,
  ObjectType, AnimationType, TileType,
  createDefaultMovement, createDefaultScoreDisplay,
} from '../types/index.js';

/** AABB collision check matching original collide() function */
function collide(
  x1: number, y1: number, w1: number, h1: number,
  x2: number, y2: number, w2: number, h2: number
): boolean {
  const r1 = x1 + w1;
  const b1 = y1 + h1;
  const r2 = x2 + w2;
  const b2 = y2 + h2;
  return !(b1 < y2 || y1 > b2 || r1 < x2 || x1 > r2);
}

/**
 * GameScene: The main gameplay scene containing ALL game logic
 * faithfully ported from the original 2956-line main.cpp.
 */
export class GameScene extends Phaser.Scene {
  // ── State ──────────────────────────────────────────────────
  private mario!: Movement;
  private objects!: Movement[][]; // [10][100]
  private animations!: Movement[][]; // [12][100]
  private bullets!: Movement[]; // [100]
  private gameScores!: ScoreDisplay[]; // [1000]
  private map!: number[][]; // [15][500]

  // Game state
  private score = 0;
  private gameTime = 0;
  private goldCoin = 0;
  private lives = 3;
  private gameLevel = 1;
  private speed = WALK_SPEED;
  private inertiaAmount = INERTIA_NORMAL;

  // Counters
  private inertiaCounter = 0;
  private goombaSpriteCounter = 0;
  private marioSpriteCounter = 0;
  private brickCounter = 0;
  private bulletCounter = 0;
  private bulletCdCounter = 0;
  private bulletCd = false;
  private mushroomCounter = 0;
  private flowerCounter = 0;
  private skyFireCounter = 0;
  private skyFireBallCounter = 0;
  private animBallCounter = 0;
  private animCoinCounter = 0;
  private animBrickCounter = 0;
  private cannonCounter = 0;
  private cannonSpawnCounter = 0;
  private globalScoreCounter = 0;

  // Mario state
  private marioFacing = 1; // 1=right, 0=left
  private keyNotJump = true;
  private keyNotShoot = true;
  private marioTransform = false;
  private marioTransformCounter = 0;
  private duckJump = false;
  private duckMode = false;
  private walkingCounter = 0;
  private onHit = true;

  // Scrolling
  private xScroll = 0;
  private marioScrollPosition = 0;
  private backgroundScroll: number[] = new Array(10).fill(0);

  // Level change
  private levelChange = true;
  private levelChangeAnimation = false;
  private completeSong = false;
  private flagTouch = false;
  private songCounter = 0;
  private flagDown = false;
  private deathSong = false;

  // Boss
  private spawnBrowser = false;
  private gameComplete = false;
  private browserJump = 0;
  private browserTurtle = 0;

  // Collision per-frame
  private upCollision = 0;
  private downCollision = 0;
  private rightCollision = 0;
  private leftCollision = 0;

  // Phaser display objects
  private displayObjects: Phaser.GameObjects.Image[] = [];
  private bgImage!: Phaser.GameObjects.TileSprite;
  private fgImages: Phaser.GameObjects.Image[] = [];
  private castleImage!: Phaser.GameObjects.Image;
  private hudTexts: Phaser.GameObjects.Text[] = [];

  // Keys
  private keys!: {
    left: Phaser.Input.Keyboard.Key;
    right: Phaser.Input.Keyboard.Key;
    up: Phaser.Input.Keyboard.Key;
    down: Phaser.Input.Keyboard.Key;
    x: Phaser.Input.Keyboard.Key;
    z: Phaser.Input.Keyboard.Key;
    p: Phaser.Input.Keyboard.Key;
    t: Phaser.Input.Keyboard.Key;
  };

  // Rendering pool
  private spritePool: Phaser.GameObjects.Image[] = [];
  private spritePoolIndex = 0;

  constructor() {
    super({ key: 'GameScene' });
  }

  init(data: GameState): void {
    this.score = data.score;
    this.goldCoin = data.goldCoin;
    this.lives = data.lives;
    this.gameLevel = data.gameLevel;
    this.mario = createDefaultMovement();
    this.mario.life = data.marioLife;
    this.levelChange = true;
  }

  create(): void {
    // Initialize arrays
    this.objects = [];
    for (let i = 0; i < 10; i++) {
      this.objects[i] = [];
      for (let j = 0; j < MAX_OBJECTS; j++) {
        this.objects[i][j] = createDefaultMovement();
      }
    }
    this.animations = [];
    for (let i = 0; i < 12; i++) {
      this.animations[i] = [];
      for (let j = 0; j < MAX_OBJECTS; j++) {
        this.animations[i][j] = createDefaultMovement();
      }
    }
    this.bullets = [];
    for (let j = 0; j < MAX_BULLETS; j++) {
      this.bullets[j] = createDefaultMovement();
      this.bullets[j].height = BULLET_SIZE;
      this.bullets[j].width = BULLET_SIZE;
    }
    this.gameScores = [];
    for (let i = 0; i < MAX_SCORE_DISPLAYS; i++) {
      this.gameScores[i] = createDefaultScoreDisplay();
    }
    this.map = [];
    for (let r = 0; r < MAP_ROWS; r++) {
      this.map[r] = new Array(MAP_COLS).fill(0);
    }

    // Set object dimensions (matching original initialization)
    for (let i = 0; i < 10; i++) {
      for (let j = 0; j < MAX_OBJECTS; j++) {
        const obj = this.objects[i][j];
        switch (i) {
          case ObjectType.BOWSER: obj.height = BOWSER_HEIGHT; obj.width = BOWSER_WIDTH; break;
          case ObjectType.GOOMBA: obj.height = GOOMBA_HEIGHT; obj.width = GOOMBA_WIDTH; break;
          case ObjectType.TURTLE: obj.height = TURTLE_WALK_HEIGHT; obj.width = TURTLE_WIDTH; obj.deathCounter = 0; break;
          case ObjectType.METEOR: obj.height = TILE_SIZE; obj.width = TILE_SIZE; break;
          case ObjectType.FLOWER: obj.height = FLOWER_HEIGHT; obj.width = FLOWER_WIDTH; break;
          case ObjectType.COIN: obj.height = 24; obj.width = 16; break; // approximate coin size
          case ObjectType.CANNON: obj.height = CANNON_HEIGHT; obj.width = CANNON_WIDTH; break;
          case ObjectType.BUTTON: obj.height = BUTTON_SIZE; obj.width = BUTTON_SIZE; break;
          case ObjectType.MUSHROOM: obj.height = MUSHROOM_SIZE; obj.width = MUSHROOM_SIZE; break;
          case ObjectType.FLAG_POLE: obj.height = FLAG_POLE_HEIGHT; obj.width = FLAG_POLE_WIDTH; break;
        }
      }
    }

    // Setup keyboard
    const kb = this.input.keyboard!;
    this.keys = {
      left: kb.addKey(Phaser.Input.Keyboard.KeyCodes.LEFT),
      right: kb.addKey(Phaser.Input.Keyboard.KeyCodes.RIGHT),
      up: kb.addKey(Phaser.Input.Keyboard.KeyCodes.UP),
      down: kb.addKey(Phaser.Input.Keyboard.KeyCodes.DOWN),
      x: kb.addKey(Phaser.Input.Keyboard.KeyCodes.X),
      z: kb.addKey(Phaser.Input.Keyboard.KeyCodes.Z),
      p: kb.addKey(Phaser.Input.Keyboard.KeyCodes.P),
      t: kb.addKey(Phaser.Input.Keyboard.KeyCodes.T),
    };

    // Create sprite pool for rendering
    for (let i = 0; i < 800; i++) {
      const sp = this.add.image(0, 0, 'small_stand_r').setVisible(false).setOrigin(0, 0);
      this.spritePool.push(sp);
    }

    // HUD texts
    const hudStyle = { fontSize: '24px', fontFamily: 'Arial', color: '#ffff00' };
    const valStyle = { fontSize: '24px', fontFamily: 'Arial', color: '#ffffff' };
    this.hudTexts.push(this.add.text(15, 10, 'Score', hudStyle).setScrollFactor(0).setDepth(100));
    this.hudTexts.push(this.add.text(400, 10, 'World', hudStyle).setScrollFactor(0).setDepth(100));
    this.hudTexts.push(this.add.text(565, 10, 'Time', hudStyle).setScrollFactor(0).setDepth(100));
    this.hudTexts.push(this.add.text(10, 35, '     0', valStyle).setScrollFactor(0).setDepth(100));
    this.hudTexts.push(this.add.text(255, 35, ' 0', valStyle).setScrollFactor(0).setDepth(100));
    this.hudTexts.push(this.add.text(420, 35, ' 1', valStyle).setScrollFactor(0).setDepth(100));
    this.hudTexts.push(this.add.text(575, 35, '300', valStyle).setScrollFactor(0).setDepth(100));

    // Reset counters
    this.bulletCounter = 0;
    this.bulletCdCounter = 0;
    this.bulletCd = false;
    this.spawnBrowser = false;
    this.gameComplete = false;
    this.browserJump = 0;
    this.browserTurtle = 0;
    this.goombaSpriteCounter = 0;
    this.marioSpriteCounter = 0;
    this.inertiaCounter = 0;
    this.globalScoreCounter = 0;
  }

  update(_time: number, _delta: number): void {
    // Pause check
    if (Phaser.Input.Keyboard.JustDown(this.keys.p)) {
      this.scene.pause();
      this.scene.launch('PauseScene');
      return;
    }

    // Cheat key (T)
    if (this.keys.t.isDown) {
      this.lives = 99;
      if (this.mario.life === MarioState.SMALL) this.mario.yPos -= 32;
      this.mario.life = MarioState.FIRE;
    }

    // ── Death/respawn check ───────────────────────────────────
    if (this.mario.life === MarioState.DEAD && this.lives > 1 && !this.mario.death && this.mario.yPos > 480 && !this.deathSong) {
      this.lives--;
      this.mario.life = MarioState.SMALL;
      this.levelChange = true;
    } else if (this.mario.life === MarioState.DEAD && this.lives <= 1 && !this.mario.death && this.mario.yPos > 480) {
      // Game over
      this.scene.start('GameOverScene', {
        score: this.score,
        goldCoin: this.goldCoin,
        lives: this.lives,
        gameLevel: this.gameLevel,
        marioLife: this.mario.life,
      } as GameState);
      return;
    }

    // ── Level initialization ──────────────────────────────────
    if (this.levelChange) {
      this.initLevel();
    }

    // ── Time out ──────────────────────────────────────────────
    if (this.gameTime === 0 && this.mario.life !== MarioState.DEAD && !this.levelChangeAnimation && !this.gameComplete) {
      this.mario.life = MarioState.DEAD;
      this.mario.down = 15;
    }

    // ── Turtle size update ────────────────────────────────────
    for (let j = 0; j < MAX_OBJECTS; j++) {
      if (this.objects[ObjectType.TURTLE][j].life === 1) {
        this.objects[ObjectType.TURTLE][j].height = TURTLE_SHELL_HEIGHT;
        this.objects[ObjectType.TURTLE][j].width = TURTLE_WIDTH;
      } else if (this.objects[ObjectType.TURTLE][j].life === 2) {
        this.objects[ObjectType.TURTLE][j].height = TURTLE_WALK_HEIGHT;
        this.objects[ObjectType.TURTLE][j].width = TURTLE_WIDTH;
      }
    }

    // ── Mario size ────────────────────────────────────────────
    if (this.mario.life === MarioState.SMALL) {
      this.mario.height = MARIO_SMALL_HEIGHT;
      this.mario.width = MARIO_SMALL_WIDTH;
    } else if (this.mario.life >= MarioState.BIG) {
      this.mario.height = MARIO_BIG_HEIGHT;
      this.mario.width = MARIO_BIG_WIDTH;
    }

    // Duck
    if (this.keys.down.isDown && this.mario.life > MarioState.SMALL && !this.levelChangeAnimation && !this.gameComplete && this.duckJump) {
      this.mario.height = MARIO_DUCK_HEIGHT;
      this.mario.width = MARIO_BIG_WIDTH;
      this.duckMode = true;
    } else {
      this.duckMode = false;
    }

    // ── Mario movement ────────────────────────────────────────
    if (this.keys.right.isDown && !this.keys.down.isDown && this.rightCollision === 0 && this.mario.life > 0 && !this.levelChangeAnimation && !this.gameComplete) {
      this.mario.move += 1;
      this.marioFacing = 1;
    }
    if (this.keys.left.isDown && !this.keys.down.isDown && this.leftCollision === 0 && this.mario.life > 0 && !this.levelChangeAnimation && !this.gameComplete) {
      this.mario.move -= 1;
      this.marioFacing = 0;
    }

    // Run / fireball
    if (this.keys.z.isDown) {
      this.speed = RUN_SPEED;
      this.inertiaAmount = INERTIA_RUN;

      if (this.mario.life === MarioState.FIRE && this.keyNotShoot && !this.levelChangeAnimation && !this.gameComplete && !this.bulletCd) {
        const b = this.bullets[this.bulletCounter];
        b.life = 1;
        b.yPos = this.mario.yPos + 30;
        if (this.marioFacing === 0) {
          b.xPos = this.mario.xPos - b.width;
          b.move = -FIREBALL_SPEED;
        } else {
          b.xPos = this.mario.xPos + this.mario.width;
          b.move = FIREBALL_SPEED;
        }
        this.bulletCounter++;
        this.playSfx('sfx_fireball');
        this.keyNotShoot = false;
        this.bulletCd = true;
      }
    }
    if (!this.keys.z.isDown) {
      this.speed = WALK_SPEED;
      this.inertiaAmount = INERTIA_NORMAL;
      this.keyNotShoot = true;
    }

    // ── Collision detection ───────────────────────────────────
    this.rightCollision = 0;
    this.upCollision = 0;
    this.leftCollision = 0;
    this.downCollision = 0;

    // TOP collision
    this.doTopCollision();
    // BOTTOM collision
    this.doBottomCollision();
    // Mario X movement
    this.mario.xPos += this.mario.move;
    // RIGHT collision
    this.doRightCollision();
    // LEFT collision
    this.doLeftCollision();

    // ── Jump ──────────────────────────────────────────────────
    if (this.keys.x.isDown && this.downCollision !== 0 && this.keyNotJump && this.mario.life > 0
        && (this.mario.height + this.mario.yPos < 480) && !this.levelChangeAnimation && !this.gameComplete) {
      this.mario.down += JUMP_FORCE;
      this.keyNotJump = false;
      this.playSfx(this.mario.life === MarioState.SMALL ? 'sfx_jump1' : 'sfx_jump2');
      this.duckJump = this.keys.down.isDown;
    }
    if (this.keys.down.isDown && this.downCollision !== 0) {
      this.duckJump = true;
    }
    if (!this.keys.x.isDown) this.keyNotJump = true;

    // Speed cap
    if (this.mario.move > this.speed) this.mario.move = this.speed;
    else if (this.mario.move < -this.speed) this.mario.move = -this.speed;

    // Falling (y position)
    this.mario.yPos -= this.mario.down;

    // ── Monster logic ─────────────────────────────────────────
    this.doMonsterLogic();
    // ── Unit collision (goomba/turtle inter-collision) ─────────
    this.doUnitCollision();
    // ── Bullet logic ──────────────────────────────────────────
    this.doBulletLogic();

    // ── 99 coins = 1 up ───────────────────────────────────────
    if (this.goldCoin > 99) {
      this.goldCoin -= 99;
      this.lives++;
      const anim = this.animations[AnimationType.ONE_UP][0];
      anim.life = 1;
      anim.move = 0;
      anim.xPos = this.mario.xPos + 60;
      anim.yPos = this.mario.yPos - 60;
      this.playSfx('sfx_1up');
    }

    // ── Animation updates ─────────────────────────────────────
    this.doAnimationUpdates();

    // ── Counters ──────────────────────────────────────────────
    this.inertiaCounter++;
    this.goombaSpriteCounter++;
    this.brickCounter++;
    this.marioSpriteCounter += Math.abs(this.mario.move);

    // Bullet cooldown
    if (this.bulletCd) this.bulletCdCounter++;
    if (this.bulletCdCounter > FIREBALL_COOLDOWN) {
      this.bulletCdCounter = 0;
      this.bulletCd = false;
    }

    // Animation counters
    for (let b = 0; b < MAX_OBJECTS; b++) {
      if (this.animations[AnimationType.GOOMBA_SQUASH][b].life === 1) this.animations[AnimationType.GOOMBA_SQUASH][b].deathCounter++;
      if (this.animations[AnimationType.COIN_ANIM][b].life === 1) this.animations[AnimationType.COIN_ANIM][b].deathCounter++;
      if (this.animations[AnimationType.EXPLOSION][b].life === 1) this.animations[AnimationType.EXPLOSION][b].deathCounter += 3;
      if (this.animations[AnimationType.ONE_UP][b].life === 1) this.animations[AnimationType.ONE_UP][b].deathCounter++;
      if (this.animations[AnimationType.BIG_EXPLOSION][b].life === 1) this.animations[AnimationType.BIG_EXPLOSION][b].deathCounter += 3;
    }

    // ── Falling rocks / cannons (level-specific) ──────────────
    this.doLevelSpecificSpawns();

    // ── Camera scrolling ──────────────────────────────────────
    if (this.mario.xPos < 320) {
      this.marioScrollPosition = this.mario.xPos;
      this.xScroll = 0;
    } else {
      this.marioScrollPosition = 320;
      this.xScroll = this.mario.xPos - 320;
    }

    // Mario position bounds
    if (this.mario.xPos <= 0) this.mario.xPos = 0;
    if (this.mario.yPos > 480) {
      this.mario.yPos = 481;
      this.mario.life = MarioState.DEAD;
    }

    // ── Boss fight ────────────────────────────────────────────
    this.doBossFight();

    // ── Background scroll positions ───────────────────────────
    for (let i = 0; i < 10; i++) {
      this.backgroundScroll[i] = -(this.xScroll / 5) + i * 1592;
    }

    // ── Inertia ───────────────────────────────────────────────
    if (this.inertiaCounter > this.inertiaAmount) {
      this.inertiaCounter = 0;
      if (this.mario.move > 0) this.mario.move--;
      if (this.mario.move < 0) this.mario.move++;
    }

    // ── Death timer ───────────────────────────────────────────
    if (this.mario.death) this.mario.deathCounter++;
    if (this.mario.deathCounter > 180) {
      this.mario.deathCounter = 0;
      this.mario.death = false;
    }

    // ── Score display ─────────────────────────────────────────
    for (let i = 0; i < MAX_SCORE_DISPLAYS; i++) {
      if (this.gameScores[i].life > 0) {
        this.gameScores[i].timeCounter--;
        this.gameScores[i].yPos--;
      }
      if (this.gameScores[i].timeCounter <= 0) {
        this.gameScores[i].life = 0;
      }
    }

    // ── Turtle regen ──────────────────────────────────────────
    for (let b = 0; b < MAX_OBJECTS; b++) {
      const t = this.objects[ObjectType.TURTLE][b];
      if (t.life === 1 && t.move === 0) t.deathCounter++;
      else if (t.life === 1 && t.move !== 0) t.deathCounter = 0;
      if (t.deathCounter > TURTLE_REGEN_TIME) {
        t.deathCounter = 0;
        t.life = 2;
        t.move = 1;
      }
    }

    // ── Brick timer (small mario hit brick -> up brick -> normal) ──
    if (this.brickCounter > 15) {
      this.brickCounter = 0;
      for (let i = 0; i < MAP_ROWS; i++) {
        for (let j = 0; j < MAP_COLS; j++) {
          if (this.map[i][j] === TileType.UP_BRICK) this.map[i][j] = TileType.BRICK;
        }
      }
    }

    // ── Counter wrapping ──────────────────────────────────────
    if (this.skyFireBallCounter >= 99) this.skyFireBallCounter = 0;
    if (this.animBallCounter >= 99) this.animBallCounter = 0;
    if (this.mushroomCounter >= 99) this.mushroomCounter = 0;
    if (this.flowerCounter >= 99) this.flowerCounter = 0;
    if (this.bulletCounter >= 100) this.bulletCounter = 0;
    if (this.animCoinCounter >= 99) this.animCoinCounter = 0;
    if (this.animBrickCounter >= 96) this.animBrickCounter = 0;
    if (this.cannonSpawnCounter >= 96) this.cannonSpawnCounter = 0;
    if (this.globalScoreCounter >= MAX_SCORE_DISPLAYS) this.globalScoreCounter = 0;

    // ── Second counter / sprite timer ─────────────────────────
    if (this.goombaSpriteCounter > 60) {
      this.goombaSpriteCounter = 0;
      if (!this.levelChangeAnimation) this.gameTime--;
    }
    if (this.marioSpriteCounter > 80) this.marioSpriteCounter = 0;
    this.walkingCounter = Math.floor(this.marioSpriteCounter / 20);

    // ── Mario transform counter ───────────────────────────────
    if (this.marioTransform) {
      this.marioTransformCounter++;
      if (this.marioTransformCounter > INVULN_FRAMES) {
        this.marioTransformCounter = 0;
        this.marioTransform = false;
      }
    }

    // ── Game Complete / Level Complete ─────────────────────────
    this.doGameComplete();
    this.doLevelComplete();

    // ── Death music trigger ───────────────────────────────────
    if (this.mario.life === MarioState.DEAD && this.deathSong && !this.mario.death) {
      this.playSfx('sfx_death');
      this.deathSong = false;
      this.mario.death = true;
    }

    // ── RENDER ────────────────────────────────────────────────
    this.renderFrame();
  }

  // ═══════════════════════════════════════════════════════════
  // LEVEL INITIALIZATION
  // ═══════════════════════════════════════════════════════════
  private initLevel(): void {
    const ld = LEVEL_DESIGNS[this.gameLevel];
    if (!ld) {
      // All levels complete - back to menu
      this.scene.start('MenuScene');
      return;
    }

    // Reset all objects and animations
    for (let i = 0; i < 12; i++) {
      for (let j = 0; j < MAX_OBJECTS; j++) {
        this.animations[i][j].life = 0;
        this.animations[i][j].xPos = 0;
        this.animations[i][j].yPos = 0;
        this.animations[i][j].move = 0;
        this.animations[i][j].down = 0;
        if (i < 10) {
          this.objects[i][j].life = 0;
          this.objects[i][j].xPos = 0;
          this.objects[i][j].yPos = 0;
          this.objects[i][j].move = 0;
          this.objects[i][j].down = 0;
        }
      }
    }

    // Reset counters
    this.mushroomCounter = 0;
    this.flowerCounter = 0;
    this.skyFireCounter = 0;
    this.skyFireBallCounter = 0;
    this.animBallCounter = 0;
    this.animBrickCounter = 0;
    this.animCoinCounter = 0;
    this.cannonCounter = 0;
    this.cannonSpawnCounter = 0;
    this.goombaSpriteCounter = 0;
    this.marioSpriteCounter = 0;
    this.marioTransform = false;
    this.marioTransformCounter = 0;
    this.spawnBrowser = false;
    this.levelChangeAnimation = false;
    this.gameComplete = false;
    this.flagDown = false;

    // Level 6: Button + Princess
    if (this.gameLevel === 6) {
      this.objects[ObjectType.BUTTON][0].life = 1;
      this.objects[ObjectType.BUTTON][0].yPos = 200;
      this.objects[ObjectType.BUTTON][0].xPos = 5506;
      this.objects[ObjectType.BUTTON][0].move = 0;
      this.animations[AnimationType.PRINCESS][0].life = 1;
      this.animations[AnimationType.PRINCESS][0].yPos = 376;
      this.animations[AnimationType.PRINCESS][0].xPos = 5929;
    }

    // Timer
    this.gameTime = LEVEL_TIME;

    // Mario start position
    this.mario.yPos = 160;
    this.mario.down = 0;
    this.mario.xPos = 16;
    this.marioFacing = 1;

    // Flag pole
    this.objects[ObjectType.FLAG_POLE][0].life = 1;
    this.objects[ObjectType.FLAG_POLE][0].yPos = 33;
    this.objects[ObjectType.FLAG_POLE][0].xPos = ld.flagPosition;
    this.animations[AnimationType.FLAG][0].life = 1;
    this.animations[AnimationType.FLAG][0].xPos = ld.flagPosition - 25;
    this.animations[AnimationType.FLAG][0].move = 0;

    // Spawn enemies and coins
    let globalSpawn = 640;
    let coinSpawn = 400;
    for (let a = 0; a < MAX_OBJECTS; a++) {
      this.objects[ObjectType.MUSHROOM][a].life = 0;
      this.objects[ObjectType.FLOWER][a].life = 0;
      const goombaSpawn = Math.floor(Math.random() * 320);
      const turtleSpawn = Math.floor(Math.random() * 320);

      if (globalSpawn < ld.ladderPosition) {
        // Goomba
        this.objects[ObjectType.GOOMBA][a].life = 1;
        this.objects[ObjectType.GOOMBA][a].yPos = ld.heightSpawn + Math.floor(Math.random() * 150);
        this.objects[ObjectType.GOOMBA][a].xPos = globalSpawn - goombaSpawn;
        this.objects[ObjectType.GOOMBA][a].move = 2;
        // Turtle
        this.objects[ObjectType.TURTLE][a].life = 2;
        this.objects[ObjectType.TURTLE][a].yPos = ld.heightSpawn + Math.floor(Math.random() * 150);
        this.objects[ObjectType.TURTLE][a].xPos = globalSpawn - turtleSpawn;
        this.objects[ObjectType.TURTLE][a].move = 1;
      }
      globalSpawn += Math.floor(ld.ladderPosition / ld.numberSpawn);

      if (coinSpawn < ld.ladderPosition) {
        this.objects[ObjectType.COIN][a].life = 1;
        this.objects[ObjectType.COIN][a].yPos = Math.floor(Math.random() * 300) + 40;
        this.objects[ObjectType.COIN][a].xPos = coinSpawn - Math.floor(Math.random() * 80);
      }
      coinSpawn += Math.floor(ld.ladderPosition / ld.numberGoldSpawn);
    }

    // Load map
    this.loadMap(this.gameLevel);

    this.levelChange = false;
    this.deathSong = true;
    this.completeSong = true;
    this.songCounter = 0;
  }

  private loadMap(level: number): void {
    const data = this.cache.text.get(`level_${level}`);
    if (!data) return;
    const lines = data.trim().split('\n');
    for (let r = 0; r < MAP_ROWS && r < lines.length; r++) {
      const nums = lines[r].trim().split(/\s+/).map(Number);
      for (let c = 0; c < MAP_COLS && c < nums.length; c++) {
        this.map[r][c] = nums[c];
      }
    }
  }

  // ═══════════════════════════════════════════════════════════
  // TOP COLLISION (Mario hitting blocks above)
  // ═══════════════════════════════════════════════════════════
  private doTopCollision(): void {
    if (this.mario.down <= 0) return;

    let i = Math.floor(this.mario.yPos / TILE_SIZE);
    const j1 = Math.floor(this.mario.xPos / TILE_SIZE);
    let j2 = Math.floor((this.mario.xPos + this.mario.width) / TILE_SIZE);
    if ((this.mario.xPos + this.mario.width) % TILE_SIZE === 0) j2--;
    if (this.mario.yPos % TILE_SIZE === 0) i--;

    for (let j = j1; j <= j2; j++) {
      if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS) continue;
      if (collide(j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, this.mario.xPos, this.mario.yPos, this.mario.width, this.mario.height)
          && this.map[i][j] !== TileType.SKY && this.mario.yPos > 0 && this.mario.life !== MarioState.DEAD) {
        this.upCollision++;
        this.playSfx('sfx_cant_brick');

        // Kill objects on breakable blocks
        if (this.map[i][j] === TileType.SHROOM_BOX || this.map[i][j] === TileType.QUESTION_BOX || this.map[i][j] === TileType.BRICK) {
          this.killObjectsOnBlock(i, j);
        }

        // Brick breaking
        if (this.map[i][j] === TileType.BRICK && this.upCollision === 1 && this.mario.life > MarioState.SMALL) {
          this.map[i][j] = TileType.SKY;
          this.spawnBrickAnimation(i, j);
          this.playSfx('sfx_brick_break');
        }
        if (this.map[i][j] === TileType.BRICK && this.upCollision === 1 && this.mario.life === MarioState.SMALL) {
          this.map[i][j] = TileType.UP_BRICK;
          this.brickCounter = 0;
        }

        // ? box -> coin
        if (this.map[i][j] === TileType.QUESTION_BOX && this.upCollision === 1) {
          this.map[i][j] = TileType.EMPTY_BOX;
          this.goldCoin++;
          this.score += SCORE_BRICK_COIN;
          this.spawnCoinAnimation(i, j);
          this.playSfx('sfx_coin');
        }

        // Shroom ? box -> power-up
        if (this.map[i][j] === TileType.SHROOM_BOX && this.upCollision === 1) {
          this.map[i][j] = TileType.EMPTY_BOX;
          this.playSfx('sfx_mushroom');
          if (this.mario.life === MarioState.SMALL) {
            const m = this.objects[ObjectType.MUSHROOM][this.mushroomCounter];
            m.life = 1;
            m.height = 0;
            m.yPos = i * TILE_SIZE;
            m.xPos = j * TILE_SIZE;
            m.move = 0;
            this.mushroomCounter++;
          } else {
            const f = this.objects[ObjectType.FLOWER][this.flowerCounter];
            f.life = 1;
            f.height = 0;
            f.yPos = i * TILE_SIZE;
            f.xPos = j * TILE_SIZE + 3;
            f.move = 0;
            this.flowerCounter++;
          }
        }
      }
    }
    if (this.upCollision !== 0) {
      this.mario.down = 0;
      this.mario.yPos = (i + 1) * TILE_SIZE;
    }
  }

  // ═══════════════════════════════════════════════════════════
  // BOTTOM COLLISION (Mario landing)
  // ═══════════════════════════════════════════════════════════
  private doBottomCollision(): void {
    const i = Math.floor((this.mario.yPos + this.mario.height) / TILE_SIZE);
    const j1 = Math.floor(this.mario.xPos / TILE_SIZE);
    let j2 = Math.floor((this.mario.xPos + this.mario.width) / TILE_SIZE);
    if ((this.mario.xPos + this.mario.width) % TILE_SIZE === 0) j2--;

    for (let j = j1; j <= j2; j++) {
      if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS) continue;
      if (collide(j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, this.mario.xPos, this.mario.yPos, this.mario.width, this.mario.height)
          && this.map[i][j] !== TileType.SKY) {
        this.downCollision++;
      }
    }

    if (this.downCollision !== 0 && this.mario.life > MarioState.DEAD && (this.mario.yPos + this.mario.height < 480)) {
      this.mario.down = 0;
      this.mario.yPos = i * TILE_SIZE - this.mario.height;
      // Ice level = slippery
      if (this.gameLevel === 2) {
        this.inertiaAmount = INERTIA_ICE;
      }
    } else if (this.downCollision === 0 || this.mario.life < MarioState.SMALL) {
      this.mario.down -= 1; // gravity
    }
  }

  // ═══════════════════════════════════════════════════════════
  // RIGHT COLLISION
  // ═══════════════════════════════════════════════════════════
  private doRightCollision(): void {
    const i1 = Math.floor(this.mario.yPos / TILE_SIZE);
    let i2 = Math.floor((this.mario.yPos + this.mario.height) / TILE_SIZE);
    if ((this.mario.yPos + this.mario.height) % TILE_SIZE === 0) i2--;
    const j = Math.floor((this.mario.xPos + this.mario.width) / TILE_SIZE);

    for (let i = i1; i <= i2; i++) {
      if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS) continue;
      if (collide(j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, this.mario.xPos, this.mario.yPos, this.mario.width, this.mario.height)
          && this.map[i][j] !== TileType.SKY && this.mario.yPos > 0) {
        this.rightCollision++;
      }
    }
    if (this.rightCollision !== 0 && this.mario.move > 0 && this.mario.life > MarioState.DEAD) {
      this.mario.xPos = j * TILE_SIZE - this.mario.width;
      this.mario.move = 0;
      this.inertiaCounter = 0;
    }
  }

  // ═══════════════════════════════════════════════════════════
  // LEFT COLLISION
  // ═══════════════════════════════════════════════════════════
  private doLeftCollision(): void {
    const i1 = Math.floor(this.mario.yPos / TILE_SIZE);
    let i2 = Math.floor((this.mario.yPos + this.mario.height) / TILE_SIZE);
    if ((this.mario.yPos + this.mario.height) % TILE_SIZE === 0) i2--;
    let j = Math.floor(this.mario.xPos / TILE_SIZE);
    if (this.mario.xPos % TILE_SIZE === 0 && this.mario.move < 0) j--;

    for (let i = i1; i <= i2; i++) {
      if (i < 0 || i >= MAP_ROWS || j < 0 || j >= MAP_COLS) continue;
      if (collide(j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, this.mario.xPos, this.mario.yPos, this.mario.width, this.mario.height)
          && this.map[i][j] !== TileType.SKY && this.mario.yPos > 0) {
        this.leftCollision++;
      }
    }
    if (this.leftCollision !== 0 && this.mario.yPos >= 0 && this.mario.life > MarioState.DEAD) {
      this.mario.xPos = (j + 1) * TILE_SIZE;
      this.mario.move = 0;
      this.inertiaCounter = 0;
    }
  }

  // ═══════════════════════════════════════════════════════════
  // MONSTER LOGIC
  // ═══════════════════════════════════════════════════════════
  private doMonsterLogic(): void {
    this.onHit = true;
    for (let a = 9; a >= 0; a--) {
      for (let b = 0; b < MAX_OBJECTS; b++) {
        const obj = this.objects[a][b];

        // Mushroom/flower grow animation
        if ((a === ObjectType.MUSHROOM || a === ObjectType.FLOWER) && obj.life > 0) {
          if (obj.height < 32) obj.height++;
          if (obj.height === 31 && a === ObjectType.MUSHROOM) obj.move = 2;
        }

        // Mario-object collision
        if (collide(this.mario.xPos, this.mario.yPos, this.mario.width, this.mario.height,
                    obj.xPos, obj.yPos, obj.width, obj.height)
            && obj.life > 0 && this.mario.life > MarioState.DEAD && this.onHit) {
          this.handleObjectCollision(a, b);
        }

        // Object falls off map
        if (obj.xPos + obj.width < 0 || obj.yPos >= 480) {
          obj.life = 0;
        }

        // Object physics
        if (obj.life > 0) {
          this.doObjectPhysics(a, b);
        }
      }
    }
  }

  private handleObjectCollision(a: number, b: number): void {
    const obj = this.objects[a][b];

    if (a === ObjectType.MUSHROOM) {
      if (this.mario.life === MarioState.SMALL) {
        this.mario.yPos -= 32;
        this.playSfx('sfx_powerup');
        this.score += SCORE_MUSHROOM;
        this.mario.life = MarioState.BIG;
      } else {
        this.score += SCORE_MUSHROOM;
      }
      this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_MUSHROOM);
      obj.life = 0;
    }

    if (a === ObjectType.GOOMBA && !this.marioTransform) {
      this.onHit = false;
      if (this.mario.down < 0 && this.mario.yPos < obj.yPos) {
        // Stomp
        this.playSfx('sfx_enemy_stomp');
        this.score += SCORE_GOOMBA_STOMP;
        this.mario.down = 15;
        obj.life = 0;
        this.mario.yPos = obj.yPos - this.mario.height;
        this.animations[AnimationType.GOOMBA_SQUASH][b].life = 1;
        this.animations[AnimationType.GOOMBA_SQUASH][b].xPos = obj.xPos;
        this.animations[AnimationType.GOOMBA_SQUASH][b].yPos = obj.yPos;
        this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_GOOMBA_STOMP);
      } else if ((this.mario.yPos + this.mario.height) < (obj.yPos + obj.height) && this.mario.down === 0) {
        obj.move = -obj.move;
      } else if (this.mario.down >= 0) {
        this.hurtMario();
        obj.move = -obj.move;
      }
    }

    if (a === ObjectType.TURTLE && !this.marioTransform) {
      this.onHit = false;
      if (this.mario.down < 0 && this.mario.yPos + this.mario.height < obj.yPos + obj.height) {
        this.mario.down = 13;
        this.mario.yPos = obj.yPos - this.mario.height;
        obj.life = 1;
        obj.deathCounter = 0;
        if (obj.life === 1 && obj.xPos >= this.xScroll && obj.xPos <= this.xScroll + 640) {
          this.playSfx('sfx_enemy_stomp');
        }
        if (obj.life === 2) obj.move = 0;
      } else if ((this.mario.yPos + this.mario.height) < (obj.yPos + obj.height)
                 && (this.mario.down === 0 && obj.life === 2 || (obj.life === 1 && obj.move === 0))) {
        obj.move = -obj.move;
      } else if (this.mario.down >= 0) {
        if (obj.life === 2 || (obj.life === 1 && obj.move !== 0)) {
          this.hurtMario();
          obj.move = -obj.move;
        }
      }
      // Shell kick
      if (obj.life === 1 && obj.move === 0 && (this.mario.move !== 0 || this.mario.down !== 0)) {
        this.playSfx('sfx_shell_kick');
        this.score += SCORE_SHELL_KICK;
        obj.move = this.marioFacing === 1 ? SHELL_SPEED : -SHELL_SPEED;
        if (this.mario.down === 0 && this.mario.move < 0) {
          this.mario.move = 0;
          this.mario.xPos = obj.xPos + obj.width + 1;
        } else if (this.mario.down === 0 && this.mario.move > 0) {
          this.mario.move = 0;
          this.mario.xPos = obj.xPos - this.mario.width - 1;
        }
        this.mario.down = 15;
      } else if (obj.life === 1 && obj.move !== 0) {
        obj.move = 0;
      }
    }

    if (a === ObjectType.METEOR && !this.marioTransform) {
      this.onHit = false;
      this.hurtMario();
      // Explosion animation
      const anim = this.animations[AnimationType.BIG_EXPLOSION][this.animBallCounter];
      anim.move = 0; anim.down = 0; anim.life = 1; anim.deathCounter = 0;
      anim.xPos = obj.xPos; anim.yPos = obj.yPos;
      obj.life = 0;
      this.animBallCounter++;
    }

    if (a === ObjectType.FLOWER) {
      if (this.mario.life === MarioState.SMALL) {
        this.mario.yPos -= 32;
        this.mario.xPos -= 10;
      }
      if (this.mario.life !== MarioState.FIRE) {
        this.playSfx('sfx_powerup');
      }
      this.score += SCORE_FLOWER;
      this.mario.life = MarioState.FIRE;
      obj.life = 0;
      this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_FLOWER);
    }

    if (a === ObjectType.COIN) {
      this.score += SCORE_COIN;
      this.goldCoin++;
      this.playSfx('sfx_coin');
      obj.life = 0;
    }

    if (a === ObjectType.CANNON && !this.marioTransform) {
      this.onHit = false;
      if ((this.mario.down < 0 && this.mario.yPos < obj.yPos) || ((this.mario.yPos + this.mario.height) < (obj.yPos + obj.height) && this.mario.down === 0)) {
        this.playSfx('sfx_enemy_stomp');
        this.score += SCORE_CANNON_STOMP;
        this.mario.down = 15;
        obj.life = 0;
        this.mario.yPos = obj.yPos - this.mario.height;
        this.animations[AnimationType.DEAD_CANNON][b].life = 1;
        this.animations[AnimationType.DEAD_CANNON][b].xPos = obj.xPos;
        this.animations[AnimationType.DEAD_CANNON][b].yPos = obj.yPos;
        this.animations[AnimationType.DEAD_CANNON][b].move = obj.move;
        this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_CANNON_STOMP);
      } else if (this.mario.down >= 0) {
        this.hurtMario();
      }
    }

    if (a === ObjectType.BUTTON) {
      if (this.mario.down < 0 && this.mario.yPos < obj.yPos) {
        obj.life = 0;
        this.objects[ObjectType.FLAG_POLE][0].down = 6;
        this.playSfx('sfx_enemy_stomp');
        // Destroy visible bricks (boss arena)
        const startC = Math.floor(this.xScroll / TILE_SIZE);
        for (let c = startC; c < startC + 20; c++) {
          for (let d = 0; d < MAP_ROWS; d++) {
            if (this.map[d][c] === TileType.BRICK) {
              this.map[d][c] = TileType.SKY;
              this.animations[AnimationType.BRICK_ANIM][this.animBrickCounter].life = 1;
              this.animations[AnimationType.BRICK_ANIM][this.animBrickCounter].yPos = (d - 1) * TILE_SIZE;
              this.animations[AnimationType.BRICK_ANIM][this.animBrickCounter].xPos = c * TILE_SIZE;
              this.animations[AnimationType.BRICK_ANIM][this.animBrickCounter].down = 2;
              this.animBrickCounter++;
              this.playSfx('sfx_brick_break');
            }
          }
        }
      } else {
        // Solid collision with button
        if (this.mario.xPos < obj.xPos) {
          this.mario.move = 0;
          this.mario.xPos = obj.xPos - this.mario.width;
        }
        if (this.mario.xPos > obj.xPos) {
          this.mario.move = 0;
          this.mario.xPos = obj.xPos + obj.width;
        }
      }
    }

    if (a === ObjectType.BOWSER && !this.marioTransform) {
      this.hurtMario();
    }

    if (a === ObjectType.FLAG_POLE) {
      this.levelChangeAnimation = true;
    }
  }

  // ═══════════════════════════════════════════════════════════
  // OBJECT PHYSICS
  // ═══════════════════════════════════════════════════════════
  private doObjectPhysics(a: number, b: number): void {
    const obj = this.objects[a][b];
    obj.yPos -= obj.down;
    obj.xPos += obj.move;

    let rightCol2 = 0, leftCol2 = 0, downCol2 = 0;

    // Bottom collision
    const bi = Math.floor((obj.yPos + obj.height) / TILE_SIZE);
    const bj1 = Math.floor(obj.xPos / TILE_SIZE);
    let bj2 = Math.floor((obj.xPos + obj.width) / TILE_SIZE);
    if ((obj.xPos + obj.width) % TILE_SIZE === 0) bj2--;
    for (let j = bj1; j <= bj2; j++) {
      if (bi < 0 || bi >= MAP_ROWS || j < 0 || j >= MAP_COLS) continue;
      if (collide(j * TILE_SIZE, bi * TILE_SIZE, TILE_SIZE, TILE_SIZE, obj.xPos, obj.yPos, obj.width, obj.height)
          && this.map[bi][j] !== TileType.SKY && a !== ObjectType.COIN && a !== ObjectType.CANNON && obj.yPos > 0) {
        downCol2++;
        // Meteor destroys tile on impact
        if (a === ObjectType.METEOR && obj.life !== 0 && obj.yPos > 0) {
          this.map[bi][j] = TileType.SKY;
          const anim = this.animations[AnimationType.BIG_EXPLOSION][this.animBallCounter];
          anim.move = 0; anim.down = 0; anim.life = 1; anim.deathCounter = 0;
          anim.xPos = obj.xPos; anim.yPos = obj.yPos;
          obj.life = 0;
          this.animBallCounter++;
        }
      }
    }
    if ((downCol2 === 0 || obj.yPos + obj.height > 480) && a !== ObjectType.COIN && a !== ObjectType.CANNON) {
      obj.down -= 1;
    } else if (downCol2 !== 0 || a === ObjectType.COIN) {
      obj.down = 0;
      obj.yPos = bi * TILE_SIZE - obj.height;
    }

    // Right collision
    let ri1 = Math.floor(obj.yPos / TILE_SIZE);
    let ri2 = Math.floor((obj.yPos + obj.height) / TILE_SIZE);
    if ((obj.yPos + obj.height) % TILE_SIZE === 0) ri2--;
    const rj = Math.floor((obj.xPos + obj.width) / TILE_SIZE);
    for (let i = ri1; i <= ri2; i++) {
      if (i < 0 || i >= MAP_ROWS || rj < 0 || rj >= MAP_COLS) continue;
      if (collide(rj * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, obj.xPos, obj.yPos, obj.width, obj.height)
          && this.map[i][rj] !== TileType.SKY && a !== ObjectType.CANNON) {
        rightCol2++;
        if (rj > 0 && this.map[i][rj - 1] !== TileType.SKY && a !== ObjectType.CANNON) {
          obj.life = 0;
        }
      }
    }
    if (rightCol2 !== 0 && obj.yPos >= 0) {
      obj.xPos = rj * TILE_SIZE - obj.width;
      obj.move = -obj.move;
      if (obj.life === 1 && a === ObjectType.TURTLE && obj.xPos >= this.xScroll && obj.xPos <= this.xScroll + 640 && obj.move > 0) {
        this.playSfx('sfx_shell_kick');
      }
    }

    // Left collision
    const li1 = Math.floor(obj.yPos / TILE_SIZE);
    let li2 = Math.floor((obj.yPos + obj.height) / TILE_SIZE);
    if ((obj.yPos + obj.height) % TILE_SIZE === 0) li2--;
    let lj = Math.floor(obj.xPos / TILE_SIZE);
    if (obj.xPos % TILE_SIZE === 0 && obj.move < 0) lj--;
    for (let i = li1; i <= li2; i++) {
      if (i < 0 || i >= MAP_ROWS || lj < 0 || lj >= MAP_COLS) continue;
      if (collide(lj * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, obj.xPos, obj.yPos, obj.width, obj.height)
          && this.map[i][lj] !== TileType.SKY && a !== ObjectType.CANNON) {
        leftCol2++;
        if (lj + 1 < MAP_COLS && this.map[i][lj + 1] !== TileType.SKY && a !== ObjectType.CANNON) {
          obj.life = 0;
        }
      }
    }
    if (leftCol2 !== 0 && obj.yPos >= 0) {
      obj.xPos = (lj + 1) * TILE_SIZE;
      obj.move = -obj.move;
      if (obj.life === 1 && a === ObjectType.TURTLE && obj.xPos >= this.xScroll && obj.xPos <= this.xScroll + 640 && obj.move > 0) {
        this.playSfx('sfx_shell_kick');
      }
    }
  }

  // ═══════════════════════════════════════════════════════════
  // UNIT (GOOMBA/TURTLE) INTER-COLLISION
  // ═══════════════════════════════════════════════════════════
  private doUnitCollision(): void {
    for (let a = 1; a < 3; a++) {
      for (let b = 0; b < MAX_OBJECTS; b++) {
        for (let c = a; c < 3; c++) {
          const eStart = c === a ? b + 1 : 0;
          for (let d = eStart; d < MAX_OBJECTS; d++) {
            const objA = this.objects[a][b];
            const objC = this.objects[c][d];
            if (objA.life !== 0 && objC.life !== 0 &&
                collide(objC.xPos, objC.yPos, objC.width, objC.height, objA.xPos, objA.yPos, objA.width, objA.height)) {
              let turtleTurning = false;

              // Shell kills other enemies
              if ((objA.life === 1 || objC.life === 1) && (c === ObjectType.TURTLE || a === ObjectType.TURTLE)) {
                if (c === ObjectType.TURTLE && objC.life === 1 && objC.move !== 0) {
                  this.killEnemyByShell(a, b, objA);
                  turtleTurning = true;
                }
                if (a === ObjectType.TURTLE && objA.life === 1 && objA.move !== 0) {
                  this.killEnemyByShell(c, d, objC);
                  turtleTurning = true;
                }
              }

              if (!turtleTurning) {
                // Bounce off each other
                if (objC.move < 0 && objA.move < 0 && objC.xPos < objA.xPos) objA.move = -objA.move;
                else if (objC.move > 0 && objA.move > 0 && objC.xPos > objA.xPos) objA.move = -objA.move;
                else if (objC.move < 0 && objA.move < 0 && objC.xPos > objA.xPos) objC.move = -objC.move;
                else if (objC.move > 0 && objA.move > 0 && objC.xPos < objA.xPos) objC.move = -objC.move;
                else {
                  if (objC.xPos < objA.xPos) objA.xPos = objC.xPos + objC.width;
                  else if (objC.xPos > objA.xPos) objC.xPos = objA.xPos + objA.width;
                  objA.move = -objA.move;
                  objC.move = -objC.move;
                }
              }
            }
          }
        }
      }
    }
  }

  private killEnemyByShell(type: number, idx: number, obj: Movement): void {
    if (type === ObjectType.GOOMBA) {
      this.animations[AnimationType.GOOMBA_DEAD][idx].life = 1;
      this.animations[AnimationType.GOOMBA_DEAD][idx].xPos = obj.xPos;
      this.animations[AnimationType.GOOMBA_DEAD][idx].yPos = obj.yPos;
      this.animations[AnimationType.GOOMBA_DEAD][idx].move = obj.move;
      this.animations[AnimationType.GOOMBA_DEAD][idx].down = 4;
    } else if (type === ObjectType.TURTLE) {
      this.animations[AnimationType.TURTLE_DEAD][idx].life = 1;
      this.animations[AnimationType.TURTLE_DEAD][idx].xPos = obj.xPos;
      this.animations[AnimationType.TURTLE_DEAD][idx].yPos = obj.yPos;
      this.animations[AnimationType.TURTLE_DEAD][idx].move = obj.move;
      this.animations[AnimationType.TURTLE_DEAD][idx].down = 4;
    }
    obj.life = 0;
    this.score += SCORE_SHELL_KILL;
    this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_SHELL_KILL);
  }

  // ═══════════════════════════════════════════════════════════
  // BULLET LOGIC
  // ═══════════════════════════════════════════════════════════
  private doBulletLogic(): void {
    for (let a = 0; a < MAX_BULLETS; a++) {
      const b = this.bullets[a];
      if (b.life <= 0) continue;

      b.yPos -= b.down;
      b.xPos += b.move;
      let rightCol3 = 0, leftCol3 = 0, downCol3 = 0, upCol3 = 0;

      // Top collision
      if (b.down > 0) {
        let bi = Math.floor(b.yPos / TILE_SIZE);
        const bj1 = Math.floor(b.xPos / TILE_SIZE);
        let bj2 = Math.floor((b.xPos + b.width) / TILE_SIZE);
        if ((b.xPos + b.width) % TILE_SIZE === 0) bj2--;
        if (b.yPos % TILE_SIZE === 0) bi--;
        for (let j = bj1; j <= bj2; j++) {
          if (bi < 0 || bi >= MAP_ROWS || j < 0 || j >= MAP_COLS) continue;
          if (collide(j * TILE_SIZE, bi * TILE_SIZE, TILE_SIZE, TILE_SIZE, b.xPos, b.yPos, b.width, b.height) && this.map[bi][j] !== TileType.SKY && b.yPos > 0) {
            upCol3++;
          }
        }
        if (upCol3 !== 0) {
          this.animations[AnimationType.EXPLOSION][a].life = 1;
          this.animations[AnimationType.EXPLOSION][a].xPos = b.xPos;
          this.animations[AnimationType.EXPLOSION][a].yPos = b.yPos;
          b.life = 0;
          continue;
        }
      }

      // Bottom collision
      const di = Math.floor((b.yPos + b.height) / TILE_SIZE);
      const dj1 = Math.floor(b.xPos / TILE_SIZE);
      let dj2 = Math.floor((b.xPos + b.width) / TILE_SIZE);
      if ((b.xPos + b.width) % TILE_SIZE === 0) dj2--;
      for (let j = dj1; j <= dj2; j++) {
        if (di < 0 || di >= MAP_ROWS || j < 0 || j >= MAP_COLS) continue;
        if (collide(j * TILE_SIZE, di * TILE_SIZE, TILE_SIZE, TILE_SIZE, b.xPos, b.yPos, b.width, b.height) && this.map[di][j] !== TileType.SKY) {
          downCol3++;
        }
      }
      if (downCol3 === 0) b.down -= 1;
      if (downCol3 !== 0) { b.down = FIREBALL_BOUNCE; b.yPos = di * TILE_SIZE - b.height; }
      if (b.yPos > 480) { b.life = 0; continue; }

      // Right collision
      const ri1 = Math.floor(b.yPos / TILE_SIZE);
      let ri2 = Math.floor((b.yPos + b.height) / TILE_SIZE);
      if ((b.yPos + b.height) % TILE_SIZE === 0) ri2--;
      const rj = Math.floor((b.xPos + b.width) / TILE_SIZE);
      for (let i = ri1; i <= ri2; i++) {
        if (i < 0 || i >= MAP_ROWS || rj < 0 || rj >= MAP_COLS) continue;
        if (collide(rj * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, b.xPos, b.yPos, b.width, b.height) && this.map[i][rj] !== TileType.SKY) {
          rightCol3++;
        }
      }
      if (rightCol3 !== 0) { this.bulletExplode(a, b); continue; }

      // Left collision
      const li1 = Math.floor(b.yPos / TILE_SIZE);
      let li2 = Math.floor((b.yPos + b.height) / TILE_SIZE);
      if ((b.yPos + b.height) % TILE_SIZE === 0) li2--;
      let lj = Math.floor(b.xPos / TILE_SIZE);
      if (b.xPos % TILE_SIZE === 0 && b.move < 0) lj--;
      for (let i = li1; i <= li2; i++) {
        if (i < 0 || i >= MAP_ROWS || lj < 0 || lj >= MAP_COLS) continue;
        if (collide(lj * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, b.xPos, b.yPos, b.width, b.height) && this.map[i][lj] !== TileType.SKY) {
          leftCol3++;
        }
      }
      if (leftCol3 !== 0) { this.bulletExplode(a, b); continue; }

      // Bullet vs enemies (goomba, turtle; bowser immune)
      for (let c = 0; c < 3; c++) {
        for (let d = 0; d < MAX_OBJECTS; d++) {
          const obj = this.objects[c][d];
          if (collide(obj.xPos, obj.yPos, obj.width, obj.height, b.xPos, b.yPos, b.width, b.height) && b.life !== 0 && obj.life !== 0) {
            this.bulletExplode(a, b);
            if (c === ObjectType.GOOMBA) {
              obj.life = 0;
              this.playSfx('sfx_shell_kick');
              this.score += SCORE_GOOMBA_FIRE;
              this.animations[AnimationType.GOOMBA_DEAD][d].life = 1;
              this.animations[AnimationType.GOOMBA_DEAD][d].xPos = obj.xPos;
              this.animations[AnimationType.GOOMBA_DEAD][d].yPos = obj.yPos;
              this.animations[AnimationType.GOOMBA_DEAD][d].move = obj.move;
              this.animations[AnimationType.GOOMBA_DEAD][d].down = 3;
              this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_GOOMBA_FIRE);
            } else if (c === ObjectType.TURTLE) {
              obj.life = 0;
              this.playSfx('sfx_shell_kick');
              this.score += SCORE_TURTLE_FIRE;
              this.animations[AnimationType.TURTLE_DEAD][d].life = 1;
              this.animations[AnimationType.TURTLE_DEAD][d].xPos = obj.xPos;
              this.animations[AnimationType.TURTLE_DEAD][d].yPos = obj.yPos;
              this.animations[AnimationType.TURTLE_DEAD][d].move = obj.move;
              this.animations[AnimationType.TURTLE_DEAD][d].down = 3;
              this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_TURTLE_FIRE);
            }
          }
        }
      }
    }
  }

  private bulletExplode(idx: number, b: Movement): void {
    this.animations[AnimationType.EXPLOSION][idx].life = 1;
    this.animations[AnimationType.EXPLOSION][idx].xPos = b.xPos;
    this.animations[AnimationType.EXPLOSION][idx].yPos = b.yPos;
    b.life = 0;
  }

  // ═══════════════════════════════════════════════════════════
  // ANIMATION UPDATES
  // ═══════════════════════════════════════════════════════════
  private doAnimationUpdates(): void {
    for (let a = 0; a < 12; a++) {
      for (let b = 0; b < MAX_OBJECTS; b++) {
        const anim = this.animations[a][b];
        if (anim.life > 0) {
          if (!this.levelChangeAnimation) {
            this.animations[AnimationType.FLAG][0].yPos = this.objects[ObjectType.FLAG_POLE][0].yPos + 17;
            this.animations[AnimationType.FLAG][0].down = 0;
          }
          if (a !== AnimationType.GOOMBA_SQUASH && a !== AnimationType.EXPLOSION
              && a !== AnimationType.ONE_UP && a !== AnimationType.PRINCESS
              && a !== AnimationType.END_MESSAGE && a !== AnimationType.BIG_EXPLOSION) {
            anim.yPos -= anim.down;
          }
          anim.xPos += anim.move;
          anim.down -= 1;
          if (anim.yPos > 480 || ((a === AnimationType.COIN_ANIM || a === AnimationType.GOOMBA_SQUASH
              || a === AnimationType.EXPLOSION || a === AnimationType.ONE_UP || a === AnimationType.PRINCESS
              || a === AnimationType.END_MESSAGE || a === AnimationType.BIG_EXPLOSION)
              && anim.deathCounter > 30)) {
            anim.life = 0;
            anim.deathCounter = 0;
            if (a === AnimationType.COIN_ANIM) {
              this.addScoreDisplay(anim.xPos, anim.yPos, SCORE_BRICK_COIN);
            }
          }
        }
      }
    }
  }

  // ═══════════════════════════════════════════════════════════
  // LEVEL-SPECIFIC SPAWNS (meteors, cannons)
  // ═══════════════════════════════════════════════════════════
  private doLevelSpecificSpawns(): void {
    const ld = LEVEL_DESIGNS[this.gameLevel];

    // Falling meteors (level 4 & 5)
    if (this.gameLevel === 5 || this.gameLevel === 4) {
      this.skyFireCounter++;
      for (let b = 0; b < MAX_OBJECTS; b++) {
        this.objects[ObjectType.METEOR][b].down = -2;
      }
      if (this.skyFireCounter > 85 && this.mario.xPos < ld.ladderPosition - 600) {
        const m = this.objects[ObjectType.METEOR][this.skyFireBallCounter];
        m.life = 1;
        m.yPos = -32;
        m.down = 0;
        m.xPos = this.mario.xPos + Math.floor(Math.random() * 720) + 90;
        m.move = m.xPos % 2 === 0 ? 1 : -1;
        this.skyFireCounter = 0;
        this.skyFireBallCounter++;
      }
    }

    // Cannons (level 3 & 5)
    if (this.gameLevel === 5 || this.gameLevel === 3) {
      this.cannonCounter++;
      if (this.cannonCounter > 100 && this.mario.xPos < ld.ladderPosition - 400) {
        const cn = this.objects[ObjectType.CANNON][this.cannonSpawnCounter];
        cn.life = 1;
        cn.yPos = this.mario.yPos - 40 + Math.floor(Math.random() * 80);
        cn.down = 0;
        cn.xPos = this.mario.xPos + 640;
        cn.move = -2;
        this.cannonCounter = 0;
        this.cannonSpawnCounter++;
      }
      if (this.levelChangeAnimation || this.gameComplete) {
        for (let i = 0; i < MAX_OBJECTS; i++) {
          this.objects[ObjectType.CANNON][i].life = 0;
        }
      }
    }
  }

  // ═══════════════════════════════════════════════════════════
  // BOSS FIGHT
  // ═══════════════════════════════════════════════════════════
  private doBossFight(): void {
    if (this.mario.xPos > BOWSER_TRIGGER_X && this.gameLevel === 6 && !this.spawnBrowser) {
      this.objects[ObjectType.BOWSER][0].life = 1;
      this.objects[ObjectType.BOWSER][0].yPos = 0;
      this.objects[ObjectType.BOWSER][0].xPos = BOWSER_SPAWN_X;
      this.objects[ObjectType.BOWSER][0].move = 1;
      this.objects[ObjectType.BOWSER][0].down = 0;
      this.browserJump = 0;
      this.browserTurtle = 0;
      this.spawnBrowser = true;
    }

    const bowser = this.objects[ObjectType.BOWSER][0];
    if (bowser.life === 1) {
      this.browserJump++;
      this.browserTurtle++;

      if (bowser.xPos > BOWSER_MAX_X) bowser.move = -Math.abs(bowser.move);
      else if (bowser.xPos < BOWSER_MIN_X) bowser.move = Math.abs(bowser.move);

      if (this.browserJump > BOWSER_JUMP_INTERVAL && this.objects[ObjectType.BUTTON][0].life === 1) {
        bowser.down = BOWSER_JUMP_FORCE;
        this.browserJump = 0;
      }
      if (this.browserTurtle > BOWSER_TURTLE_INTERVAL && this.objects[ObjectType.BUTTON][0].life === 1) {
        const t = this.objects[ObjectType.TURTLE][this.flowerCounter];
        t.life = 1;
        t.yPos = bowser.yPos + 20;
        t.xPos = bowser.xPos - t.width - 1;
        t.move = -SHELL_SPEED;
        this.browserTurtle = 0;
        this.flowerCounter++;
      }

      // Lock scroll in boss arena
      if (this.xScroll > BOSS_SCROLL_MAX) this.xScroll = BOSS_SCROLL_MAX;
      else if (this.xScroll < BOSS_SCROLL_MIN) this.xScroll = BOSS_SCROLL_MIN;
      this.marioScrollPosition = this.mario.xPos - this.xScroll;
      if (this.mario.xPos < this.xScroll) { this.mario.xPos = this.xScroll; this.mario.move = 0; }
      if (this.mario.xPos > this.xScroll + 640 + this.mario.width) {
        this.mario.xPos = this.xScroll + 640 - this.mario.width; this.mario.move = 0;
      }

      // Bowser vs turtle shell collision
      for (let b = 0; b < MAX_OBJECTS; b++) {
        const t = this.objects[ObjectType.TURTLE][b];
        if (t.life !== 0 && collide(bowser.xPos, bowser.yPos, bowser.width, bowser.height, t.xPos, t.yPos, t.width, t.height)) {
          t.life = 0;
          this.animations[AnimationType.TURTLE_DEAD][b].life = 1;
          this.animations[AnimationType.TURTLE_DEAD][b].xPos = t.xPos;
          this.animations[AnimationType.TURTLE_DEAD][b].yPos = t.yPos;
          this.animations[AnimationType.TURTLE_DEAD][b].move = t.move;
          this.animations[AnimationType.TURTLE_DEAD][b].down = 3;
        }
      }
    }

    if (bowser.life === 0 && this.spawnBrowser) {
      this.gameComplete = true;
    }
  }

  // ═══════════════════════════════════════════════════════════
  // GAME COMPLETE (beat bowser)
  // ═══════════════════════════════════════════════════════════
  private doGameComplete(): void {
    if (!this.gameComplete) return;

    if (this.completeSong) {
      this.songCounter++;
      if (this.songCounter > 510) {
        this.songCounter = 0;
        this.completeSong = false;
      }
    } else if (this.mario.xPos < 5850) {
      this.mario.move = 3;
      this.marioFacing = 1;
    }

    if (this.mario.xPos > 5850 && this.songCounter === 0) {
      this.mario.move = 0;
      this.songCounter++;
    } else if (this.mario.xPos > 5850 && this.songCounter !== 0) {
      this.animations[AnimationType.END_MESSAGE][0].life = 1;
      this.animations[AnimationType.END_MESSAGE][0].yPos = 255;
      this.animations[AnimationType.END_MESSAGE][0].xPos = this.xScroll + 280;
      if (this.keys.x.isDown) {
        this.scene.start('MenuScene');
      }
    }
  }

  // ═══════════════════════════════════════════════════════════
  // LEVEL COMPLETE (flag touched)
  // ═══════════════════════════════════════════════════════════
  private doLevelComplete(): void {
    if (!this.levelChangeAnimation) return;
    const ld = LEVEL_DESIGNS[this.gameLevel];

    if (this.completeSong) {
      this.songCounter++;
      if (this.songCounter > 360) {
        this.songCounter = 0;
        this.completeSong = false;
      }
    }

    // Mario falls down flag pole
    if (this.downCollision === 0 && this.flagTouch) {
      this.score += this.gameTime * 10;
      this.gameTime = 0;
      this.mario.xPos = this.objects[ObjectType.FLAG_POLE][0].xPos + 7 - this.mario.width;
      this.marioFacing = 1;
      this.mario.down = -1;
      this.mario.move = 0;
    } else {
      this.flagTouch = false;
    }

    if (this.songCounter === 1) {
      this.flagTouch = true;
    }

    // Flag descends
    const flagAnim = this.animations[AnimationType.FLAG][0];
    const pole = this.objects[ObjectType.FLAG_POLE][0];
    if ((flagAnim.yPos + 34) < pole.yPos + pole.height) {
      flagAnim.down = -3;
    } else {
      this.flagDown = true;
      flagAnim.down = 0;
      flagAnim.yPos = pole.yPos + pole.height - 34;
    }

    // Mario walks to castle
    if (!this.flagTouch && this.flagDown) {
      this.marioFacing = 1;
      this.mario.move = 3;
    }

    // Enter castle
    if (this.mario.xPos > ld.castlePosition + 75) {
      this.mario.move = 0;
      if (!this.completeSong) {
        this.levelChange = true;
        this.gameLevel++;
        if (this.gameLevel > 6) {
          this.scene.start('MenuScene');
          return;
        }
        // Show level info screen
        this.scene.start('LevelInfoScene', {
          score: this.score,
          goldCoin: this.goldCoin,
          lives: this.lives,
          gameLevel: this.gameLevel,
          marioLife: this.mario.life,
        } as GameState);
      }
    }
  }

  // ═══════════════════════════════════════════════════════════
  // HELPERS
  // ═══════════════════════════════════════════════════════════
  private hurtMario(): void {
    if (this.mario.life > MarioState.SMALL) {
      this.mario.life--;
      this.playSfx('sfx_hurt');
    } else if (this.mario.life === MarioState.SMALL) {
      this.mario.life = MarioState.DEAD;
    }
    if (this.mario.life === MarioState.DEAD) {
      this.mario.down = 15;
    } else {
      this.marioTransform = true;
    }
  }

  private playSfx(key: string): void {
    try { this.sound.play(key); } catch { /* ok */ }
  }

  private addScoreDisplay(x: number, y: number, score: number): void {
    const gs = this.gameScores[this.globalScoreCounter];
    gs.life = 1;
    gs.xPos = x;
    gs.yPos = y;
    gs.score = score;
    gs.timeCounter = 60;
    this.globalScoreCounter++;
  }

  private killObjectsOnBlock(i: number, j: number): void {
    if (this.upCollision !== 1) return;
    for (let a = 1; a < 10; a++) {
      for (let b = 0; b < MAX_OBJECTS; b++) {
        if ((a === ObjectType.GOOMBA || a === ObjectType.TURTLE || a === ObjectType.COIN || a === ObjectType.MUSHROOM) && this.objects[a][b].life > 0) {
          const obj = this.objects[a][b];
          if (collide(j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE, obj.xPos, obj.yPos, obj.width, obj.height) && this.map[i][j] !== TileType.SKY) {
            const centerX = obj.xPos + obj.width / 2;
            if ((centerX > j * TILE_SIZE && centerX < (j + 1) * TILE_SIZE) || a === ObjectType.MUSHROOM) {
              if (a !== ObjectType.MUSHROOM && a !== ObjectType.COIN) {
                obj.life = 0;
              }
              if (a === ObjectType.MUSHROOM && this.map[i][j] !== TileType.SHROOM_BOX) {
                obj.move = -obj.move;
              } else if (a === ObjectType.GOOMBA) {
                this.score += SCORE_GOOMBA_FIRE;
                this.animations[AnimationType.GOOMBA_DEAD][b].life = 1;
                this.animations[AnimationType.GOOMBA_DEAD][b].xPos = obj.xPos;
                this.animations[AnimationType.GOOMBA_DEAD][b].yPos = obj.yPos;
                this.animations[AnimationType.GOOMBA_DEAD][b].move = obj.move;
                this.animations[AnimationType.GOOMBA_DEAD][b].down = 5;
                this.playSfx('sfx_shell_kick');
                this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_GOOMBA_FIRE);
              } else if (a === ObjectType.TURTLE) {
                this.score += SCORE_GOOMBA_FIRE;
                this.animations[AnimationType.TURTLE_DEAD][b].life = 1;
                this.animations[AnimationType.TURTLE_DEAD][b].xPos = obj.xPos;
                this.animations[AnimationType.TURTLE_DEAD][b].yPos = obj.yPos;
                this.animations[AnimationType.TURTLE_DEAD][b].move = obj.move;
                this.animations[AnimationType.TURTLE_DEAD][b].down = 5;
                this.playSfx('sfx_shell_kick');
                this.addScoreDisplay(obj.xPos, obj.yPos, SCORE_GOOMBA_FIRE);
              } else if (a === ObjectType.COIN) {
                if (obj.yPos < i * TILE_SIZE && obj.xPos > j * TILE_SIZE && obj.xPos < (j + 1) * TILE_SIZE) {
                  obj.life = 0;
                  this.goldCoin++;
                  this.score += SCORE_COIN;
                  this.spawnCoinAnimation(i, j);
                  this.playSfx('sfx_coin');
                }
              }
            }
          }
        }
      }
    }
  }

  private spawnBrickAnimation(i: number, j: number): void {
    let e = 0;
    for (let a = this.animBrickCounter; a < this.animBrickCounter + 4; a++) {
      e++;
      const anim = this.animations[AnimationType.BRICK_ANIM][a];
      anim.life = 1;
      anim.yPos = (i - 1) * TILE_SIZE;
      anim.xPos = j * TILE_SIZE;
      anim.down = e % 2 === 0 ? 5 : 2;
      anim.move = e < 3 ? 2 : -2;
    }
    this.animBrickCounter += 4;
  }

  private spawnCoinAnimation(i: number, j: number): void {
    const anim = this.animations[AnimationType.COIN_ANIM][this.animCoinCounter];
    anim.life = 1;
    anim.yPos = (i - 1) * TILE_SIZE;
    anim.xPos = j * TILE_SIZE + 6;
    anim.down = 15;
    this.animCoinCounter++;
  }

  // ═══════════════════════════════════════════════════════════
  // RENDER FRAME
  // ═══════════════════════════════════════════════════════════
  private renderFrame(): void {
    // Reset sprite pool
    for (let i = 0; i < this.spritePoolIndex; i++) {
      this.spritePool[i].setVisible(false);
    }
    this.spritePoolIndex = 0;
    this.resetScoreTexts();

    const lvl = this.gameLevel;
    const ld = LEVEL_DESIGNS[lvl];
    const xs = this.xScroll;

    // Background
    this.drawSprite('panorama_bg_' + lvl, -(xs / 30), 0);
    // Foreground parallax
    for (let i = 0; i < 10; i++) {
      this.drawSprite('panorama_' + lvl, this.backgroundScroll[i], 0);
    }

    // Castle
    this.drawSprite('small_castle', ld.castlePosition - xs, 192);

    // ── Tile rendering ────────────────────────────────────────
    const startCol = Math.floor(xs / TILE_SIZE);
    for (let i = 0; i < 16; i++) {
      for (let j = startCol; j < startCol + 21; j++) {
        if (i >= MAP_ROWS || j >= MAP_COLS || j < 0) continue;
        const tile = this.map[i][j];
        const tx = j * TILE_SIZE - xs;
        const ty = i * TILE_SIZE;

        switch (tile) {
          case TileType.GROUND_SOIL:
            this.drawSprite(`in_ground_${lvl}_${j % 5}`, tx, ty);
            break;
          case TileType.GROUND_GRASS:
            this.drawSprite(`out_ground_${lvl}_${j % 3}`, tx, ty);
            break;
          case TileType.BRICK:
            this.drawSprite(ld.backgroundType === 1 ? 'night_brick' : 'brick', tx, ty);
            break;
          case TileType.QUESTION_BOX:
          case TileType.SHROOM_BOX: {
            const frame = Math.floor(this.goombaSpriteCounter / 20);
            this.drawSprite(frame === 2 ? 'box_d' : frame === 1 ? 'box_l' : 'box_m', tx, ty);
            break;
          }
          case TileType.TUBE_EXT_LEFT: this.drawSprite('tube_ext_left', tx, ty); break;
          case TileType.TUBE_EXT_RIGHT: this.drawSprite('tube_ext_right', tx, ty); break;
          case TileType.TUBE_TOP_LEFT: this.drawSprite('tube_top_left', tx, ty); break;
          case TileType.TUBE_TOP_RIGHT: this.drawSprite('tube_top_right', tx, ty); break;
          case TileType.BLOCK: this.drawSprite('block', tx, ty); break;
          case TileType.EMPTY_BOX: this.drawSprite('box_hit', tx, ty); break;
          case TileType.POLE: this.drawSprite('pole', tx, ty); break;
          case TileType.POLE_HEAD: this.drawSprite('pole_head', tx, ty); break;
          case TileType.UP_BRICK: this.drawSprite(ld.backgroundType === 1 ? 'night_brick' : 'brick', tx, ty - 5); break;
        }
      }
    }

    // ── Game objects ──────────────────────────────────────────
    for (let i = 0; i < 10; i++) {
      for (let j = 0; j < MAX_OBJECTS; j++) {
        const obj = this.objects[i][j];
        if (obj.life <= 0) continue;
        const ox = obj.xPos - xs;
        const oy = obj.yPos;
        if (ox < -100 || ox > SCREEN_WIDTH + 100) continue;

        switch (i) {
          case ObjectType.METEOR: this.drawSprite('sky_fire', ox, oy); break;
          case ObjectType.MUSHROOM: this.drawSpriteClip('mushroom', ox, oy, obj.width, obj.height); break;
          case ObjectType.GOOMBA:
            this.drawSprite(Math.floor(this.goombaSpriteCounter / 10) % 2 === 0 ? 'goomba_l' : 'goomba_r', ox, oy);
            break;
          case ObjectType.TURTLE:
            if (obj.life === 2) {
              const frame2 = Math.floor(this.goombaSpriteCounter / 10) % 2;
              if (obj.move > 0) this.drawSprite(frame2 === 0 ? 'turtle_r1' : 'turtle_r2', ox, oy);
              else this.drawSprite(frame2 === 0 ? 'turtle_l1' : 'turtle_l2', ox, oy);
            } else {
              this.drawSprite('turtle_shell', ox, oy);
            }
            break;
          case ObjectType.FLOWER: {
            const fkey = Math.floor(this.goombaSpriteCounter / 30) % 2 === 0 ? 'flower2' : 'flower1';
            this.drawSpriteClip(fkey, ox, oy, obj.width, obj.height);
            break;
          }
          case ObjectType.COIN: {
            const cf = Math.floor(this.goombaSpriteCounter / 20);
            this.drawSprite(cf === 2 ? 'coin_d' : cf === 1 ? 'coin_l' : 'coin_m', ox, oy);
            break;
          }
          case ObjectType.BUTTON: this.drawSprite('button', ox, oy); break;
          case ObjectType.BOWSER: {
            if (obj.down === 0) {
              this.drawSprite(Math.floor(this.goombaSpriteCounter / 10) % 2 === 0 ? 'bowser_walk1' : 'bowser_walk2', ox, oy);
            } else if (obj.yPos <= 352) {
              this.drawSprite('bowser_jump', ox, oy);
            } else {
              this.drawSprite('bowser_fall', ox, oy);
            }
            break;
          }
          case ObjectType.FLAG_POLE: this.drawSprite('flag_pole', ox, oy); break;
          case ObjectType.CANNON: this.drawSprite('cannon_bullet', ox, oy); break;
        }
      }
    }

    // ── Animations ────────────────────────────────────────────
    const animKeys: Record<number, string> = {
      [AnimationType.COIN_ANIM]: 'box_coin',
      [AnimationType.BRICK_ANIM]: ld.backgroundType === 1 ? 'blue_broken_brick' : 'broken_brick',
      [AnimationType.GOOMBA_SQUASH]: 'goomba_squash',
      [AnimationType.GOOMBA_DEAD]: 'goomba_dead',
      [AnimationType.TURTLE_DEAD]: 'turtle_dead',
      [AnimationType.EXPLOSION]: 'explosion',
      [AnimationType.ONE_UP]: 'one_up',
      [AnimationType.PRINCESS]: 'princess',
      [AnimationType.END_MESSAGE]: 'end_message',
      [AnimationType.FLAG]: 'flag',
      [AnimationType.BIG_EXPLOSION]: 'big_explosion',
      [AnimationType.DEAD_CANNON]: 'cannon_bullet',
    };

    for (let i = 0; i < 12; i++) {
      for (let j = 0; j < MAX_OBJECTS; j++) {
        const anim = this.animations[i][j];
        if (anim.life > 0 && animKeys[i]) {
          this.drawSprite(animKeys[i], anim.xPos - xs, anim.yPos);
        }
      }
    }

    // ── Fireballs ─────────────────────────────────────────────
    for (let j = 0; j < MAX_BULLETS; j++) {
      const b = this.bullets[j];
      if (b.life > 0) {
        const ff = Math.floor(this.goombaSpriteCounter / 15);
        const fk = ff === 1 ? 'fireball1' : ff === 2 ? 'fireball2' : ff === 3 ? 'fireball3' : 'fireball4';
        this.drawSprite(fk, b.xPos - xs, b.yPos);
      }
    }

    // ── Mario ─────────────────────────────────────────────────
    if ((this.mario.xPos + this.mario.width) < (ld.castlePosition + 95)) {
      if (this.marioTransform && Math.floor(this.goombaSpriteCounter / 10) % 2 !== 0) {
        this.drawSprite('flash_mario', this.marioScrollPosition, this.mario.yPos);
      } else if (this.mario.life === MarioState.DEAD) {
        this.drawSprite('dead_mario', this.marioScrollPosition, this.mario.yPos);
      } else {
        this.drawMarioSprite();
      }
    }

    // ── Score popups ──────────────────────────────────────────
    for (let i = 0; i < MAX_SCORE_DISPLAYS; i++) {
      const gs = this.gameScores[i];
      if (gs.life > 0) {
        this.drawText(String(gs.score), gs.xPos - xs, gs.yPos);
      }
    }

    // ── HUD update ────────────────────────────────────────────
    this.hudTexts[3].setText(String(this.score).padStart(6, ' '));
    this.hudTexts[4].setText(String(this.goldCoin).padStart(2, ' '));
    this.hudTexts[5].setText(String(this.gameLevel).padStart(2, ' '));
    this.hudTexts[6].setText(String(Math.max(0, this.gameTime)).padStart(3, ' '));
  }

  private drawMarioSprite(): void {
    const mx = this.marioScrollPosition;
    const my = this.mario.yPos;
    const wc = this.walkingCounter;
    const dc = this.downCollision;
    const mv = this.mario.move;

    if (this.mario.life === MarioState.SMALL) {
      if (this.marioFacing === 1) {
        if (dc === 0) this.drawSprite('small_jump_r', mx, my);
        else if (mv > 1 && wc === 0) this.drawSprite('small_walk_r_1', mx, my);
        else if (mv > 1 && (wc === 1 || wc === 3)) this.drawSprite('small_walk_r_2', mx, my);
        else if (mv < 0) this.drawSprite('small_turn_r', mx, my);
        else this.drawSprite('small_stand_r', mx, my);
      } else {
        if (dc === 0) this.drawSprite('small_jump_l', mx, my);
        else if (mv < -1 && wc === 0) this.drawSprite('small_walk_l_1', mx, my);
        else if (mv < -1 && (wc === 1 || wc === 3)) this.drawSprite('small_walk_l_2', mx, my);
        else if (mv > 0) this.drawSprite('small_turn_l', mx, my);
        else this.drawSprite('small_stand_l', mx, my);
      }
    } else if (this.mario.life === MarioState.BIG) {
      if (this.marioFacing === 1) {
        if (this.duckMode) this.drawSprite('large_duck_r', mx, my);
        else if (dc === 0) this.drawSprite('large_jump_r', mx, my);
        else if (mv > 1 && wc === 0) this.drawSprite('large_walk_r_1', mx, my);
        else if (mv > 1 && (wc === 1 || wc === 3)) this.drawSprite('large_walk_r_2', mx, my);
        else if (mv < 0) this.drawSprite('large_turn_r', mx, my);
        else this.drawSprite('large_stand_r', mx, my);
      } else {
        if (this.duckMode) this.drawSprite('large_duck_l', mx, my);
        else if (dc === 0) this.drawSprite('large_jump_l', mx, my);
        else if (mv < -1 && wc === 0) this.drawSprite('large_walk_l_1', mx, my);
        else if (mv < -1 && (wc === 1 || wc === 3)) this.drawSprite('large_walk_l_2', mx, my);
        else if (mv > 0) this.drawSprite('large_turn_l', mx, my);
        else this.drawSprite('large_stand_l', mx, my);
      }
    } else if (this.mario.life === MarioState.FIRE) {
      if (this.marioFacing === 1) {
        if (this.duckMode) this.drawSprite('fire_duck_r', mx, my);
        else if (dc === 0) this.drawSprite('fire_jump_r', mx, my);
        else if (mv > 1 && wc === 0) this.drawSprite('fire_walk_r_1', mx, my);
        else if (mv > 1 && (wc === 1 || wc === 3)) this.drawSprite('fire_walk_r_2', mx, my);
        else if (mv < 0) this.drawSprite('fire_turn_r', mx, my);
        else this.drawSprite('fire_stand_r', mx, my);
      } else {
        if (this.duckMode) this.drawSprite('fire_duck_l', mx, my);
        else if (dc === 0) this.drawSprite('fire_jump_l', mx, my);
        else if (mv < -1 && wc === 0) this.drawSprite('fire_walk_l_1', mx, my);
        else if (mv < -1 && (wc === 1 || wc === 3)) this.drawSprite('fire_walk_l_2', mx, my);
        else if (mv > 0) this.drawSprite('fire_turn_l', mx, my);
        else this.drawSprite('fire_stand_l', mx, my);
      }
    }
  }

  private drawSprite(key: string, x: number, y: number): void {
    if (this.spritePoolIndex >= this.spritePool.length) {
      const sp = this.add.image(0, 0, key).setOrigin(0, 0);
      this.spritePool.push(sp);
    }
    const sp = this.spritePool[this.spritePoolIndex++];
    sp.setTexture(key).setPosition(x, y).setVisible(true).setDepth(1).setCrop(0, 0, 9999, 9999);
  }

  private drawSpriteClip(key: string, x: number, y: number, w: number, h: number): void {
    if (this.spritePoolIndex >= this.spritePool.length) {
      const sp = this.add.image(0, 0, key).setOrigin(0, 0);
      this.spritePool.push(sp);
    }
    const sp = this.spritePool[this.spritePoolIndex++];
    const tex = this.textures.get(key);
    const frame = tex.get();
    const fullH = frame.height;
    const clipY = fullH - h;
    sp.setTexture(key).setPosition(x, y).setVisible(true).setDepth(1);
    sp.setCrop(0, clipY, w, h);
  }

  private scoreTexts: Phaser.GameObjects.Text[] = [];
  private scoreTextIndex = 0;

  private drawText(text: string, x: number, y: number): void {
    if (this.scoreTextIndex >= this.scoreTexts.length) {
      const t = this.add.text(0, 0, '', {
        fontSize: '18px', fontFamily: 'Arial', color: '#ffffff',
        stroke: '#000000', strokeThickness: 2,
      }).setDepth(50);
      this.scoreTexts.push(t);
    }
    const t = this.scoreTexts[this.scoreTextIndex++];
    t.setText(text).setPosition(x, y).setVisible(true);
  }

  /** Called at start of renderFrame to also reset score texts */
  private resetScoreTexts(): void {
    for (let i = 0; i < this.scoreTextIndex; i++) {
      this.scoreTexts[i].setVisible(false);
    }
    this.scoreTextIndex = 0;
  }
}
