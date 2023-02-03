const main = require('../build/Debug/main.node');

console.log(main);
const img = new main.Image();
console.log(img, img.convert("yizhan.txt", "yizhan.txt"));
// console.log(main.getMoreDetail());
// const { Student } = main;
// const stu = new Student(11, 22, "yizhan");
// console.log(stu, stu.id, stu.age, stu.name);