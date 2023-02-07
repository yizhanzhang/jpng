const path = require('path');
const fs = require('fs');
const main = require('../build/Debug/main.node');

const originImage = main.Image;

class Image {
  constructor() {
    this.__image = new originImage();
  }
  setPixels(width, height, data) {
    this.__image.setPixels(width, height, data);
  }
  decode(inputPath) {
    const inputType = path.extname(inputPath).slice(1);
    const inputBuffer = fs.readFileSync(inputPath);
    this.__image.decode(inputType, inputBuffer);
  }
  encode(outputPath) {
    const outputType = path.extname(outputPath).slice(1);
    const output = this.__image.encode(outputType);
    fs.writeFileSync(outputPath, output);
  }
}

main.Image = Image;

module.exports = main;