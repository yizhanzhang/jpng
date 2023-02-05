const fs = require('fs');
const main = require('../build/Debug/main.node');

console.log(main);
const img = new main.Image();
const inputBuffer = fs.readFileSync("yizhan.png");
console.log(inputBuffer);
// img.convert("yizhan.png", "yizhan_o.jpeg");