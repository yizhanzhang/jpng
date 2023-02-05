const main = require('../build/Debug/main.node');

console.log(main);
const img = new main.Image();
img.convert("yizhan.png", "yizhan_o.jpeg");