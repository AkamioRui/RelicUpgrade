import VarPlus  from './VarPlus.js'
import * as relic from './script.js'
import * as d3_raw from 'https://cdn.jsdelivr.net/npm/d3@7/+esm';
/** @type {import( "d3" )} */
let d3 = d3_raw;



// const event = require('node:events');
    // const emitter = new EventTarget();
    // emitter.addEventListener('custom', e => console.log(e.detail));
    // emitter.dispatchEvent(new CustomEvent('custom', { detail: 42 }));
// thresholdOpt.on('input',function(){
//     d3.select('#thresholdMeter > div').text(this.value);
    
//     //update global var
//     thresholdCode = this.value;
//     codeElement.text(getCode());
// });

//testing
//#region

//#endregion



//constant
const code_piece = {
    '0': 'HEAD'  ,
    '1': 'HANDS' ,
    '2': 'BODY'  ,
    '3': 'BOOTS' ,
    '4': 'PLANAR',
    '5': 'ROPE'  
};
const piece_mainstat = {
    HEAD   :['HP'],    
    HANDS  :['ATK'],    
    BODY   :[
      'HP%',
      'ATK%',
      'DEF%',
      'CD',
      'CR',
      'EHR',
      'HEAL'
    ],    
    BOOTS  :[
      'HP%',
      'ATK%',
      'DEF%',
      'SPD'
    ],    
    PLANAR :[
      'HP%',
      'ATK%',
      'DEF%',
      'DMG'
    ],    
    ROPE   :[
      'HP%',
      'ATK%',
      'DEF%',
      'BE',
      'ER'
    ],      
};
const code_stat = {
    '0'  :'HP'      ,
    '1'  :'ATK'     ,
    '2'  :'DEF'     ,
    '3'  :'HP%'     ,
    '4'  :'ATK%'    ,
    '5'  :'DEF%'    ,
    '6'  :'SPD'     ,
    '7'  :'CR'      ,
    '8'  :'CD'      ,
    '9'  :'EHR'     ,
    '10' :'ERS'     ,
    '11' :'BE'      ,
    '-1' :'DMG'     ,
    '-2' :'HEAL'    ,
    '-3' :'ER'      ,
};
const keyword_code = {
'HP'     : 0,
'ATK'    : 1,
'DEF'    : 2,
'HP%'    : 3,
'ATK%'   : 4,
'DEF%'   : 5,
'SPD'    : 6,
'CR'     : 7,
'CD'     : 8,
'EHR'    : 9,
'ERS'    : 10,
'BE'     : 11,
'DMG'    : -1,
'HEAL'   : -2,
'ER'     : -3,

'HEAD'  :0,
'HANDS' :1,
'BODY'  :2,
'BOOTS' :3,
'PLANAR':4,
'ROPE'  :5
};


// option element
const _menu = d3.select('#menu');
const _option = _menu.append('div').attr('id','option');
const _piece    = create_piece(_option);
const _mainstat = create_mainstat(_option,_piece);
const _substat  = create_substat(_option,_mainstat);

//result elment

const _result = _menu.append('div').attr('id','result');
const _clacButton = _result.append('button')
    .text('calculate')    
;
// const cost = 53400;
const cost = 264000;
const _summary = _result.append('div').attr('id','summary');
const _summary_header = _summary.append('div');
    _summary_header.append('div').text(d=>`Threshold`)    
    _summary_header.append('div').text(d=>`Cost(${cost})`)    
;
/** @type {d3.Selection<BaseType, {threshold: number;    cost: number;}[], HTMLDivElement, any>} */
let _summary_list = _summary.selectAll('.summaryNode');
//TODO summary always exist, initially with only header

//option logic
//#region 
/**
 * @param {d3.Selection<HTMLDivElement, any, HTMLElement, any> } _parent
 * @returns {d3.Selection<HTMLDivElement, string, HTMLElement, any> & {chosen: VarPlus<any>}}
 */
