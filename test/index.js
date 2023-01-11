const main = require('../build/Debug/main.node');
const { Student } = main;
const stu = new Student(11, 22);
stu.age = 333;
console.log(stu, stu.id, stu.age);