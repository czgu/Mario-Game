import Phaser from 'phaser';
import { SCREEN_WIDTH, SCREEN_HEIGHT } from '../config/Constants.js';

/**
 * MenuScene: Title screen with scrolling background, options.
 * Matches original Menu_Screen() behavior.
 */
export class MenuScene extends Phaser.Scene {
  private scrollBg!: Phaser.GameObjects.TileSprite;
  private title!: Phaser.GameObjects.Image;
  private enterPrompt!: Phaser.GameObjects.Image;
  private optionMenu!: Phaser.GameObjects.Image;
  private selectIcon!: Phaser.GameObjects.Image;
  private instructionPic!: Phaser.GameObjects.Image;
  private border!: Phaser.GameObjects.Image;

  private selected = 1;
  private started = false;
  private hitTwice = false;
  private showInstruction = false;
  private cdCounter = 30;
  private startCounter = 0;
  private titleY = -167;
  private enterY = 260;
  private enterSpeed = 1;
  private secondCounter = 0;
  private scrollX = 0;
  private scrollDir = 0;

  private keys!: {
    up: Phaser.Input.Keyboard.Key;
    down: Phaser.Input.Keyboard.Key;
    enter: Phaser.Input.Keyboard.Key;
  };
  private prevUp = false;
  private prevDown = false;
  private titleMusic?: Phaser.Sound.BaseSound;

  constructor() {
    super({ key: 'MenuScene' });
  }

  create(): void {
    // Reset state
    this.selected = 1;
    this.started = false;
    this.hitTwice = false;
    this.showInstruction = false;
    this.cdCounter = 30;
    this.startCounter = 0;
    this.titleY = -167;
    this.enterY = 260;
    this.enterSpeed = 1;
    this.secondCounter = 0;
    this.scrollX = 0;
    this.scrollDir = 0;
    this.prevUp = false;
    this.prevDown = false;

    // Background scroll
    this.scrollBg = this.add.tileSprite(
      SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2,
      SCREEN_WIDTH, SCREEN_HEIGHT,
      'menu_scroll'
    );

    // Title
    this.title = this.add.image(55 + 272, this.titleY + 83, 'menu_title').setOrigin(0.5);

    // Enter prompt
    this.enterPrompt = this.add.image(220 + 115, this.enterY + 57, 'menu_enter').setOrigin(0.5);

    // Option menu (hidden initially)
    this.optionMenu = this.add.image(220 + 115, 240 + 57, 'menu_option').setOrigin(0.5).setVisible(false);

    // Selection mushroom
    this.selectIcon = this.add.image(213, 258, 'menu_select').setOrigin(0.5).setVisible(false);

    // Border overlay
    this.border = this.add.image(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 'menu_bg').setOrigin(0.5);

    // Instruction
    this.instructionPic = this.add.image(180 + 160, 220 + 95, 'menu_instruction')
      .setOrigin(0.5).setVisible(false);

    // Input
    const kb = this.input.keyboard!;
    this.keys = {
      up: kb.addKey(Phaser.Input.Keyboard.KeyCodes.UP),
      down: kb.addKey(Phaser.Input.Keyboard.KeyCodes.DOWN),
      enter: kb.addKey(Phaser.Input.Keyboard.KeyCodes.ENTER),
    };

    // Play title music
    try {
      this.titleMusic = this.sound.add('title_theme', { loop: true });
      this.titleMusic.play();
    } catch { /* audio may fail silently */ }
  }

  update(_time: number, _delta: number): void {
    // Update counters
    this.secondCounter++;
    if (this.secondCounter > 60) this.secondCounter = 0;
    if (this.cdCounter < 30) this.cdCounter++;

    // Enter key handling
    if (Phaser.Input.Keyboard.JustDown(this.keys.enter)) {
      if (!this.started) {
        this.started = true;
        this.startCounter = 0;
      } else if (this.hitTwice && this.cdCounter >= 30) {
        this.cdCounter = 0;
        if (this.selected === 1) {
          // Instructions toggle
          this.showInstruction = !this.showInstruction;
        } else if (this.selected === 2) {
          // Start game
          this.titleMusic?.stop();
          this.scene.start('LevelInfoScene', {
            score: 0, goldCoin: 0, lives: 3,
            gameLevel: 1, marioLife: 1,
          });
          return;
        } else if (this.selected === 3) {
          // Quit - go back to title
          this.titleMusic?.stop();
          this.scene.restart();
          return;
        }
      }
    }

    // Start counter -> hitTwice
    if (this.started && !this.hitTwice) {
      this.startCounter++;
      if (this.startCounter > 20) {
        this.hitTwice = true;
      }
    }

    // Enter key bounce animation
    if (this.secondCounter % 4 === 0) {
      this.enterY += this.enterSpeed;
    }
    if (this.enterY > 270) this.enterSpeed = -1;
    else if (this.enterY < 250) this.enterSpeed = 1;

    // Background scroll
    if (this.scrollDir === 0) this.scrollX -= 1;
    else this.scrollX += 1;
    if (this.scrollX < -4000) this.scrollDir = 1;
    else if (this.scrollX >= 0) this.scrollDir = 0;
    this.scrollBg.tilePositionX = -this.scrollX;

    // Title drop animation
    if (this.titleY < 45) this.titleY += 2;
    this.title.y = this.titleY + 83;

    // Menu selection
    if (this.hitTwice && !this.showInstruction) {
      const upDown = this.keys.up.isDown;
      const downDown = this.keys.down.isDown;

      if (upDown && !this.prevUp) {
        this.selected--;
        if (this.selected < 1) this.selected = 3;
      }
      if (downDown && !this.prevDown) {
        this.selected++;
        if (this.selected > 3) this.selected = 1;
      }
      this.prevUp = upDown;
      this.prevDown = downDown;
    }

    // Update visibility
    this.enterPrompt.setVisible(!this.started);
    this.enterPrompt.y = this.enterY + 57;

    this.optionMenu.setVisible(this.started);
    this.selectIcon.setVisible(this.hitTwice);

    if (this.hitTwice) {
      const selectY = this.selected === 1 ? 258 : this.selected === 2 ? 298 : 333;
      this.selectIcon.setPosition(213, selectY);
    }

    this.instructionPic.setVisible(this.showInstruction);
  }
}
