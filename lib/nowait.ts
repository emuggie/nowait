import bindings from 'bindings';
const Manager = bindings('nowait').Nowait;

function Nowait(runnable:()=>any):Promise<any>{
    let tries = new Promise(resolve=>{
        resolve();
    }).then(()=>{
        return new Promise((resolve,reject)=>{
            const manager = new Manager();
            manager.watch(1000, reject);
            let result = runnable();
            manager.unwatch();
            resolve(result);
        });
        
    });

    return tries;
}

export { Nowait };