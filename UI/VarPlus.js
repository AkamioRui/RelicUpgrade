
/**
 * @template T
 */
export default class VarPlus{
    #value;
    #event;

    /**
     * @param {T} val 
     */
    constructor(val){
        this.#event = [];
        this.#value = val;
    }

    /**
     * @returns {T} 
     */
    get(){
        return this.#value;
    }

    /**
     * @param {T} value 
     */
    set(value){
        this.#value = value;
        this.#event.forEach(fn=>fn(value));
    }

  

    /**
     * 
     * @param {(value:T)=>{}} fn 
     * @param {boolean} immediateFire 
     */
    modify(fn,immediateFire = true){
        this.#event.push(fn);
        if(immediateFire)fn(this.#value);
    }


    
    toString(){
        return String(this.#value);
    }
};


 