function create_piece(_parent){
    //constant
    const optionList = Object.values(code_piece);
    const thisName = 'piece';
    const initVal = '_';
    

    //element
    /** @type {d3.Selection<HTMLDivElement, string, HTMLElement, any> & {chosen: VarPlus<any>}} */
    const _piece = _parent.append('div').attr('id',thisName);

    const _tbHead = _piece.append('div').classed('tbHead',true);
    const _tbBody = _piece.append('div').classed('tbBody',true);

    const _title = _tbHead.append('div').text(thisName.toLocaleUpperCase());
    const _tbOpt_list  = _tbBody.selectAll('.tbOpt').data(optionList).enter()
        .append('div').attr('class',(d)=>d).classed('tbOpt',true)
        .text((d)=>d)
        
    ;

    //logic
    _piece.chosen = new VarPlus(initVal);
    

    _tbOpt_list.on('click',function(e,d){
        _piece.chosen.set(_piece.chosen.get() === d ? '_' : d);
    });

    _piece.chosen.modify(function up_piece_selected(value){
        _tbOpt_list.classed('selected',false);
        if(value !== '_'){
            _tbOpt_list.filter((d)=>d===value)
                .classed('selected',true);
        }
    });    

    return _piece;
}

/**
 * @param {d3.Selection<HTMLDivElement, any, HTMLElement, any> } _parent
 * @param {d3.Selection<HTMLDivElement, string, HTMLElement, any>  & {chosen: VarPlus<any>}} _piece
 * @returns {d3.Selection<HTMLDivElement, string, HTMLElement, any> & {chosen: VarPlus<any>}}
 */
function create_mainstat(_parent,_piece){
    //constant
    // const optionList = Object.values(code_piece);
    const thisName = 'mainstat';
    const initVal = '_';
    

    //element
    /** @type {d3.Selection<HTMLDivElement, string, HTMLElement, any> & {chosen: VarPlus<any>}} */
    const _mainstat = _parent.append('div').attr('id',thisName);

    const _tbHead = _mainstat.append('div').classed('tbHead',true);
    const _tbBody = _mainstat.append('div').classed('tbBody',true);

    const _title = _tbHead.append('div').text(thisName.toLocaleUpperCase());
    let _tbOpt_list  = _tbBody.selectAll('.tbOpt');//the rest depend on _piece.chosen
    
    

    //logic
    _mainstat.chosen = new VarPlus(initVal);
    /** @type {string[]} */
    let optionList;

    _piece.chosen.modify(function up_mainstat_option(pieceValue){
        

        optionList = pieceValue === '_' ? [] : piece_mainstat[pieceValue];
        
        
        _tbOpt_list = _tbOpt_list.data(optionList,d=>d).join(
            enter => enter.append('div').attr('class',(d)=>d).classed('tbOpt',true)
                .text(d=>d)
                .on('click',(e,d)=>{
                    
                    _mainstat.chosen.set(_mainstat.chosen.get() === d ? '_' : d);
                })
            ,
            merge => merge,
            exit => exit.remove()
        )
    }) 
    
    _piece.chosen.modify(function up_mainstatChosen(pieceValue){
        
        _mainstat.chosen.set(
            optionList.some(d=>d === _mainstat.chosen.get()) ?
            _mainstat.chosen.get() : '_');
    }) 

    

    _mainstat.chosen.modify(function up_mainstat_selected(value){
        
        _tbOpt_list.classed('selected',false);
        if(value !== '_'){
            _tbOpt_list.filter((d)=>d===value)
                .classed('selected',true);
        }
    });    

    return _mainstat;
}

/**
 * @param {d3.Selection<HTMLDivElement, any, HTMLElement, any> } _parent
 * @param {d3.Selection<HTMLDivElement, string, HTMLElement, any>  & {chosen: VarPlus<any>}} _mainstat
 * @returns {d3.Selection<HTMLDivElement, string, HTMLElement, any> & {chosen: VarPlus<string[]>}}
 */
