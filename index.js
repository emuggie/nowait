const bindings = require('bindings');

const a = bindings('nowait');
let t= new a.Nowait();

let p =new Promise((resolve,reject)=>{
    console.log("np")
    t.unwatch();
    resolve("e")
}).then(rslt=>{
    console.log("next")
    t.unwatch();
}).catch(err=>{
    console.log("err")
    t.unwatch();
});
// Promise.race(p);
// setTimeout(()=>{
//     console.log('1')
//     // while(true);
// },2000);

setTimeout(()=>{
    t.unwatch();
    console.log('2')
},3000);
t.unwatch();
console.log('1')

// setTimeout(()=>{
//     console.log('4')
//     while(true);
// },4000);

// setTimeout(()=>{
//     console.log('5')
//     while(true);
// },5000);


