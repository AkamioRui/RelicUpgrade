import event from 'node:events';
import util from 'node:util';


/**
 * @template T
 */
class VarPlus{
    #value;
    #event;

    /**
     * @param {T} val 
     */
    constructor(val){
        this.#event = new event();
        this.#value = val;
    }

    /**
     * @returns {T} 
     */
    get(){return this.#value;}

    /**
     * @param {T} value 
     */
    set(value){
        this.#value = value;
        this.#event.emit('modified',value);
    }

    /**
     * 
     * @param {VarPlus} varplus 
     * @param {(value:T)=>{}} fn 
     */
    static depends(varplus,fn) {
        varplus.#event.on('modified',fn);
    } 

    [util.inspect.custom](){
        return String(this.#value);
    }
};

let a = new VarPlus(11);
VarPlus.depends(a,val=>console.log(a));
let b = new VarPlus(11);
VarPlus.depends(b,val=>console.log(b));


console.log('initally(a,b):',a,b);

console.log('change a');
a.set(33);

console.log('add');
VarPlus.depends(a,val=>console.log(a,a));


console.log('change a');
a.set(44);

