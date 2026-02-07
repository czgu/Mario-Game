import { LevelDesign } from '../types/index.js';

/** Hard-coded level design data matching the original C++ code */
export const LEVEL_DESIGNS: Record<number, LevelDesign> = {
  1: {
    flagPosition: 5895,
    castlePosition: 6080,
    ladderPosition: 5400,
    backgroundType: 0,
    heightSpawn: 0,
    numberSpawn: 15,
    numberGoldSpawn: 35,
  },
  2: {
    flagPosition: 6885,
    castlePosition: 7050,
    ladderPosition: 6400,
    backgroundType: 0,
    heightSpawn: 0,
    numberSpawn: 20,
    numberGoldSpawn: 35,
  },
  3: {
    flagPosition: 5635,
    castlePosition: 5815,
    ladderPosition: 4960,
    backgroundType: 1,
    heightSpawn: 100,
    numberSpawn: 25,
    numberGoldSpawn: 90,
  },
  4: {
    flagPosition: 7428,
    castlePosition: 7616,
    ladderPosition: 6800,
    backgroundType: 0,
    heightSpawn: 0,
    numberSpawn: 25,
    numberGoldSpawn: 75,
  },
  5: {
    flagPosition: 6885,
    castlePosition: 7050,
    ladderPosition: 6400,
    backgroundType: 0,
    heightSpawn: 0,
    numberSpawn: 20,
    numberGoldSpawn: 35,
  },
  6: {
    flagPosition: 20000,
    castlePosition: 20000,
    ladderPosition: 4512,
    backgroundType: 1,
    heightSpawn: 130,
    numberSpawn: 20,
    numberGoldSpawn: 100,
  },
};
