import * as d3Raw from 'https://cdn.jsdelivr.net/npm/d3@7/+esm';

/** @type {import ("d3")} */  
const d3 =d3Raw;



{/* <svg width="200" height="100">
  <rect x="0" y="0" width="200" height="100" stroke="red" stroke-width="3px" fill="white"/>
  <text x="50%" y="50%" dominant-baseline="middle" text-anchor="middle">TEXT</text>    
</svg> */}

let mysvg = d3.select('body').append('svg')
  .style('background-color','aqua');

// let marginx = '100px';
// let marginy = '100px';
// mysvg.append('text')
//   .text('test123')
//   .attr('x',marginx)
//   .attr('y',marginy)
  
// mysvg.append('rect')
//   .attr('x','0')
//   .attr('y','0')
//   .attr('width',marginx)
//   .attr('height',marginy)
//   .style('fill','green');

let textbox = {
  x:100,
  y:10,
  width:100,
  height:20,
  fill:'green',
  msg:'loremIpsum'
}

mysvg.append('g')
  .attr('transform','translate('+textbox.x+','+textbox.y+')')
.append('rect')
  .attr('width',textbox.width)
  .attr('height',textbox.height)
  .attr('fill',textbox.fill)
.select(function (){return this.parentNode})
.append('text')
  .attr('x',textbox.width/2)
  .attr('y',textbox.height/2)
  .attr('text-anchor','middle')
  .attr('dominant-baseline','middle')
  .text(textbox.msg)

mysvg.append('line')
  .attr('x1',100)
  .attr('y1',100)
  .attr('x2',50)
  .attr('y2',150)
  .style('stroke-width','2')
  .style('stroke','black')

;








let node = [
  {x:100,y:0,msg:'aaaaaa'},
  {x:0,y:100,msg:'aaaaaa'},
  {x:300,y:200,msg:'aaaaaa'}
];


async function drawCartesian(){
  // Set Dimensions
  const xSize = 580; 
  const ySize = 580;
  const margin = 40;
  const xMax = xSize - margin*2;
  const yMax = ySize - margin*2;

  //append svg
  let mysvg = d3.select('body').append('svg')
    .attr('id','mygraph')
    .style("width",xSize)
    .style("height",ySize)
    /* test */.style("background-color"," aqua");

  //append margin 
  let graph = mysvg.append("g")
    .attr("transform","translate(" + margin + "," + margin + ")")
    .style("background-color"," green");

  //append axis
  const y = d3.scaleLinear().range([yMax, 0]).domain([0, 100]);
  graph.append("g").call(d3.axisLeft(y));
  const x = d3.scaleLinear().domain([0,10]).range([0,xMax]);
  graph.append('g').call(d3.axisBottom(x))
    .attr("transform","translate(0,"+yMax+")");
    
  // append dots
  let jsondata =  (await (await fetch('mydata.json')).json());
  /** @type {[{x:number,y:number}]} */
  let nodes = jsondata[0].nodes;

  graph.append('g')
    .selectAll('points')  
    .data(nodes).enter()
    .append('circle')
    .attr('cx', function(m){return x(m.x)})
    .attr('cy', function(d){return y(d.y)})
    .attr('r', 3)
    .style('fill','red')
  ;

  /** @type {[{src:number,dst:number}]} */
  let links= jsondata[0].links;
  /** @type {[{x1:number,y1:number,x2:number,y2:number}]} */
  let linkscord = links.map(function(value){return {
    x1:nodes[value.src].x,
    y1:nodes[value.src].y,
    x2:nodes[value.dst].x,
    y2:nodes[value.dst].y
  }});

  graph.append('g')
    .selectAll('links')
    .data(linkscord).enter()
    .append('line')
    .attr('x1',function(d){return x(d.x1)})
    .attr('y1',function(d){return y(d.y1)})
    .attr('x2',function(d){return x(d.x2)})
    .attr('y2',function(d){return y(d.y2)})
    .style('stroke-width','2')
    .style('stroke','black')


}


/**
 * @param {string} msg 
 * @param {number} myInt 
 */
function dostuff2(msg,myInt){
    console.log(msg);
    console.log(myInt);

    /**
     * @type {{a:boolean, b:number, c:string}}
     */ 
    var foo;
    foo.a = true;
    foo.b = 11; 
}

/* 
tldr the size of svg element snapshot
*/
d3.select('.but1').on('click',()=>{
  marginx = '10px';
});
d3.select('.but2').on('click',()=>{
  console.log(marginx);
});
