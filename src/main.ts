import Phaser from 'phaser';
import { BootScene } from './scenes/BootScene.js';
import { MenuScene } from './scenes/MenuScene.js';
import { GameScene } from './scenes/GameScene.js';
import { LevelInfoScene } from './scenes/LevelInfoScene.js';
import { PauseScene } from './scenes/PauseScene.js';
import { GameOverScene } from './scenes/GameOverScene.js';
import { SCREEN_WIDTH, SCREEN_HEIGHT } from './config/Constants.js';

const config: Phaser.Types.Core.GameConfig = {
  type: Phaser.AUTO,
  width: SCREEN_WIDTH,
  height: SCREEN_HEIGHT,
  backgroundColor: '#000000',
  parent: document.body,
  pixelArt: true,
  physics: {
    default: 'arcade',
    arcade: {
      gravity: { x: 0, y: 0 },
      debug: false,
    },
  },
  scene: [BootScene, MenuScene, LevelInfoScene, GameScene, PauseScene, GameOverScene],
  scale: {
    mode: Phaser.Scale.FIT,
    autoCenter: Phaser.Scale.CENTER_BOTH,
  },
  input: {
    keyboard: true,
  },
};

new Phaser.Game(config);
