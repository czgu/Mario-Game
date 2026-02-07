import Phaser from 'phaser';

/** BootScene: loads all game assets before transitioning to MenuScene */
export class BootScene extends Phaser.Scene {
  constructor() {
    super({ key: 'BootScene' });
  }

  preload(): void {
    // Progress bar
    const w = this.cameras.main.width;
    const h = this.cameras.main.height;
    const progressBar = this.add.graphics();
    const progressBox = this.add.graphics();
    progressBox.fillStyle(0x222222, 0.8);
    progressBox.fillRect(w / 4, h / 2 - 15, w / 2, 30);

    this.load.on('progress', (value: number) => {
      progressBar.clear();
      progressBar.fillStyle(0xffffff, 1);
      progressBar.fillRect(w / 4 + 5, h / 2 - 10, (w / 2 - 10) * value, 20);
    });

    this.load.on('complete', () => {
      progressBar.destroy();
      progressBox.destroy();
    });

    const img = 'assets/img/';
    const mus = 'assets/music/';

    // ── Mario sprites ─────────────────────────────────────────
    // Fire Mario
    this.load.image('fire_stand_r', img + 'FIRE mario R 1.png');
    this.load.image('fire_walk_r_1', img + 'FIRE mario R 2.png');
    this.load.image('fire_walk_r_2', img + 'FIRE mario R 3.png');
    this.load.image('fire_jump_r', img + 'FIRE mario R 4.png');
    this.load.image('fire_turn_r', img + 'FIRE mario L 5.png');
    this.load.image('fire_stand_l', img + 'FIRE mario L 1.png');
    this.load.image('fire_walk_l_1', img + 'FIRE mario L 2.png');
    this.load.image('fire_walk_l_2', img + 'FIRE mario L 3.png');
    this.load.image('fire_jump_l', img + 'FIRE mario L 4.png');
    this.load.image('fire_turn_l', img + 'FIRE mario R 5.png');
    this.load.image('fire_duck_l', img + 'FIRE mario L 6.png');
    this.load.image('fire_duck_r', img + 'FIRE mario R 6.png');
    // Large Mario
    this.load.image('large_stand_r', img + 'L Mario R 1.png');
    this.load.image('large_walk_r_1', img + 'L Mario R 2.png');
    this.load.image('large_walk_r_2', img + 'L Mario R 4.png');
    this.load.image('large_jump_r', img + 'L Mario R 3.png');
    this.load.image('large_turn_r', img + 'L Mario L 5.png');
    this.load.image('large_stand_l', img + 'L Mario L 1.png');
    this.load.image('large_walk_l_1', img + 'L Mario L 2.png');
    this.load.image('large_walk_l_2', img + 'L Mario L 4.png');
    this.load.image('large_jump_l', img + 'L Mario L 3.png');
    this.load.image('large_turn_l', img + 'L Mario R 5.png');
    this.load.image('large_duck_l', img + 'L Mario L 6.png');
    this.load.image('large_duck_r', img + 'L Mario R 6.png');
    // Small Mario
    this.load.image('small_stand_r', img + 'SMALL Mario R 1.png');
    this.load.image('small_walk_r_1', img + 'SMALL Mario R 2.png');
    this.load.image('small_walk_r_2', img + 'SMALL Mario R 4.png');
    this.load.image('small_jump_r', img + 'SMALL Mario R 3.png');
    this.load.image('small_turn_r', img + 'SMALL Mario L 5.png');
    this.load.image('small_stand_l', img + 'SMALL Mario L 1.png');
    this.load.image('small_walk_l_1', img + 'SMALL Mario L 2.png');
    this.load.image('small_walk_l_2', img + 'SMALL Mario L 4.png');
    this.load.image('small_jump_l', img + 'SMALL Mario L 3.png');
    this.load.image('small_turn_l', img + 'SMALL Mario R 5.png');
    // Dead / flash
    this.load.image('dead_mario', img + 'Dead Mario.png');
    this.load.image('flash_mario', img + 'Mario_Flash.png');
    this.load.image('mario_icon', img + 'Mario.png');

    // ── Enemies ───────────────────────────────────────────────
    this.load.image('goomba_l', img + 'Goomba L.png');
    this.load.image('goomba_r', img + 'Goomba R.png');
    this.load.image('turtle_l1', img + 'Turtle L 1.png');
    this.load.image('turtle_l2', img + 'Turtle L 2.png');
    this.load.image('turtle_r1', img + 'Turtle R 1.png');
    this.load.image('turtle_r2', img + 'Turtle R 2.png');
    this.load.image('turtle_shell', img + 'Dead turtle.png');
    this.load.image('bowser_walk1', img + 'Bowers 1.png');
    this.load.image('bowser_walk2', img + 'Bowers 3.png');
    this.load.image('bowser_jump', img + 'Bowers 2.png');
    this.load.image('bowser_fall', img + 'Bowers 4.png');
    this.load.image('cannon_bullet', img + 'Bullet.png');
    this.load.image('button', img + 'Button.png');
    this.load.image('sky_fire', img + 'Sky Ball.png');
    this.load.image('princess', img + 'Princess.png');

    // ── Power-ups ─────────────────────────────────────────────
    this.load.image('mushroom', img + 'Shroom.png');
    this.load.image('flower1', img + 'Flower 1.png');
    this.load.image('flower2', img + 'Flower 2.png');
    this.load.image('coin_d', img + 'Coin D.png');
    this.load.image('coin_l', img + 'Coin L.png');
    this.load.image('coin_m', img + 'Coin M.png');
    this.load.image('coin_multi_l', img + 'Multiplier L.png');
    this.load.image('coin_multi_d', img + 'Multiplier D.png');

    // ── Fireballs ─────────────────────────────────────────────
    this.load.image('fireball1', img + 'Fireball 1.png');
    this.load.image('fireball2', img + 'Fireball 2.png');
    this.load.image('fireball3', img + 'Fireball 3.png');
    this.load.image('fireball4', img + 'Fireball 4.png');

    // ── Animations ────────────────────────────────────────────
    this.load.image('box_coin', img + 'Brick_Coin.png');
    this.load.image('broken_brick', img + 'Broken Brick.png');
    this.load.image('blue_broken_brick', img + 'Blue Broken Brick.png');
    this.load.image('goomba_squash', img + 'Squash.png');
    this.load.image('goomba_dead', img + 'Dead_Goomba.png');
    this.load.image('turtle_dead', img + 'Dead_Turtle.png');
    this.load.image('explosion', img + 'Explosion.png');
    this.load.image('one_up', img + '1UP.png');
    this.load.image('end_message', img + 'End Message.png');
    this.load.image('big_explosion', img + 'Big explosion.png');

    // ── Flag ──────────────────────────────────────────────────
    this.load.image('flag_pole', img + 'Whole flag pole.png');
    this.load.image('flag', img + 'Flag.png');

    // ── Tiles ─────────────────────────────────────────────────
    this.load.image('brick', img + 'Brick new.png');
    this.load.image('night_brick', img + 'Blue_Brick.png');
    this.load.image('brick_up', img + 'Up Brick.png');
    this.load.image('tube_ext_left', img + 'Tube_Ext_Left.png');
    this.load.image('tube_ext_right', img + 'Tube_Ext_Right.png');
    this.load.image('tube_top_left', img + 'Tube_Top_Left.png');
    this.load.image('tube_top_right', img + 'Tube_Top_Right.png');
    this.load.image('block', img + 'Block new.png');
    this.load.image('night_block', img + 'N Block.png');
    this.load.image('box_d', img + 'Unknown Box D.png');
    this.load.image('box_l', img + 'Unknown Box L.png');
    this.load.image('box_m', img + 'Unknown Box M.png');
    this.load.image('box_hit', img + 'Box_Hit.png');
    this.load.image('pole', img + 'Pole.png');
    this.load.image('pole_head', img + 'Pole Head.png');
    this.load.image('small_castle', img + 'Small_Castle.png');

    // Ground tiles per level
    for (const [lvl, prefix] of [
      [1, 'Grass'], [2, 'Ice top'], [3, 'Underground top'],
      [4, 'Dessert top'], [5, 'Mountain top'], [6, 'Castle Top'],
    ] as [number, string][]) {
      if (lvl === 1) {
        this.load.image(`out_ground_${lvl}_0`, img + 'Grass 1.png');
        this.load.image(`out_ground_${lvl}_1`, img + 'Grass 2.png');
        this.load.image(`out_ground_${lvl}_2`, img + 'Grass 3.png');
      } else if (lvl === 2) {
        this.load.image(`out_ground_${lvl}_0`, img + 'Ice top 2.png');
        this.load.image(`out_ground_${lvl}_1`, img + 'Ice top 1.png');
        this.load.image(`out_ground_${lvl}_2`, img + 'Ice top 2.png');
      } else if (lvl === 3) {
        this.load.image(`out_ground_${lvl}_0`, img + 'Underground top 1.png');
        this.load.image(`out_ground_${lvl}_1`, img + 'Underground top 2.png');
        this.load.image(`out_ground_${lvl}_2`, img + 'Underground top 3.png');
      } else if (lvl === 4) {
        this.load.image(`out_ground_${lvl}_0`, img + 'Dessert top1.png');
        this.load.image(`out_ground_${lvl}_1`, img + 'Dessert top2.png');
        this.load.image(`out_ground_${lvl}_2`, img + 'Dessert top1.png');
      } else if (lvl === 5) {
        this.load.image(`out_ground_${lvl}_0`, img + 'Mountain top 1.png');
        this.load.image(`out_ground_${lvl}_1`, img + 'Mountain top 2.png');
        this.load.image(`out_ground_${lvl}_2`, img + 'Mountain top 1.png');
      } else if (lvl === 6) {
        this.load.image(`out_ground_${lvl}_0`, img + 'Castle Top 1.png');
        this.load.image(`out_ground_${lvl}_1`, img + 'Castle Top 2.png');
        this.load.image(`out_ground_${lvl}_2`, img + 'Castle Top 3.png');
      }
    }

    // Inner ground tiles
    this.load.image('in_ground_1_0', img + 'Soil 1.png');
    this.load.image('in_ground_1_1', img + 'Soil 2.png');
    this.load.image('in_ground_1_2', img + 'Soil 3.png');
    this.load.image('in_ground_1_3', img + 'Soil 4.png');
    this.load.image('in_ground_1_4', img + 'Soil 5.png');

    this.load.image('in_ground_2_0', img + 'Ice bottom 2.png');
    this.load.image('in_ground_2_1', img + 'Ice bottom 1.png');
    this.load.image('in_ground_2_2', img + 'Ice bottom 2.png');
    this.load.image('in_ground_2_3', img + 'Ice bottom 1.png');
    this.load.image('in_ground_2_4', img + 'Ice bottom 2.png');

    this.load.image('in_ground_3_0', img + 'Underground bottom 1.png');
    this.load.image('in_ground_3_1', img + 'Underground bottom 2.png');
    this.load.image('in_ground_3_2', img + 'Underground bottom 3.png');
    this.load.image('in_ground_3_3', img + 'Underground bottom 4.png');
    this.load.image('in_ground_3_4', img + 'Underground bottom 1.png');

    this.load.image('in_ground_4_0', img + 'Dessert sand1.png');
    this.load.image('in_ground_4_1', img + 'Dessert sand2.png');
    this.load.image('in_ground_4_2', img + 'Dessert sand1.png');
    this.load.image('in_ground_4_3', img + 'Dessert sand2.png');
    this.load.image('in_ground_4_4', img + 'Dessert sand1.png');

    this.load.image('in_ground_5_0', img + 'Mountain bottom 1.png');
    this.load.image('in_ground_5_1', img + 'Mountain bottom 2.png');
    this.load.image('in_ground_5_2', img + 'Mountain bottom 3.png');
    this.load.image('in_ground_5_3', img + 'Mountain bottom 4.png');
    this.load.image('in_ground_5_4', img + 'Mountain bottom 2.png');

    this.load.image('in_ground_6_0', img + 'Castle edge.png');
    this.load.image('in_ground_6_1', img + 'Castle edge.png');
    this.load.image('in_ground_6_2', img + 'Castle edge.png');
    this.load.image('in_ground_6_3', img + 'Castle edge.png');
    this.load.image('in_ground_6_4', img + 'Castle edge.png');

    // ── Backgrounds ───────────────────────────────────────────
    this.load.image('panorama_1', img + 'Clouds.png');
    this.load.image('panorama_bg_1', img + 'Background.png');
    this.load.image('panorama_2', img + 'Ice foreground.png');
    this.load.image('panorama_bg_2', img + 'Ice Background.png');
    this.load.image('panorama_3', img + 'Underground ForeGround.png');
    this.load.image('panorama_bg_3', img + 'N background.png');
    this.load.image('panorama_4', img + 'Stars.png');
    this.load.image('panorama_bg_4', img + 'Sand Background.png');
    this.load.image('panorama_5', img + 'Dense Clouds.png');
    this.load.image('panorama_bg_5', img + 'Mountains.png');
    this.load.image('panorama_6', img + 'Castle Foreground.png');
    this.load.image('panorama_bg_6', img + 'Star Background.png');

    // ── Menu images ───────────────────────────────────────────
    this.load.image('menu_bg', img + 'Main Menu.png');
    this.load.image('menu_scroll', img + 'Main Menu scroll.png');
    this.load.image('menu_title', img + 'MenuTitle.png');
    this.load.image('menu_enter', img + 'Main Menu Enter.png');
    this.load.image('menu_option', img + 'MenuOption.png');
    this.load.image('menu_select', img + 'Selected.png');
    this.load.image('menu_instruction', img + 'Instruction Menu.png');
    this.load.image('pause_menu', img + 'Pause Menu.png');

    // ── Audio (WAV only for browser) ──────────────────────────
    this.load.audio('sfx_cant_brick', mus + "Can't Brick.wav");
    this.load.audio('sfx_coin', mus + 'coin.wav');
    this.load.audio('sfx_shell_kick', mus + 'Shell Kick.wav');
    this.load.audio('sfx_enemy_stomp', mus + 'Enemy Stomp.wav');
    this.load.audio('sfx_jump1', mus + 'Mario Jump 1.wav');
    this.load.audio('sfx_jump2', mus + 'Mario Jump 2.wav');
    this.load.audio('sfx_powerup', mus + 'Power Up.wav');
    this.load.audio('sfx_hurt', mus + 'Mario Hurt.wav');
    this.load.audio('sfx_mushroom', mus + 'Mushroom Sound.wav');
    this.load.audio('sfx_brick_break', mus + 'Brick Break.wav');
    this.load.audio('sfx_fireball', mus + 'FireballThrow.wav');
    this.load.audio('sfx_1up', mus + '1UP.wav');
    this.load.audio('sfx_pause', mus + 'Pause Sound Effect.wav');
    this.load.audio('sfx_death', mus + 'Death.wav');
    this.load.audio('title_theme', mus + 'Title Theme.wav');
    this.load.audio('level1_music', mus + 'Level 1.wav');

    // ── Level data ────────────────────────────────────────────
    for (let i = 1; i <= 6; i++) {
      this.load.text(`level_${i}`, `assets/data/Level ${i}.txt`);
    }
  }

  create(): void {
    this.scene.start('MenuScene');
  }
}
