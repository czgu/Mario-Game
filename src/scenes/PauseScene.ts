import Phaser from 'phaser';

/**
 * PauseScene: Overlay shown when game is paused.
 * Matches original Paused_Screen() behavior.
 */
export class PauseScene extends Phaser.Scene {
  private selected = 1;
  private showInstruction = false;
  private cdCounter = 0;
  private prevUp = false;
  private prevDown = false;
  private selectIcon!: Phaser.GameObjects.Image;
  private instructionPic!: Phaser.GameObjects.Image;

  private keys!: {
    up: Phaser.Input.Keyboard.Key;
    down: Phaser.Input.Keyboard.Key;
    enter: Phaser.Input.Keyboard.Key;
  };

  constructor() {
    super({ key: 'PauseScene' });
  }

  create(): void {
    this.selected = 1;
    this.showInstruction = false;
    this.cdCounter = 0;
    this.prevUp = false;
    this.prevDown = false;

    // Semi-transparent overlay
    this.add.rectangle(320, 240, 640, 480, 0x000000, 0.5);

    // Pause menu background
    this.add.image(320, 195, 'pause_menu').setOrigin(0.5);

    // Selection mushroom
    this.selectIcon = this.add.image(210, 160 + 65, 'menu_select').setOrigin(0.5);

    // Instruction overlay
    this.instructionPic = this.add.image(320, 195, 'menu_instruction')
      .setOrigin(0.5).setVisible(false);

    const kb = this.input.keyboard!;
    this.keys = {
      up: kb.addKey(Phaser.Input.Keyboard.KeyCodes.UP),
      down: kb.addKey(Phaser.Input.Keyboard.KeyCodes.DOWN),
      enter: kb.addKey(Phaser.Input.Keyboard.KeyCodes.ENTER),
    };

    // Play pause sound
    try { this.sound.play('sfx_pause'); } catch { /* ok */ }
  }

  update(): void {
    if (this.cdCounter < 30) this.cdCounter++;

    if (Phaser.Input.Keyboard.JustDown(this.keys.enter) && this.cdCounter >= 30) {
      this.cdCounter = 0;
      if (this.selected === 1) {
        // Resume
        try { this.sound.play('sfx_pause'); } catch { /* ok */ }
        this.scene.stop();
        this.scene.resume('GameScene');
        return;
      } else if (this.selected === 2) {
        // Instructions
        this.showInstruction = !this.showInstruction;
      } else if (this.selected === 3) {
        // Quit to menu
        this.scene.stop('GameScene');
        this.scene.stop();
        this.scene.start('MenuScene');
        return;
      }
    }

    if (!this.showInstruction) {
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

    const sy = this.selected === 1 ? 165 : this.selected === 2 ? 208 : 249;
    this.selectIcon.setPosition(210, sy);
    this.instructionPic.setVisible(this.showInstruction);
  }
}
