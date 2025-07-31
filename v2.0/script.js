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









/**
 * @param {SVGElement[]} drawnNode 
 * @param {link[]} links 
 * @param {import ("d3").Selection<SVGSVGElement, any, HTMLElement, any>} svg_g 
 */
/* done */function __drawLinks(drawnNode,links,svg_g){
  console.log(svg_g.node());
  let svg_bound = svg_g.node().getBoundingClientRect();
  
  links.forEach(link =>{  
    if(link.dst && link.dst) {
      let src_bound = drawnNode[link.src].getBoundingClientRect();
      let dst_bound = drawnNode[link.dst].getBoundingClientRect();

      svg_g.append('line')
        .attr('x1',(src_bound.left + src_bound.right)/2 - svg_bound.x)
        .attr('y1',src_bound.bottom - svg_bound.y)
        .attr('x2',(dst_bound.left + dst_bound.right)/2 - svg_bound.x)
        .attr('y2',dst_bound.top - svg_bound.y)
        .attr('stroke-width',1)
        .attr('stroke','black')
    } 
  })
  
}
/**
 * @param {import ("d3").Selection<SVGSVGElement, any, HTMLElement, any>} gnodeRoot 
 * @returns {number}
 */
/* done */function __fixNodesX(gnodeRoot){
  let offset = 0;
  let children = gnodeRoot.selectAll(':scope > .gnode');
  children.nodes().forEach(node=>{
    gnodeRoot.select(function (){return node;}).attr('transform','translate('+offset+',100)');
    offset += __fixNodesX(gnodeRoot.select(function (){return node;})) + nodeSetting.padding;
  });

  //move parent
  let childWidth = offset - nodeSetting.padding; 
  if(childWidth > 0) gnodeRoot.select(':scope > .gtext')
    .attr('transform','translate('+(childWidth-nodeSetting.width)/2+',0)');

  
  return childWidth >0 ? childWidth: nodeSetting.width;
}
/**
 * @param {SVGElement[]} drawnNode 
 * @param {link[]} links 
 * @param {import ("d3").Selection<SVGSVGElement, any, HTMLElement, any>} svg_g 
 */
/* done */function __connectNodes(drawnNode,links,svg_g){
  links.forEach((link)=>{
    if(link.dst && link.dst) {
      let child = drawnNode[link.dst];// this only include the gtext
      let parent = drawnNode[link.src];
      parent.parentElement.appendChild(child.parentElement);
    }
  })
}
/**
 * @param {import ("d3").Selection<SVGSVGElement, any, HTMLElement, any>} svg_g
 * @param {node[]} nodes
 * @returns {SVGElement[]} 
 */
/* done */function __drawNodes(svg_g,nodes){
  svg_g.selectAll("aaaa")
  .data(nodes).enter()
  .append('g').attr('class',function(d){return d.detail})
    .classed('gnode',true)
    /* redundant */.attr('transform','translate(0,'+(2*nodeSetting.height)+')')
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

  console.log(links);
  let heir = d3.stratify();
  heir = heir.id((d) => d.dst);
  heir = heir.parentId((d) => d.src);
  heir = heir(links);
  console.log(heir);

  

  
  let drawnNode = __drawNodes(svg_g,nodes);
  __connectNodes(drawnNode,links,svg_g);
  let drawnNodeRoot = svg_g.select(':scope > .gnode');
  drawnNodeRoot.attr('transform','translate(0,0)');
  __fixNodesX(drawnNodeRoot);
  __drawLinks(drawnNode,links,svg_g);
  

  //fixing svg size
  let mysvg_Size = mysvg.node().getBoundingClientRect();
  let svg_g_Size = svg_g.node().getBoundingClientRect();
  mysvg
    .attr('width', svg_g_Size.right-mysvg_Size.x)
    .attr('height',svg_g_Size.bottom-mysvg_Size.y);

  


 

}
 



console.log();
drawGraph();