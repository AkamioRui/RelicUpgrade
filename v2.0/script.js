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
  width:110,
  height:50,
  padding:20
}


// test();


drawGraphD3();
async function drawGraphD3(){
  //common element
  const all = d3.select('body').append('div');
  const backButton = all.append('button').text('back');
  const nextButton = all.append('button').text('next');
  all.append('br');
  const svg = all.append('svg');
  const svg_g = svg.append('g');

  /** @typedef {{chance:number,price:number,succesR:number,detail:string,accept:number}} treeLinkData */
  /** @typedef {{id:number,parentId:any,data:treeLinkData}} treeLink */
  /** @typedef {{id:number,parentId:any,data:{chance:number}}} outerLink */

  


  //create some constant variable
  /** @type {{treeLinks:treeLink[],outerLinks:outerLink[]}[]} */
  const jsondata = await (await fetch('mydata.json')).json();
  /** @type {d3.StratifyOperator<treeLink>} */
  const json2heirarchy = d3.stratify();
    json2heirarchy
    .id(d=>d.id)
    .parentId(d=>d.parentId)
  ;
  /** @type {d3.TreeLayout<treeLink>} */
  const treelayout = d3.tree()
    .nodeSize([nodeSetting.width+nodeSetting.padding*2,nodeSetting.height*2])
  ; 
  let currentGraph = 0;
  /** @type {d3.HierarchyPointLink<treeLink>[]} */
  let treeLinks;
  /* 
  children: (2) [h, h]
  data: {id: 0, parentId: '', data: {…}}
  depth: 0
  height: 2
  id: "0"
  parent: null
  x: 0
  y: 0
  */
  /** @type {d3.HierarchyNode<treeLink>[]} */
  let treeNodes;
  function getGraphData(){
    let root = json2heirarchy(jsondata[currentGraph].treeLinks);
    treeLinks = treelayout(root).links();
    treeNodes = root.descendants();

    let treeNodesMap = [];
    treeNodes.forEach((d)=>{
      treeNodesMap[d.id] = d;
    })

    let treeOuterLinks = jsondata[currentGraph].outerLinks.map(
      (v)=>{return {source:treeNodesMap[v.parentId],target:treeNodesMap[v.id]} ;}
    )
    treeLinks.push(...treeOuterLinks);
  }
  getGraphData();
  
  
  
  function resizeSVG(){
    let xArr = treeNodes.map((d)=>d.x);
    let minX = Math.min(...xArr);
    let maxX = Math.max(...xArr);
    let yArr = treeNodes.map(d=>d.y);

    svg_g.attr('transform','translate('
      +(minX*(-1) + nodeSetting.width/2)+
      ','
      +(nodeSetting.height/2)+
      ')')
    ;

    svg
    .attr('width',nodeSetting.width + maxX-minX+1)
    .attr('height',nodeSetting.height + Math.max(...yArr) + 1)
  }
  resizeSVG();




  //drawLinks
  /**
   * @param {d3.Selection<SVGPathElement, HierarchyPointLink<treeLink>, SVGGElement, any>} myselection 
   */
  /** @type {d3.Link< any, d3.HierarchyPointLink<treeLink>, d3.HierarchyNode<treeLink> >} */
  const linkGenerator = d3.linkVertical();
    linkGenerator
    .source(d=>d.source)
    .target(d=>d.target)
    .x(d=>d.x)
    .y(d=>d.y)
  ;
  /**
   * @param { d3.Selection<SVGPathElement, d3.HierarchyPointLink<treeLink>, SVGGElement, any>} myselection 
   * @param {string} color 
   */
  function adjustLink(myselection,color){
    myselection
      .attr('d',linkGenerator)
      .attr('fill',"none")
      .attr('stroke',color)
  }

  const linkElements = svg_g
    .append('g').attr('class','links')
    .selectAll('linkElement')
    .data(treeLinks).enter()
    .append('path') 
    .call(adjustLink,'black')
  ;

  ///*  */3 gone, 5 edited, 6 added
  //normal: black, enter:green
  function updateLinks(){
    linkElements.data(treeLinks, d=>d.source.id+','+d.target.id ).join(
        enter => enter
          .append('path')
          .call(adjustLink,'lime')  
        ,
        update => update.call(adjustLink,'black'),
        exit => exit.remove()
      );
  }
  

  //draw node;
  /**
   * @param { d3.Selection<SVGGElement, d3.HierarchyNode<treeLink>, SVGGElement, any>} selection 
   * @param {string} color 
   */
  function initNode(selection,color){
    selection
      .attr('transform', d => 'translate('+d.x+','+d.y+')')
      .each(function (d){
        this.oldData = d.data.data;
      })
    selection.append('rect')
      .attr('x'       ,-nodeSetting.width/2)
      .attr('y'       ,-nodeSetting.height/2)
      .attr('width'   ,nodeSetting.width)
      .attr('height'  ,nodeSetting.height)
      .attr('fill','white')
      .attr('stroke',color)

    let text = selection.append('text');
    text  
      .attr('dominant-baseline','middle')
      .attr('text-anchor','middle')
    text.append('tspan').attr('class','detail')
      .text(d=>d.data.data.detail)
      .attr('dy','-0.9em')
      .attr('x','0')
      .attr('y','0')
    text.append('tspan').attr('class','price')
      .text(d=>d.data.data.price)
      .attr('dy','0.9em')
      .attr('x','0')
    text.append('tspan').attr('class','succesR')
      .text(d=>d.data.data.succesR)
      .attr('dy','0.9em')
      .attr('x','0')
    text.append('tspan').attr('class','accept')//this will be depricated, accept will dictate the color of rect
      .text(d=>d.data.data.accept)
      .attr('dy','0.9em')
      .attr('x','0')
  }
  const nodeElements = svg_g
    .append('g').attr('class','nodes')
    .selectAll('nodeElement')
    .data(treeNodes).enter()
    .append('g').attr('class','node')
    .call(initNode,'black')

  //normal:black, new:lime, updated:aqua
  function reRenderNodes(){
    /**
     * @param { d3.Selection<SVGGElement , d3.HierarchyNode<treeLink>, SVGGElement, any> } selection 
     * @param {string} colorModiefied 
     * @param {string} colorStable 
     */
    function updateNode(selection,colorModiefied,colorStable){
      selection
        .attr('transform', d => 'translate('+d.x+','+d.y+')')
        .each(function (d){
          let treeLinkData = d.data.data;
          this.modified = 0;
          for(let v of Object.entries(this.oldData)){
            if( v[1] == treeLinkData[v[0]]) continue;
            this.modified = 1;
            break;
          }
          this.oldData = d.data;
        })
      selection.select('rect')
        .attr('stroke',function (){
          if(this.parentNode.modified)return colorModiefied;
          else return colorStable;
        })

      let text = selection.select('text');
      text.select('tspan.detail')
        .text(d=>d.data.data.detail)
      text.select('tspan.price')
        .text(d=>d.data.data.price)
      text.select('tspan.succesR')
        .text(d=>d.data.data.succesR)
      //this will be depricated, accept will dictate the color of rect
      text.select('tspan.accept')
        .text(d=>d.data.data.accept)
      
    }

    //join statement
    nodeElements.data(treeNodes,d=>d.id).join(
      enter => enter.append('g').attr('class','node')
      .call(initNode,'lime'),
      update => update.call(updateNode,'aqua','black'),
      exit => exit.remove()
    )

  }
  


  /*  */nextGraph();
  function nextGraph(){
    currentGraph++;
    getGraphData();
    resizeSVG();
    updateLinks();
    reRenderNodes();
  }
  nextButton.on('click',nextGraph);
}

