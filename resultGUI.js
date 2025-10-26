import * as VarPlus from './VarPlus.js'
import * as d3_raw from 'https://cdn.jsdelivr.net/npm/d3@7/+esm';
import * as relic from './script.js'
/** @type {import( "d3" )} */
let d3 = d3_raw;




const _menu = d3.select('#menu');
const _result = _menu.append('div').attr('id','result');
const _clacButton = _result.append('button')
    .text('calculate')    
    .on('click',async ()=>{
        //TODO temporary values
        let piece = '2';
        let mainstat = '7';
        let substat = ['8','4','5'];


        let command = `/calculate/${piece}/${mainstat}/_/${substat.join('/')}`;
        /** @type {relic.JsonData} */
        let data = await fetch(command)
        .then(v=>v.json())
        .then(v=>v.map(d=>d[0]));
        
        
        console.log(data.map(d=>d.treeLinks[0].nodeData));
        
        
        

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

    
