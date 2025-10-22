// import * as d3_raw from 'https://cdn.jsdelivr.net/npm/d3@7/+esm';
// /** @type {import( "d3" )} */
// let d3 = d3_raw;

const d3 = import('d3');
const event = require('node:events');
const util = require('node:util');

let a = new event();

// import * as d3 from 'd3';
// import * as event from 'node:events';
// import * as util from 'node:util';


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
      'HPP',
      'ATKP',
      'DEFP',
      'CD',
      'CR',
      'EHR',
      'HEAL'
    ],    
    BOOTS  :[
      'HPP',
      'ATKP',
      'DEFP',
      'SPD'
    ],    
    PLANAR :[
      'HPP',
      'ATKP',
      'DEFP',
      'DMG'
    ],    
    ROPE   :[
      'HPP',
      'ATKP',
      'DEFP',
      'BE',
      'ER'
    ],      
};
const code_stat = {
    '0'  :'HP'      ,
    '1'  :'ATK'     ,
    '2'  :'DEF'     ,
    '3'  :'HPP'     ,
    '4'  :'ATKP'    ,
    '5'  :'DEFP'    ,
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
HP     : 0,
ATK    : 1,
DEF    : 2,
HPP    : 3,
ATKP   : 4,
DEFP   : 5,
SPD    : 6,
CR     : 7,
CD     : 8,
EHR    : 9,
ERS    : 10,
BE     : 11,
DMG    : -1,
HEAL   : -2,
ER     : -3,

HEAD  :0,
HANDS :1,
BODY  :2,
BOOTS :3,
PLANAR:4,
ROPE  :5
};

//global
let piece, mainstat, substat;


//element
const _menu = d3.select('#menu');
const _option = _menu.append('div').attr('id','option');
const _piece    = create_piece(_option);
const _mainstat = create_piece(_option,'piece',Object.values(code_piece));
const _substat  = create_piece(_option,'piece',Object.values(code_piece));



/**
 * @param {d3.Selection<HTMLDivElement, any, HTMLElement, any> & {selecting: string}} _parent
 * @returns {d3.Selection<HTMLDivElement, string, HTMLElement, any> & {selecting: string}}
 */
function create_piece(_parent){
    //constant
    const optionList = Object.values(code_piece);
    const thisName = 'piece';
    const initVal = '_';
    

    //element
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

    _tbOpt_list.on('click',function(d){
        console.log(d);
        
        // _tbOpt_list.classed('selected')
    });


// let thisPiece = this.classList[1];
//     let thisPieceCode = keyword_code[thisPiece];
//     let thisSelection = d3.select(this);
//     let thisMainstat = piece_mainstat[thisPiece];
//     let thisMainstatStr = thisMainstat.map(v=>'.'+v).join(', ');

//     thisSelection.on('click',function(){

//         //piece wise 
//         // if selected : node not selected
//         // if not selected : node be selected
//         if(thisSelection.classed('selected')){
//             thisSelection.classed('selected',false);
//             pieceCode = '_';
            
//         } else {
//             pieceCode = thisPieceCode;
//             pieceOpt.classed('selected',false);
//             thisSelection.classed('selected',true);

//             //change mainstat option
            
//         }

        
//         //mainstat wise
//         // after piece wise
//         // if none selected: show nothing
//         // if there is piece selected: show piece mainstat
//         //none is selected
//         mainstatOpt.classed('selected',false);
//         mainstatCode = '_';
//         mainstatOpt.style('display','none'); 
//         if(typeof pieceCode === 'number'){
//             d3.selectAll('#mainstat :is('+thisMainstatStr+')')
//                 .style('display','block')
//                 ;
//         }

//         //substat wise
//         // since mainstat is not going to be clicked, then unban all sub
//         substatOpt.classed('banned',false);
//         substatCode = [];
//         substatOpt.each(function(){
//             if (
//                 d3.select(this).classed('selected') &&
//                 !d3.select(this).classed('banned') 
//             )substatCode.push(keyword_code[this.classList[1]]);
//         });
    
//         //apply changes to code
//         codeElement.text(getCode());
//     });
// });

    return _piece;
}
   
    
// <div class="tbHead">
//     <div>PEICE</div>
// </div>
// <div class="tbBody">
//     <div class="tbOpt HEAD   "> HEAD   </div>
//     <div class="tbOpt HANDS  "> HANDS  </div>
//     <div class="tbOpt BODY   "> BODY   </div>
//     <div class="tbOpt BOOTS  "> BOOTS  </div>
//     <div class="tbOpt PLANAR "> PLANAR </div>
//     <div class="tbOpt ROPE   "> ROPE   </div>
// </div> 

    
/* 

<div id="threshold">
                <div class="tbHead">
                    <div>Threshold</div>
                </div>
                <div class="tbBody">
                    <div id="thresholdMeter">
                        <div>6</div> 
                    </div>
                    <div>
                        <input type="range" id="thresholdSlider" min="1" max="9" step="1">
                    </div>
                </div> 
                
            </div>
*/






//control panel logic

// let pieceCode = '_';
// let mainstatCode = '_';
// let thresholdCode = '6';
// let substatCode = [];


// let getCode = ()=>`./relicUpgrade ${pieceCode} ${mainstatCode} ${thresholdCode} ${substatCode.join(' ')}`;   








