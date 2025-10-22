
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

