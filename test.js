const bindings = require('bindings');

const a = bindings('nowait');
// while(i++ < 3){
    //     console.log("test")
    //     t.unwatch();
    // };
// let Manager= new a.Nowait;
let manager= new a.Nowait();
function wathcer(func,callback){
    let resolved = false
    return new Promise(r=>{
        r();
    }).then(()=>{
        setImmediate(()=>{
            console.log("later")
            if(!resolved){
                callback("error")
            }
        })
        console.log("2")
        manager.watch(1000, ()=>{});
        console.log("3")
        let result = func();
        console.log("4")
        manager.unwatch();
        console.log("5")
        resolved = true;
        callback(result);
    });
}

wathcer(()=>{
    console.log("infinite")
    while(true);
},(rslt)=>{
    console.log(rslt);
})

wathcer(()=>{

}, 1000);