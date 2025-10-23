import * as VarPlus from './VarPlus.js'
import * as d3_raw from 'https://cdn.jsdelivr.net/npm/d3@7/+esm';
/** @type {import( "d3" )} */
let d3 = d3_raw;


const _menu = d3.select('#menu');
const _result = _menu.append('div').attr('id','result');
const _clacButton = _result.append('button')
    .text('calculate')    
    .on('click',()=>{
        
        fetch('/calculate')
        .then(v=>v.text())
        .then(v=>console.log(v))
        

    })
;

//



//appear after the calculation finished
const _threshold = _result.append('div').attr('id','summary');

let range = 5;
let start = 1;

let summary = [
    {key:'threshold',value:'days(53100)'},
    {key:2,value:100},
    {key:3,value:123},
    {key:4,value:345},
    {key:5,value:567}
];


let _summary = _threshold.selectAll('.summaryNode')
    .data(summary).enter()
    .append('div')
    // .classed('summaryNode',true)
    
    ;
_summary.append('div').text(d=>d.key)    
_summary.append('div').text(d=>d.value)    

// function init(){}    
// function update(){}    

    
