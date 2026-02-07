import Phaser from 'phaser';
import { SCREEN_WIDTH, SCREEN_HEIGHT } from '../config/Constants.js';
import { GameState } from '../types/index.js';

/**
 * LevelInfoScene: Shows "WORLD X" + lives before a level starts.
 * Matches original Display_Info() - displays for 2 seconds then transitions.
 */
export class LevelInfoScene extends Phaser.Scene {
  private counter = 0;
  private state!: GameState;

  constructor() {
    super({ key: 'LevelInfoScene' });
  }

  init(data: GameState): void {
    this.state = data;
    this.counter = 0;
  }

  create(): void {
    const { lives, score, goldCoin, gameLevel } = this.state;
    const cx = SCREEN_WIDTH / 2;

    // Black background
    this.cameras.main.setBackgroundColor('#000000');

    // "WORLD X" text
    this.add.text(cx, 195, `WORLD  ${gameLevel}`, {
      fontSize: '30px', fontFamily: 'Arial', color: '#ffffff',
    }).setOrigin(0.5);

    // Mario icon + x lives
    this.add.image(275, 252, 'mario_icon').setOrigin(0.5);
    this.add.text(320, 240, `X   ${lives}`, {
      fontSize: '30px', fontFamily: 'Arial', color: '#ffffff',
    });

    // HUD bar
    this.add.text(15, 10, 'Score', { fontSize: '24px', fontFamily: 'Arial', color: '#ffff00' });
    this.add.text(400, 10, 'World', { fontSize: '24px', fontFamily: 'Arial', color: '#ffff00' });
    this.add.text(565, 10, 'Time', { fontSize: '24px', fontFamily: 'Arial', color: '#ffff00' });
    this.add.text(10, 35, String(score).padStart(6, ' '), {
      fontSize: '24px', fontFamily: 'Arial', color: '#ffffff',
    });
    this.add.image(230, 45, 'coin_multi_l').setOrigin(0);
    this.add.text(255, 35, String(goldCoin).padStart(2, ' '), {
      fontSize: '24px', fontFamily: 'Arial', color: '#ffffff',
    });
    this.add.text(420, 35, String(gameLevel).padStart(2, ' '), {
      fontSize: '24px', fontFamily: 'Arial', color: '#ffffff',
    });
  }

  update(): void {
    this.counter++;
    // Display for 2 seconds (120 frames at 60fps)
    if (this.counter > 120) {
      this.scene.start('GameScene', this.state);
    }
  }
}