// let pieceOpt = d3.selectAll('#piece .tbOpt');
// let mainstatOpt = d3.selectAll('#mainstat .tbOpt');
// let thresholdOpt = d3.selectAll('#thresholdSlider');
// let substatOpt = d3.selectAll('#substat .tbOpt');
// let codeElement = d3.select('#code');
// let descElement = d3.select('#relicDesc');

// d3.select('#jsonLabel').on('input',function(){
//     let value = d3.select(this).property('value');
//     let json = JSON.parse(value);

//     descElement.text(
//         `piece : ${code_piece[json.piece]}\n`
//         + `mainstat : ${code_stat[json.mainstat]}\n`
//         + `threshold : ${json.threshold}\n`
//         + `substat : ${json.substat.map(v=>code_stat[v]).join(' ')}`

//     );
     
  
//   })

// //   fetch('/browserUI/result/LABEL.json').then(
// //     response=>{
// //         if(response.ok) return response.json();
// //         else return Promise.reject(new Error("no file"));
// //     }
// //   ). then (
// //     json => {
// //         descElement.text(
// //             `piece : ${code_piece[json.piece]}\n`
// //             + `mainstat : ${code_stat[json.mainstat]}\n`
// //             + `threshold : ${json.threshold}\n`
// //             + `substat : ${json.substat.map(v=>code_stat[v]).join(' ')}`
// //         )
// //     }
// //   )






// pieceOpt.each(function(){//append onclick event
//     let thisPiece = this.classList[1];
//     let thisPieceCode = keyword_code[thisPiece];
//     let thisSelection = d3.select(this);
//     let thisMainstat = piece_mainstat[thisPiece];
//     let thisMainstatStr = thisMainstat.map(v=>'.'+v).join(', ');

//     thisSelection.on('click',function(){

//         //piece wise 
//         // if selected : node not selected
//         // if not selected : node be selected
//         if(thisSelection.classed('selected')){
//             thisSelection.classed('selected',false);
//             pieceCode = '_';
            
//         } else {
//             pieceCode = thisPieceCode;
//             pieceOpt.classed('selected',false);
//             thisSelection.classed('selected',true);

//             //change mainstat option
            
//         }

        
//         //mainstat wise
//         // after piece wise
//         // if none selected: show nothing
//         // if there is piece selected: show piece mainstat
//         //none is selected
//         mainstatOpt.classed('selected',false);
//         mainstatCode = '_';
//         mainstatOpt.style('display','none'); 
//         if(typeof pieceCode === 'number'){
//             d3.selectAll('#mainstat :is('+thisMainstatStr+')')
//                 .style('display','block')
//                 ;
//         }

//         //substat wise
//         // since mainstat is not going to be clicked, then unban all sub
//         substatOpt.classed('banned',false);
//         substatCode = [];
//         substatOpt.each(function(){
//             if (
//                 d3.select(this).classed('selected') &&
//                 !d3.select(this).classed('banned') 
//             )substatCode.push(keyword_code[this.classList[1]]);
//         });
    
//         //apply changes to code
//         codeElement.text(getCode());
//     });
// });

// mainstatOpt.each(function(){//append onclick event
//     let thisMain = this.classList[1];//stat name
//     let thisMainCode = keyword_code[thisMain];//stat code
//     let thisSelection = d3.select(this);//selection containing this mainstat
//     let subSelection = substatOpt.filter(function(){
//         return d3.select(this).classed(thisMain);
//     })

//     thisSelection.on('click',function(){

//         //mainstat wise + substat wise
//         // if click on selected: unselect node, and unban subs == this
//         // if click on not selected: un select all, select node, 
//         if(thisSelection.classed('selected')){
//             thisSelection.classed('selected',false);
//             mainstatCode = '_';

//             substatOpt.classed('banned',false);

//         } else {//initially not selected
            
//             mainstatOpt.classed('selected',false);
//             thisSelection.classed('selected',true);
//             mainstatCode = thisMainCode;

//             substatOpt.classed('banned',false);
//             subSelection.classed('banned',true);
//             substatCode = [];
//             substatOpt.each(function(){
//                 if (
//                     d3.select(this).classed('selected') &&
//                     !d3.select(this).classed('banned') 
//                 )substatCode.push(keyword_code[this.classList[1]]);
//             });

            
//         }
  
//         //apply changes to code
//         codeElement.text(getCode());

//     });
// });

// substatOpt.each(function(){//append onclick event
//     let thisSub = this.classList[1];//stat name
//     let thisSubCode = keyword_code[thisSub];//stat code
//     let thisSelection = d3.select(this);//selection containing this mainstat
    

//     thisSelection.on('click',function(){

//         //update substat
//         if(thisSelection.classed('selected')){
//             thisSelection.classed('selected',false);

//             substatCode = [];
//             substatOpt.each(function(){
//                 if (
//                     d3.select(this).classed('selected') &&
//                     !d3.select(this).classed('banned') 
//                 )substatCode.push(keyword_code[this.classList[1]]);
//             });

//         } else if(thisSelection.classed('banned')){
//             //nothing

//         } else {//initially not selected
//             thisSelection.classed('selected',true);
            
//             substatCode.push(thisSubCode);
//         }
        
//         codeElement.text(getCode());
//     });

// });



// // thresholdOpt.on('input',function(){
// //     d3.select('#thresholdMeter > div').text(this.value);
    
// //     //update global var
// //     thresholdCode = this.value;
// //     codeElement.text(getCode());
// // });




  