// drawGraph2();
async function drawGraph2(){
  let button_back = d3.select('body').append('button').text('back');
  let button_next = d3.select('body').append('button').text('next');
  d3.select('body').append('br');
  let svg = d3.select('body').append('svg');
  let svg_g = svg.append('g');

  

  /** @type {{nodes:node[],links:link[]}[]} */
  const data = await (await fetch('mydata.json')).json();
  let currentGraph = 0;
  /** @type {node[]} */
  let treeNodesData;
  /** @type {d3.HierarchyNode<link>[]}*/
  let treeNodes;
  /** @type {d3.HierarchyPointLink<link>[]}*/
  let treeLinks, treeOuterLinks;
  const treelayout = d3.tree().nodeSize([50,100]);
  const table2Heirarchy = d3.stratify()
    .id(function (d){return d.dst})
    .parentId(function (d){return d.src})
  ;


  function generateCoordinate(){
    treeNodesData = data[currentGraph].nodes;
    const root = table2Heirarchy (data[currentGraph].links);
    treeLinks = treelayout(root).links();//set x and y for each node
    treeNodes = root.descendants();
    treeNodes.forEach((d)=>{
      d.mydata = treeNodesData[d.data.dst]
    })
    /* test */console.log(treeNodes);

    //still odd
    treeOuterLinks = [];
    treeOuterLinks.push({source:treeNodes[2],target:treeNodes[4]})
    treeLinks = treeLinks.concat(treeOuterLinks);
  };
  generateCoordinate();
  
  
 

  
 /**
  * @param {d3.Selection<SVGPathElement, HierarchyPointLink<link>, SVGGElement, any>} myselection 
  * @param {string} color 
  */
  function adjustPath(myselection,color){
    myselection
    .attr('fill','none')
    .attr('stroke',color)
    .attr('stroke-width',2)
    .attr('d',d3.linkVertical()
      .x( d=>d.x )
      .y( d=>d.y )
    )
    .each
  }
  /**
   * @param {d3.Selection<SVGGElement, d3.HierarchyNode<link> & {mydata:node}, HTMLElement, any>} myselection 
   * @param {string} color 
   */
  function adjustNode(myselection,color){
    myselection
    .attr('transform',(d)=>{return 'translate('+d.x+','+d.y+')'})
    .each(function(d){
      this.mydata = d.mydata;
      this.ischanged = 0;
    })
  .append('circle')
    .attr('r',4)
    .attr('fill',color)
    .attr('stroke','black')
    .attr('stroke-width',1.5)
  .select(function (){return this.parentElement;}).append('text')
    .text( d => {
      /* test */console.log(d);
      return d.mydata.detail;
    })
    .attr('x',4)
    .attr('dominant-baseline','middle')
  }
  /**
   * @param {d3.Selection<SVGGElement, d3.HierarchyNode<link> & {mydata:node}, HTMLElement, any>} myselection 
   * @param {string} color 
   */
  function updateNode(myselection,color_modified,color_unchanged){
    
    myselection
    .attr('transform',(d)=>{return 'translate('+d.x+','+d.y+')'})
    .each(function(d){
      for(let v of Object.entries(this.mydata)){
        if(d.mydata[v[0]]==v[1]) continue;
        else{   
          this.mydata = d.mydata;   
          this.ischanged = 1;
          break;
        }
      }
    })
    myselection.select('circle')
      // .attr('r',4)
      .attr('fill',function(){
        if(this.parentElement.ischanged) return color_modified;
        else return color_unchanged;
      })
      // .attr('stroke','black')
      // .attr('stroke-width',1.5)
    myselection.select('text')
      .text( function(d) {
        console.log(d);
        return d.mydata.detail;
      })
      // .attr('x',4)
      // .attr('dominant-baseline','middle')
  }
  
  
  
  
  let linksElement = svg_g.append('g').attr('class','link').selectAll('path')
    .data(treeLinks).enter()
    .append("path")
    .call(adjustPath,'black')
  ;
  let nodesElement = svg_g.append('g').attr('class','node').selectAll('g.node')
    .data(treeNodes).enter()
    .append('g').attr('class','node')
    .call(adjustNode,'blue')  
  ;
  
  


    // updateGraph();
  function updateGraph(){
    let a = 
    nodesElement
      .data(treeNodes, d => d.id);
    console.log(a);
    
    a.join(
        enter => enter.append('g').attr('class','node').call(adjustNode,'green'),
        update => update.call(updateNode,'blue','black'),
        exit => exit.remove()
      )
    ;
    console.log(a);

    linksElement
      .data(treeLinks, d => d.source.id+','+ d.target.id)
      .join(
        enter => enter.append('path').call(adjustPath,'green'),
        update => update.call(adjustPath,'black'),
        exit => exit.remove()
      )  
    ;
      
  }
  
  
  


  //correcting
  /** @type {DOMRect} */
  let svg_bound, svg_g_bound;
  function adjustSVG(){
    svg_g
    .attr('transform','translate(0,0)');
    svg_bound = svg.node().getBoundingClientRect();
    svg_g_bound = svg_g.node().getBoundingClientRect();
    
    svg_g
    .attr('transform','translate('+(svg_bound.x - svg_g_bound.left)+','+(svg_bound.y - svg_g_bound.top)+')')
    ;
    svg
      .attr('width',svg_g_bound.right - svg_g_bound.left)
      .attr('height',svg_g_bound.bottom - svg_g_bound.top)
    ;
  }
  adjustSVG();
  

  //next button
  
  button_next.on('click',()=>{
    currentGraph++;
    generateCoordinate();
    updateGraph();
    adjustSVG();
  });
  button_back.on('click',()=>{
    currentGraph--;
    generateCoordinate();
    updateGraph();
    adjustSVG();
  });
}




