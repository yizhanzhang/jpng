const path = require('path');
const binding = require('../binding/index');

const inputPath = path.join(__dirname, "test.png");
const outputPath0 = path.join(__dirname, "test_blank.png");
const outputPath1 = path.join(__dirname, "test_o.png");
const outputPath2 = path.join(__dirname, "test_o.jpeg");

const img = new binding.Image();
img.setPixels(2, 2, [255, 0, 0, 255, 255, 0, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255]);
img.encode(outputPath0);
img.decode(inputPath);
// img.encode(outputPath1);
// img.encode(outputPath2);
