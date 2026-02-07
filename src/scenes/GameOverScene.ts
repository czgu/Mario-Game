import Phaser from 'phaser';
import { SCREEN_WIDTH, SCREEN_HEIGHT } from '../config/Constants.js';
import { GameState } from '../types/index.js';

/**
 * GameOverScene: Displays "GAME OVER!" for ~5.5 seconds.
 * Matches original Display_GamerOver() behavior.
 */
export class GameOverScene extends Phaser.Scene {
  private counter = 0;

  constructor() {
    super({ key: 'GameOverScene' });
  }

  init(data: GameState): void {
    this.counter = 0;

    // Scene data is received here but we just display and go back to menu
  }

  create(data: GameState): void {
    const { score, goldCoin, gameLevel } = data;

    this.cameras.main.setBackgroundColor('#000000');

    // HUD bar
    this.add.text(15, 10, 'Score', { fontSize: '24px', fontFamily: 'Arial', color: '#ffff00' });
    this.add.text(400, 10, 'World', { fontSize: '24px', fontFamily: 'Arial', color: '#ffff00' });
    this.add.text(565, 10, 'Time', { fontSize: '24px', fontFamily: 'Arial', color: '#ffff00' });
    this.add.text(5, 35, String(score).padStart(6, ' '), {
      fontSize: '24px', fontFamily: 'Arial', color: '#ffffff',
    });
    this.add.image(230, 45, 'coin_multi_l').setOrigin(0);
    this.add.text(255, 35, String(goldCoin).padStart(2, ' '), {
      fontSize: '24px', fontFamily: 'Arial', color: '#ffffff',
    });
    this.add.text(420, 35, String(gameLevel).padStart(2, ' '), {
      fontSize: '24px', fontFamily: 'Arial', color: '#ffffff',
    });

    // GAME OVER text
    this.add.text(SCREEN_WIDTH / 2, 195, 'GAME OVER!', {
      fontSize: '30px', fontFamily: 'Arial', color: '#ffff00',
    }).setOrigin(0.5);

    // Play death sfx
    try { this.sound.play('sfx_death'); } catch { /* ok */ }
  }

  update(): void {
    this.counter++;
    if (this.counter > 330) {
      this.scene.start('MenuScene');
    }
  }
}
