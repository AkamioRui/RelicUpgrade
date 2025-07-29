import * as d3Raw from 'https://cdn.jsdelivr.net/npm/d3@7/+esm';

/** @type {import ("d3")} */  
const d3 =d3Raw;

//@ts-check

/** 
 * @typedef {object} node
 * @property {number} depth 
 * @property {number} price 
 * @property {number} succesR - success rate
 * @property {string} detail
 * @property {string} accept
 */
/** 
 * @typedef {object} link
 * @property {number} src
 * @property {number} dst
 * @property {number} chance
 */
let nodeSetting={
  fill:["red","green"],
  width:75,
  height:50,
  padding:20
}



function __drawLinks(){

}
/**
 * @param {import ("d3").Selection<SVGSVGElement, any, HTMLElement, any>} gnodeRoot 
 * @returns {number}
 */
function __fixNodesX(gnodeRoot){

  let parents = gnodeRoot.selectAll(':scope > .gtext');
  let parentsLength = parents.size();
  let parentWidthTotal = - nodeSetting.padding 
    + parentsLength *(nodeSetting.width + nodeSetting.padding);

  let children = gnodeRoot.selectAll(':scope > .gnode');
  let childrenLength = children.size();
  let childWidth = children.nodes().map((node)=>{
    return __fixNodesX(gnodeRoot.select(function (){return node;}));
  });
  let childTotalWidth = - nodeSetting.padding;
  childWidth.forEach((width)=>{
    childTotalWidth += width + nodeSetting.padding;
  });

  

  //adjusting x
  let width, childCurrent, parentCurrent, childPadding, parentPadding;
  if(childTotalWidth > parentWidthTotal){
    width = childTotalWidth;
    childPadding = nodeSetting.padding;
    parentPadding = (parentWidthTotal - parentsLength*nodeSetting.width)/(parentsLength+1);
    childCurrent = 0;
    parentCurrent = parentPadding;
    

  } else {
    width = parentWidthTotal;
  }

  

  return width;
}
/**
 * @param {SVGElement[]} drawnNode 
 * @param {link[]} links 
 * @param {import ("d3").Selection<SVGSVGElement, any, HTMLElement, any>} svg_g 
 */
function __connectNodes(drawnNode,links,svg_g){
  links.forEach((link)=>{
    let child = drawnNode[link.dst];// this only include the gtext
    let parent = drawnNode[link.src];
    if(child.parentElement.parentElement == svg_g.node()){
      parent.parentElement.appendChild(child.parentElement)
      
    } else {
      let temp = parent.parentElement;
      child.parentElement.parentElement.appendChild(parent)
      temp.remove();
    }
  })
}
/**
 * @param {import ("d3").Selection<SVGSVGElement, any, HTMLElement, any>} svg_g
 * @param {node[]} nodes
 * @returns {SVGElement[]} 
 */
function __drawNodes(svg_g,nodes){
  svg_g.selectAll("aaaa")
  .data(nodes).enter()
  .append('g').attr('class',function(d){return d.detail})
    .classed('gnode',true)
    .attr('transform','translate(0,'+(2*nodeSetting.height)+')')
    .attr('width',0)
  .append('g').attr('class',function(d){return d.detail})
  .classed('gtext',true)
  .append('rect')
    .attr('fill', (d)=>{return nodeSetting.fill[d.accept]})
    .attr('width',nodeSetting.width)
    .attr('height',nodeSetting.height)
  .select(function (){return this.parentElement;}).append('text')
  .append('tspan')
    .text((d)=>{return d.detail})
    .attr('x',nodeSetting.width/2)
    .attr('y',nodeSetting.height/2)
    .attr('dy','-.9em')
    .attr('dominant-baseline','middle')
    .attr('text-anchor','middle')
  .select(function (){return this.parentElement}).append('tspan')
    .text((d)=>{return d.price})  
    .attr('x',nodeSetting.width/2)
    .attr('y',nodeSetting.height/2)
    .attr('dy','0em')
    .attr('dominant-baseline','middle')
    .attr('text-anchor','middle')
  .select(function (){return this.parentElement}).append('tspan')
    .text((d)=>{return d.succesR})  
    .attr('x',nodeSetting.width/2)
    .attr('y',nodeSetting.height/2)
    .attr('dy','0.9em')
    .attr('dominant-baseline','middle')
    .attr('text-anchor','middle') 

  /** @type {SVGElement[]} */
  return svg_g.selectAll('g.gtext').nodes();

}
async function drawGraph(){
  
  /** @type {{nodes:node[],links:link[]}[]} */
  let jsondata = await (await fetch('mydata.json')).json();
  

  //for each graph
  let mysvg = d3.select('body').append('svg')
    .style('background-color','aqua');
  let svg_g = mysvg.append('g');
  let currentGraph = 0;
  let nodes = jsondata[currentGraph].nodes;
  let links = jsondata[currentGraph].links;

  
  let drawnNode = __drawNodes(svg_g,nodes);
  __connectNodes(drawnNode,links,svg_g);
  __fixNodesX(svg_g.select(':scope > .gnode'));
  // __drawLinks();
  

  //fixing svg size
  let mysvg_Size = mysvg.node().getBoundingClientRect();
  let svg_g_Size = svg_g.node().getBoundingClientRect();
  mysvg
    .attr('width', svg_g_Size.right-mysvg_Size.x)
    .attr('height',svg_g_Size.bottom-mysvg_Size.y);

  


 

}
 



console.log();
drawGraph();