// drawGraph();
/**
 * @param {SVGElement[]} drawnNode 
 * @param {link[]} links 
 * @param {d3.Selection<SVGSVGElement, any, HTMLElement, any>} svg_g 
 */
/* done */function __drawLinks(drawnNode,links,svg_g){
  console.log(svg_g.node());
  let svg_bound = svg_g.node().getBoundingClientRect();
  
  // links.forEach(link =>{  
  //   if(link.dst && link.dst) {
  //     let src_bound = drawnNode[link.src].getBoundingClientRect();
  //     let dst_bound = drawnNode[link.dst].getBoundingClientRect();

  //     svg_g.append('line')
  //       .attr('x1',(src_bound.left + src_bound.right)/2 - svg_bound.x)
  //       .attr('y1',src_bound.bottom - svg_bound.y)
  //       .attr('x2',(dst_bound.left + dst_bound.right)/2 - svg_bound.x)
  //       .attr('y2',dst_bound.top - svg_bound.y)
  //       .attr('stroke-width',1)
  //       .attr('stroke','black')
  //   } 
  // })

  
  let mylink = svg_g.selectAll('path')
    .data(links).enter()
    .append('path')
    .attr('stroke','black')
    .attr('fill','none')
    .attr('d',d3.linkVertical()
      .source((d)=>{ 
        let a;
        if(d.src === ""){
          a = drawnNode[d.dst].getBoundingClientRect();
          a.source = 0;

        } else {
          a = drawnNode[d.src].getBoundingClientRect();
          a.source = 1;
        }
        return a;
      })
      .target((d)=>{ 
        let a = drawnNode[d.dst].getBoundingClientRect();
        a.source = 0;
        return a;
      })
      .x((d)=>(d.left + d.right)/2 - svg_bound.x)
      .y((d)=>{
        if(d.source == 1) return d.bottom - svg_bound.y
        else return d.top - svg_bound.y;
        
      })
  
    )
  ;
  console.log(mylink);
  
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
 