function create_substat(_parent,_mainstat){
    //constant
    const optionList = Object.keys(code_stat).filter(k=>Number(k)>=0).map(k=>code_stat[k]);
    const thisName = 'substat';
    const initVal = [];

    
    
    //element
    /** @type {d3.Selection<HTMLDivElement, string, HTMLElement, any> & {chosen: VarPlus<string[]>}} */
    const _substat = _parent.append('div').attr('id',thisName);

    const _tbHead = _substat.append('div').classed('tbHead',true);
    const _tbBody = _substat.append('div').classed('tbBody',true);

    const _title = _tbHead.append('div').text(thisName.toLocaleUpperCase());
    let _tbOpt_list  = _tbBody.selectAll('.tbOpt')
    .data(optionList).enter()
    .append('div').attr('class',(d)=>d).classed('tbOpt',true)
    .text(d=>d)
    
    
    
    //logic
    _substat.chosen = new VarPlus(initVal);
    _tbOpt_list.on('click',function(e,d){         
        if(d3.select(this).classed('banned')) return;

        let oldData = _substat.chosen.get();
        let newData = oldData.filter(v=>v!==d);
        if(oldData.length === newData.length){
            newData.push(d);
        } 
        _substat.chosen.set(newData);
        
    });


    //state dependancy (must be before GUI)
    _mainstat.chosen.modify((badSub)=>{
        let newData = _substat.chosen.get().filter(subs=>subs !== badSub);
        _substat.chosen.set(newData);
    })

    //GUI dependancy
    _substat.chosen.modify((substatArr)=>{
        let bannedSub = _mainstat.chosen.get();
        _tbOpt_list 
            .classed('selected',d=>substatArr.some(subs=>subs===d))
            .classed('banned',d=>d===bannedSub)
        ;
        
    })
    
    return _substat;
}

// function printStat(v){
//     console.log({
//         piece:_piece.chosen.get(),
//         mainstat:_mainstat.chosen.get(),
//         substat:_substat.chosen.get().join(', '),
//     });
// }
// _mainstat.chosen.modify(printStat);
// _piece.chosen.modify(printStat);    
// _substat.chosen.modify(printStat);    

//#endregion
   

//result logic
//#region 

let summary = /** @type {VarPlus<{threshold:number,cost:number}[]>} */( new VarPlus([]));


function validate_input(){
    let piece = _piece.chosen.get();
    let mainstat = _mainstat.chosen.get();
    let substat = _substat.chosen.get();
    if(piece === '_' || mainstat === '_' || substat.length === 0){
        console.log('please select piece, mainstat and at least one substat');
        return false;
    }

    return [keyword_code[piece],keyword_code[mainstat],substat.map(s=>keyword_code[s])];
}
_clacButton.on('click',async ()=>{
    //TODO temporary values
    //TODO update result table value

    
    //validation 
    let result = validate_input();
    if( !result ) return;
    let [piece, mainstat, substat] = result;


    // execution
    let command = `/calculate/${piece}/${mainstat}/_/${substat.join('/')}`;
    console.log(command);
    /** @type {relic.JsonData} */
    let data = await fetch(command)
    .then(v=>v.json())
    .then(v=>v.map(d=>d[0]));
    
    
    // console.log(data.map(d=>d.treeLinks[0].nodeData));
    // let a = new Array(9).fill(1).map((_,i)=>({threshold:i+1,cost:Math.floor(Math.random()*1000)}));//TODO temporary value
    summary.set(data.map((d,i)=>{
        let rootdata = d.treeLinks[0].nodeData;
        let price = rootdata.succesR>0?
            (rootdata.price/rootdata.succesR/cost).toFixed(1):
            '_'
        ;
    
        return {
            threshold: i+1,
            cost: price
        }
    }));
    
    

})


summary.modify(summaryData=>{

    _summary_list = _summary_list
        .data(summaryData,
            d=>d.threshold
        )
        .join(
            enter => {
                let Enter = enter.append('div').classed('summaryNode',true)
                Enter.append('div').classed('thold',true).text(d=>d.threshold)
                Enter.append('div').classed('cost',true).text(d=>d.cost);
                return Enter;
            }  
            ,
            update =>{
                update
                .select('.cost')
                .text(d=>d.cost)
                return update;
            }
            ,
            exit => exit.remove()
        )
        .order()
    ;
      
});
// //appear after the calculation finished
// const _threshold = _result.append('div').attr('id','summary');

// let range = 5;
// let start = 1;

// // let summary = [
// //     {key:'threshold',value:'days(53100)'},
// //     {key:2,value:100},
// //     {key:3,value:123},
// //     {key:4,value:345},
// //     {key:5,value:567}
// // ];


// let _summary = _threshold.selectAll('.summaryNode')
//     .data(summary).enter()
//     .append('div')
//     // .classed('summaryNode',true)
    
//     ;
// _summary.append('div').text(d=>d.key)    
// _summary.append('div').text(d=>d.value)    
  
//#endregion
    




  