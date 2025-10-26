// import * as d3Raw from 'https://cdn.jsdelivr.net/npm/d3@7/+esm';
// /** @type {import("d3")} */
// const d3 = d3Raw;
import * as d3 from 'd3'

import {execFile} from 'child_process';

// test();
function test(){
  let a = {
    mm:12,
    nn:'hello'
  }
  let b = {
    mm:2,
    nn:'hello'
  }

  let yes = 1;
  for(let v of Object.entries(a)){
    if( v[1] === b[v[0]]) continue;
    yes = 0;
    break;
  }
  console.log(yes);
}

function test2(){
    let width = 400;
    let height = 600;
    let svg = d3.select('body').append('svg')
      .attr('width',width)
      .attr('height',height)
    ;
    let svg_g = svg.append('g');
  
    let linker = d3.linkHorizontal();
    console.log(linker.x());
    console.log(linker.y());
    console.log(linker([{source:[0,0],target:[100,100]}]));
    console.log(null);
    
  
    let mypath = svg_g.selectAll('blabla')
      .data([{source:[0,0],target:[100,100]}]).enter()
      .append('path')
      .attr('class','link')
      .attr('d',linker)
      .attr('fill','none')
      .attr('stroke','black')
    ;
    console.log(mypath);

}

execFille();
function execFille(){
  execFile('./test1.exe',(error,stdout,stderr)=>{
    if(error)console.log('error',error);
    if(stdout)console.log('stdout',stdout);
    if(stderr)console.log('stderr',stderr);
  })
}


