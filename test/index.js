const path = require('path');
const binding = require('../binding/index');

const inputPath = path.join(__dirname, "test.png");
const outputPath = path.join(__dirname, "test_o.png");

console.log(binding);
const img = new binding.Image();
img.decode(inputPath);
img.encode(outputPath);
