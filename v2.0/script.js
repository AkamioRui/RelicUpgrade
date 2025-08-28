
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



// drawGraphD3(jsondata,{width:20,height:20,padding:20});
drawGraphD3_STATE(await (await fetch("result/STATE.json")).json());
drawGraphD3(await (await fetch("result/HEAP.json")).json());
let backButton = d3.select("div > button.back");
let backButtons = d3.selectAll("div > button.back");
let backfuncs = [];
backButtons.each(function (){
  backfuncs.push( d3.select(this).on("click"));
});
backButton.on("click",()=>{
  backfuncs.forEach((value)=>{
    value();
  })  
})

let nextButton = d3.select("div > button.next");
let nextButtons = d3.selectAll("div > button.next");
let nextfuncs = [];
nextButtons.each(function (){
  nextfuncs.push( d3.select(this).on("click"));
});
nextButton.on("click",()=>{
  nextfuncs.forEach((value)=>{
    value();
  })  
})



//--------------code---------------------------------------------------------//


/**
 * @template _LinkData,_NodeData
 * @typedef {object} TreeLink
 * @property {number} id
 * @property {number} parentId
 * @property {_LinkData} linkData
 * @property {_NodeData} nodeData
 */ 
/**
 * @template _LinkData
 * @typedef {object} OuterLink
 * @property {number} id
 * @property {number} parentId
 * @property {_LinkData} linkData
 */ 

// /**
//  * @typedef {{treeLinks:TreeLink<any,any>[],outerLinks:OuterLink<any>[]}[]} GraphJson
//  * @typedef {label:string,jsonData:GraphJson} Graph
//  */

/**
 * 
 * @param {{treeLinks:TreeLink<any,any>[],outerLinks:OuterLink<any>[],msg:string}[]} jsondata 
 * @param {{width:number,height:number,padding:number}} nodeSetting 
 */
function drawGraphD3(jsondata,_nodeSetting = null){

  //common element
  const all = d3.select('body').append('div');
  const backButton = all.append('button').text('back').attr('class','back');
  const msgBox = all.append('p').attr('class','msgBox')
    .style('display','inline')
    .style('padding','0 10px')
  ;
  const nextButton = all.append('button').text('next').attr('class','next');
  all.append('br');
  const svg = all.append('svg');
  const svg_g = svg.append('g');
  
  

  //create some constant variable
  //#region 
  /** @type {{width:number,height:number,padding:number}} */
  let nodeSetting;
  if(!_nodeSetting){
    nodeSetting = {
      width : 110,
      height : Object.entries( jsondata[0].treeLinks[0].nodeData).length * 17,
      padding : 20,
    }
  } else nodeSetting = _nodeSetting;
  
  /** @type {d3.StratifyOperator<TreeLink<any,any>>} */
  const json2heirarchy = d3.stratify();
    json2heirarchy
    .id(d=>d.id)
    .parentId(d=>d.parentId)
  ;
  /** @type {d3.TreeLayout<TreeLink<any,any>>} */
  const treelayout = d3.tree()
    .nodeSize([nodeSetting.width+nodeSetting.padding*2,nodeSetting.height*2])
  ; 
  let currentGraph = 0;

  /** @type {(d3.HierarchyPointLink<TreeLink<any,any>>&{linkData:any})[]} */
  let treeLinks;
  /** @type {d3.HierarchyNode<TreeLink<any,any>>[]} */
  let treeNodes;
  function getGraphData(){
    let root = json2heirarchy(jsondata[currentGraph].treeLinks);
    treeLinks = treelayout(root).links();
    treeNodes = root.descendants();

    treeLinks.forEach((link)=>{
      link.linkData = link.target.data.linkData;
    })
    /** @type {d3.HierarchyNode<TreeLink<any, any>>[]} */
    let treeNodesMap = [];
    treeNodes.forEach((d)=>{
      treeNodesMap[d.id] = d;
    })

    let treeOuterLinks = jsondata[currentGraph].outerLinks.map(
      (v)=>{
        let a = {source:treeNodesMap[v.parentId],target:treeNodesMap[v.id]};
        a.linkData=v.linkData;
        return a;
      }
    )
    treeLinks.push(...treeOuterLinks);
    
    
  }
  getGraphData();
  
  //#endregion


  
  //resizeSVG
  function resizeSVG(){
    let xArr = treeNodes.map(d=>d.x);
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



  const linkDataProperty = Object.entries( jsondata[0].treeLinks[0].linkData).map(v=>v[0]);
  const nodeDataProperty = Object.entries( jsondata[0].treeLinks[0].nodeData).map(v=>v[0]);
  //drawLinks
  //#region 
  /** @type {d3.Link< any, d3.HierarchyPointLink<TreeLink<any,any>, d3.HierarchyNode<TreeLink<any,any> >} */
  const linkGenerator = d3.linkVertical()
    .source(d=>d.source)
    .target(d=>d.target)
    .x(d=>d.x)
    .y(d=>d.y)
  ;
  /**
   * @param { d3.Selection<SVGPathElement, d3.HierarchyPointLink<TreeLink<any,any>>, SVGGElement, any>} myselection 
   */
  function initLink(myselection){
    let path = myselection.append('path') 
      // .attr('d',linkGenerator)
      .attr('fill',"none")
      // .attr('stroke',color)
    ;
    let text = myselection.append('text')
      // .attr('transform',d=> 'translate('
      //   + ((d.source.x*3+d.target.x*2)/5) +','
      //   + ((d.source.y*3+d.target.y*2)/5) +')'
      // )
      .attr('dominant-baseline','middle')
      .attr('text-anchor','middle')
    ;
    linkDataProperty.forEach((property,index)=>{
      text.append('tspan').attr('class',property)
      // .text(d=>d.linkData[property])
      .attr('y',(index - (linkDataProperty.length-1)/2)*0.9 +'em')
      .attr('x',0);
    });
    
  }
  /**
   * @param { d3.Selection<SVGPathElement, d3.HierarchyPointLink<TreeLink<any,any>>, SVGGElement, any>} myselection 
   * @param {string} color 
   */
  function updateLink(myselection,color){
    let path = myselection.select('path') 
      .attr('d',linkGenerator)
      .attr('stroke',color)
    ;
    let text = myselection.select('text')
      .attr('transform',d=> 'translate('
        + ((d.source.x*3+d.target.x*2)/5) +','
        + ((d.source.y*3+d.target.y*2)/5) +')'
      )
    ;
    linkDataProperty.forEach(property=>{
      text.select('tspan.'+property)
      .text(d=>d.linkData[property])
    });
    
  }
  

  let linkElements = svg_g
    .append('g').attr('class','links')
    .selectAll('linkElement')
    .data(treeLinks).enter()
    .append('g').attr('class','link') 
    .call(initLink)
    .call(updateLink,'black')
  ;
  
  ///*  */3 gone, 5 edited, 6 added
  

  //#endregion








  //draw node;
  //#region 
  /** @typedef {SVGGElement & {oldNodeData:any,modified:number}} SVGNodeElement */
  /**
   * @param { d3.Selection<SVGGElement, d3.HierarchyNode<TreeLink<any,any>>, SVGGElement, any>} selection 
   */
  function initNode(selection){
    // selection
      // .attr('transform', d => 'translate('+d.x+','+d.y+')')
      // .each(function (d){
      //   this.oldNodeData = d.data.nodeData;
      // })
    selection.append('rect')
      .attr('x'       ,-nodeSetting.width/2)
      .attr('y'       ,-nodeSetting.height/2)
      .attr('width'   ,nodeSetting.width)
      .attr('height'  ,nodeSetting.height)
      .attr('fill','white')
      // .attr('stroke',color)

    let text = selection.append('text')
      .attr('dominant-baseline','middle')
      .attr('text-anchor','middle')
    ;
    nodeDataProperty.forEach((property,index)=>{
      text.append('tspan').attr('class',property)
      .text(d=>d.data.nodeData[property])
      .attr('y',(index - (nodeDataProperty.length-1)/2)*0.9 +'em')
      .attr('x',0);
    });
  }
  /**
     * @param { d3.Selection<SVGNodeElement , d3.HierarchyNode<TreeLink<any,any>>, SVGGElement, any> } selection 
     * @param {string} colorModiefied 
     * @param {string} colorStable 
     */
  function updateNode(selection,colorModiefied,colorStable){
    selection
      .attr('transform', d => 'translate('+d.x+','+d.y+')')
      .each(function (d){
        this.modified = 0;
        let newNodeData = d.data.nodeData;
        if(this.oldNodeData !== undefined){
          for(let v of Object.entries(this.oldNodeData)){
            if( v[1] === newNodeData[v[0]]) continue;
            this.modified = 1;
            break;
          }
        }
        this.oldNodeData = newNodeData;
      })
    ;
    selection.select('rect')
      .attr('stroke',function (){
        
        if(this.parentNode.modified)return colorModiefied;
        else return colorStable;
      })
    ;
    let text = selection.select('text');
    nodeDataProperty.forEach((property)=>{
      text.select('tspan.'+property)
      .text(d=>d.data.nodeData[property])
    });
   
    
  }

  //first time creating node G and its element
  //#region 
  let nodeElements = svg_g.append('g').attr('class','nodes')
    .selectAll('nodeElement')
    .data(treeNodes).enter()
    .append('g').attr('class','node')
    .call(initNode,'black')
    .call(updateNode,'black','black')
  ;
  //#endregion

  //protocol to responding to new data;  
  

  //#endregion


  //appending function to the button
  const jsonDataLength = jsondata.length;
  function nextGraph(increment){
    
    currentGraph += increment;
    currentGraph = Math.min(Math.max(currentGraph, 0), jsonDataLength-1);

    //update appearance
    nextButton.style('visibility',()=>{
      if(currentGraph == jsonDataLength-1) return 'hidden';
      else return 'visible';
    });
    backButton.style('visibility',()=>{
      if(currentGraph == 0) return 'hidden';
      else return 'visible';
    });
    msgBox.text(jsondata[currentGraph].msg);
    
    //update the actuall graph
    getGraphData();
    resizeSVG();
    
    
    linkElements = linkElements.data(treeLinks, function (d){
      return d.source.id+','+d.target.id;
    } ).join(//normal: black, enter:green
      enter =>{
        return enter
        .append('g').attr('class','link') 
        .call(initLink)
        .call(updateLink,'lime');
      },
      update => {
        return update
        .call(updateLink,'black');
      },
      exit => {
        return exit.remove();
      }
    );
    

    
    nodeElements = nodeElements.data(treeNodes,d=>d.id)
    .join(//normal:black, new:lime, updated:aqua
      enter => {
        return enter
        .append('g').attr('class','node')
        .call(initNode)
        .call(updateNode,'lime','lime')
      },
      update => {
        return update
        .call(updateNode,'aqua','black')
      },
      exit => {
        return exit.remove();
      }
    )
    
  }
  nextButton.on('click',()=>nextGraph(1))
    .style('visibility',()=>{
      if(jsonDataLength < 2) return 'hidden';
    });
  backButton.on('click',()=>nextGraph(-1))
    .style('visibility','hidden');
  msgBox.text(jsondata[currentGraph].msg);

}


/**  
 * @typedef {object} STATENODE
 * @property {string} detail
 * @property {number} price
 * @property {number} succesR
 * @property {number} accept
 * @property {number} inHeap
 * */ 
/**  
 * @typedef {object} STATELINK
 * @property {number} chance
 * */ 
/**
 * 
 * @param {{treeLinks:TreeLink<STATELINK,STATENODE>[],outerLinks:OuterLink<any>[],msg:string}[]} jsondata 
 * @param {{width:number,height:number,padding:number}} nodeSetting 
 */
function drawGraphD3_STATE(jsondata){

  //common element
  const all = d3.select('body').append('div');
  const backButton = all.append('button').text('back').attr('class','back');
  const msgBox = all.append('p').attr('class','msgBox')
    .style('display','inline')
    .style('padding','0 10px')
  ;
  const nextButton = all.append('button').text('next').attr('class','next');
  all.append('br');
  const svg = all.append('svg');
  const svg_g = svg.append('g');
  
  

  //create some constant variable
  //#region 
  /** @type {{width:number,height:number,padding:number,fill:string[]}} */
  let nodeSetting;
  nodeSetting = {
    width : 210,
    height : 1 * 20,
    padding : 2,
    fill: ["white","yellow","lime"]
  }
  
  
  /** @type {d3.StratifyOperator<TreeLink<STATELINK,STATENODE>>} */
  const json2heirarchy = d3.stratify();
    json2heirarchy
    .id(d=>d.id)
    .parentId(d=>d.parentId)
  ;
  /** @type {d3.TreeLayout<TreeLink<STATELINK,STATENODE>>} */
  const treelayout = d3.tree()
    .nodeSize([nodeSetting.height+nodeSetting.padding*2,nodeSetting.width*3])
  ; 
  let currentGraph = 0;

  /** @type {(d3.HierarchyPointLink<TreeLink<STATELINK,STATENODE>>&{linkData:any})[]} */
  let treeLinks;
  /** @type {d3.HierarchyNode<TreeLink<STATELINK,STATENODE>>[]} */
  let treeNodes;
  function getGraphData(){
    let root = json2heirarchy(jsondata[currentGraph].treeLinks);
    treeLinks = treelayout(root).links();
    treeNodes = root.descendants();

    treeLinks.forEach((link)=>{
      link.linkData = link.target.data.linkData;
    })
    /** @type {d3.HierarchyNode<TreeLink<any, any>>[]} */
    let treeNodesMap = [];
    treeNodes.forEach((d)=>{
      treeNodesMap[d.id] = d;
    })

    let treeOuterLinks = jsondata[currentGraph].outerLinks.map(
      (v)=>{
        let a = {source:treeNodesMap[v.parentId],target:treeNodesMap[v.id]};
        a.linkData=v.linkData;
        return a;
      }
    )
    treeLinks.push(...treeOuterLinks);
    
    
  }
  getGraphData();
  
  //#endregion


  
  //resizeSVG
  function resizeSVG(){
    // let xArr = treeNodes.map(d=>d.x);
    // let minX = Math.min(...xArr);
    // let maxX = Math.max(...xArr);
    // let yArr = treeNodes.map(d=>d.y);
    let yArr = treeNodes.map(d=>d.x);//flipped image
    let xArr = treeNodes.map(d=>d.y);//flipped image
    let minX = Math.min(...xArr);
    let maxX = Math.max(...xArr);
    let minY = Math.min(...yArr);
    let maxY = Math.max(...yArr);

    svg_g.attr('transform','translate('
      +(minX*(-1) + nodeSetting.width/2)+
      ','
      +(minY*(-1) + nodeSetting.height/2)+
      ')')
    ;

    svg
    .attr('width',nodeSetting.width + maxX-minX+1 + 10)
    .attr('height',nodeSetting.height + maxY-minY+1)
  }
  resizeSVG();



  const linkDataProperty = Object.entries( jsondata[0].treeLinks[0].linkData).map(v=>v[0]);
  // const nodeDataProperty = Object.entries( jsondata[0].treeLinks[0].nodeData).map(v=>v[0]);
  const nodeDataProperty = ["detail","price","succesR","accept"];
  //drawLinks
  //#region 
  /** @type {d3.Link< any, d3.HierarchyPointLink<TreeLink<STATELINK,STATENODE>, d3.HierarchyNode<TreeLink<STATELINK,STATENODE> >} */
  const linkGenerator = d3.linkHorizontal()
    .source(d=>d.source)
    .target(d=>d.target)
    .x(d=>d.y-nodeSetting.width/2)
    .y(d=>d.x)
  ;
  /**
   * @param { d3.Selection<SVGPathElement, d3.HierarchyPointLink<TreeLink<STATELINK,STATENODE>>, SVGGElement, any>} myselection 
   */
  function initLink(myselection){
    let path = myselection.append('path') 
      // .attr('d',linkGenerator)
      .attr('fill',"none")
      // .attr('stroke',color)
    ;
    let text = myselection.append('text')
      // .attr('transform',d=> 'translate('
      //   + ((d.source.x*3+d.target.x*2)/5) +','
      //   + ((d.source.y*3+d.target.y*2)/5) +')'
      // )
      .attr('dominant-baseline','middle')
      .attr('text-anchor','middle')
    ;
    linkDataProperty.forEach((property,index)=>{
      text.append('tspan').attr('class',property)
      // .text(d=>d.linkData[property])
      .attr('y',(index - (linkDataProperty.length-1)/2)*0.9 +'em')
      .attr('x',0);
    });
    
  }
  /**
   * @param { d3.Selection<SVGPathElement, d3.HierarchyPointLink<TreeLink<STATELINK,STATENODE>>, SVGGElement, any>} myselection 
   * @param {string} color 
   */
  function updateLink(myselection,color){
    let path = myselection.select('path') 
      .attr('d',linkGenerator)
      .attr('stroke',color)
    ;
    let text = myselection.select('text')
      .attr('transform',d=> 'translate('
        + (d.target.y-nodeSetting.width/2-40) +','
        + (d.target.x) +')'
      )
    ;
    linkDataProperty.forEach(property=>{
      text.select('tspan.'+property)
      .text(d=>d.linkData[property])
    });
    
  }
  

  let linkElements = svg_g
    .append('g').attr('class','links')
    .selectAll('linkElement')
    .data(treeLinks).enter()
    .append('g').attr('class','link') 
    .call(initLink)
    .call(updateLink,'black')
  ;
  
  ///*  */3 gone, 5 edited, 6 added
  

  //#endregion








  //draw node;
  //#region 
  /** @typedef {SVGGElement & {oldNodeData:any,modified:number}} SVGNodeElement */
  /**
   * @param { d3.Selection<SVGGElement, d3.HierarchyNode<TreeLink<STATELINK,STATENODE>>, SVGGElement, any>} selection 
   */
  function initNode(selection){
    // selection
      // .attr('transform', d => 'translate('+d.x+','+d.y+')')
      // .each(function (d){
      //   this.oldNodeData = d.data.nodeData;
      // })
    selection.append('rect')
      .attr('x'       ,-nodeSetting.width/2)
      .attr('y'       ,-nodeSetting.height/2)
      .attr('width'   ,nodeSetting.width)
      .attr('height'  ,nodeSetting.height)
      // .attr('fill','white')
      // .attr('stroke',color)
      .attr('stroke-width',3)

    let text = selection.append('text')
      .attr('dominant-baseline','middle')
      .attr('stroke','black')
      .attr('stroke-width',0.3)
    ;
      text.append('tspan').attr('class','label')
      .attr('x'       ,-nodeSetting.width/2+5)
      .attr('text-anchor','start')

      text.append('tspan').attr('class','number')
      .attr('x'       ,nodeSetting.width/2-5)
      .attr('text-anchor','end')
      // .attr('text-anchor','middle')
    
    // nodeDataProperty.forEach((property,index)=>{
    //   text.append('tspan').attr('class',property)
    //   .text(d=>d.data.nodeData[property])
    //   .attr('y',(index - (nodeDataProperty.length-1)/2)*0.9 +'em')
    //   .attr('x',0);
    // });
  }
  /**
     * @param { d3.Selection<SVGNodeElement , d3.HierarchyNode<TreeLink<STATELINK,STATENODE>>, SVGGElement, any> } selection 
     * @param {string} colorModiefied 
     * @param {string} colorStable 
     */
  function updateNode(selection,colorModiefied,colorStable){
    selection
      .attr('transform', d => 'translate('+d.y+','+d.x+')')
      .each(function (d){
        this.modified = 0;
        let newNodeData = d.data.nodeData;
        if(this.oldNodeData !== undefined){
          for(let v of Object.entries(this.oldNodeData)){
            if( v[1] === newNodeData[v[0]]) continue;
            this.modified = 1;
            break;
          }
        }
        this.oldNodeData = newNodeData;
      })
    ;
    selection.select('rect')
      .attr('fill',(d)=>{
        //[unknown,inheap,whitelisted]
        if(d.data.nodeData.accept)return nodeSetting.fill[2];
        else if(d.data.nodeData.inHeap)return nodeSetting.fill[1];
        else return nodeSetting.fill[0];
      })
      .attr('stroke',function (){
        
        if(this.parentNode.modified)return colorModiefied;
        else return colorStable;
      })
    ;
    // /** @type {STATENODE} */
    let text = selection.select('text');
    text.select('tspan.label').text((d)=>'>'
      + d.data.nodeData.detail + ':  '
    );
    
    text.select('tspan.number').text((d)=>''
      +(d.data.nodeData.succesR).toFixed(4) + '/'
      +(d.data.nodeData.price).toFixed(0) + '='
      +(d.data.nodeData.price!=0?
        (d.data.nodeData.succesR/d.data.nodeData.price*1000000).toFixed(2) : "NAN")
    );
    // text.text((d)=>'>'
    //   + d.data.nodeData.detail + ':  '
    //   +(d.data.nodeData.succesR).toFixed(4) + '/'
    //   +(d.data.nodeData.price).toFixed(0) + '='
    //   +(d.data.nodeData.price!=0?
    //     (d.data.nodeData.succesR/d.data.nodeData.price*1000000).toFixed(2) : "NAN")
    // );

    
  }

  //first time creating node G and its element
  //#region 
  let nodeElements = svg_g.append('g').attr('class','nodes')
    .selectAll('nodeElement')
    .data(treeNodes).enter()
    .append('g').attr('class','node')
    .call(initNode,'black')
    .call(updateNode,'black','black')
  ;
  //#endregion

  //protocol to responding to new data;  
  

  //#endregion


  //appending function to the button
  const jsonDataLength = jsondata.length;
  function nextGraph(increment){
    
    currentGraph += increment;
    currentGraph = Math.min(Math.max(currentGraph, 0), jsonDataLength-1);

    //update appearance
    nextButton.style('visibility',()=>{
      if(currentGraph == jsonDataLength-1) return 'hidden';
      else return 'visible';
    });
    backButton.style('visibility',()=>{
      if(currentGraph == 0) return 'hidden';
      else return 'visible';
    });
    msgBox.text(jsondata[currentGraph].msg);
    
    //update the actuall graph
    getGraphData();
    resizeSVG();
    
    
    linkElements = linkElements.data(treeLinks, function (d){
      return d.source.id+','+d.target.id;
    } ).join(//normal: black, enter:green
      enter =>{
        return enter
        .append('g').attr('class','link') 
        .call(initLink)
        .call(updateLink,'lime');
      },
      update => {
        return update
        .call(updateLink,'black');
      },
      exit => {
        return exit.remove();
      }
    );
    

    
    nodeElements = nodeElements.data(treeNodes,d=>d.id)
    .join(//normal:black, new:lime, updated:aqua
      enter => {
        return enter
        .append('g').attr('class','node')
        .call(initNode)
        .call(updateNode,'lime','lime')
      },
      update => {
        return update
        .call(updateNode,'aqua','black')
      },
      exit => {
        return exit.remove();
      }
    )
    
  }
  nextButton.on('click',()=>nextGraph(1))
    .style('visibility',()=>{
      if(jsonDataLength < 2) return 'hidden';
    });
  backButton.on('click',()=>nextGraph(-1))
    .style('visibility','hidden');
  msgBox.text(jsondata[currentGraph].msg);

}


// drawGraphD3_relic();
async function drawGraphD3_relic(){

  //common element
  const all = d3.select('body').append('div');
  const backButton = all.append('button').text('back');
  const nextButton = all.append('button').text('next');
  all.append('br');
  const svg = all.append('svg');
  const svg_g = svg.append('g');
  
  

  //create some constant variable
  //#region 
  /** @type {{width:number,height:number,padding:number,fill:string[]}} */
  let nodeSetting = {
    width : 110,
    height : ["detail","price","succesR"].length * 17,
    padding : 20,
    fill:["red","white"],
  };
  /** @type {{treeLinks:TreeLink<any,any>[],outerLinks:OuterLink<any>[]}[]} */
  const jsondata = await (await fetch("mydata.json")).json();
  
  
  
  /** @type {d3.StratifyOperator<TreeLink<any,any>>} */
  const json2heirarchy = d3.stratify();
    json2heirarchy
    .id(d=>d.id)
    .parentId(d=>d.parentId)
  ;
  /** @type {d3.TreeLayout<TreeLink<any,any>>} */
  const treelayout = d3.tree()
    .nodeSize([nodeSetting.width+nodeSetting.padding*2,nodeSetting.height*2])
  ; 
  let currentGraph = 0;
  /** @type {(d3.HierarchyPointLink<TreeLink<any,any>>&{linkData:any})[]} */
  let treeLinks;
  /** @type {d3.HierarchyNode<TreeLink<any,any>>[]} */
  let treeNodes;
  function getGraphData(){
    let root = json2heirarchy(jsondata[currentGraph].treeLinks);
    treeLinks = treelayout(root).links();
    treeNodes = root.descendants();

    treeLinks.forEach((link)=>{
      link.linkData = link.target.data.linkData;
    })
    /** @type {d3.HierarchyNode<TreeLink<any, any>>[]} */
    let treeNodesMap = [];
    treeNodes.forEach((d)=>{
      treeNodesMap[d.id] = d;
    })

    let treeOuterLinks = jsondata[currentGraph].outerLinks.map(
      (v)=>{
        let a = {source:treeNodesMap[v.parentId],target:treeNodesMap[v.id]};
        a.linkData=v.linkData;
        return a;
      }
    )
    treeLinks.push(...treeOuterLinks);
  }
  getGraphData();
  //#endregion


  
  //resizeSVG
  function resizeSVG(){
    let xArr = treeNodes.map(d=>d.x);
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
  //#region 
  /**
   * @param {d3.Selection<SVGPathElement, HierarchyPointLink<treeLink>, SVGGElement, any>} myselection 
   */
  /** @type {d3.Link< any, d3.HierarchyPointLink<TreeLink<any,any>, d3.HierarchyNode<TreeLink<any,any> >} */
  const linkGenerator = d3.linkVertical()
    .source(d=>d.source)
    .target(d=>d.target)
    .x(d=>d.x)
    .y(d=>d.y)
  ;
  const linkDataProperty = Object.entries( jsondata[0].treeLinks[0].linkData).map(v=>v[0]);
  const nodeDataProperty = ["detail","price","succesR"];
  /**
   * @param { d3.Selection<SVGPathElement, d3.HierarchyPointLink<TreeLink<any,any>>, SVGGElement, any>} myselection 
   */
  function initLink(myselection){
    let path = myselection.append('path') 
      // .attr('d',linkGenerator)
      .attr('fill',"none")
      // .attr('stroke',color)
    ;
    let text = myselection.append('text')
      // .attr('transform',d=> 'translate('
      //   + ((d.source.x*3+d.target.x*2)/5) +','
      //   + ((d.source.y*3+d.target.y*2)/5) +')'
      // )
      .attr('dominant-baseline','middle')
      .attr('text-anchor','middle')
    ;
    linkDataProperty.forEach((property,index)=>{
      text.append('tspan').attr('class',property)
      // .text(d=>d.linkData[property])
      .attr('y',(index - (linkDataProperty.length-1)/2)*0.9 +'em')
      .attr('x',0);
    });
    
  }
  /**
   * @param { d3.Selection<SVGPathElement, d3.HierarchyPointLink<TreeLink<any,any>>, SVGGElement, any>} myselection 
   * @param {string} color 
   */
  function updateLink(myselection,color){
    let path = myselection.select('path') 
      .attr('d',linkGenerator)
      .attr('stroke',color)
    ;
    let text = myselection.select('text')
      .attr('transform',d=> 'translate('
        + ((d.source.x*3+d.target.x*2)/5) +','
        + ((d.source.y*3+d.target.y*2)/5) +')'
      )
    ;
    linkDataProperty.forEach(property=>{
      text.select('tspan.'+property)
      .text(d=>d.linkData[property])
    });
    
  }

  let linkElements = svg_g
    .append('g').attr('class','links')
    .selectAll('linkElement')
    .data(treeLinks).enter()
    .append('g').attr('class','link') 
    .call(initLink)
    .call(updateLink,'black')
  ;

  //#endregion




  //draw node;
  //#region 
  /**
   * @param { d3.Selection<SVGGElement, d3.HierarchyNode<TreeLink<any,any>>, SVGGElement, any>} selection 
   */
  function initNode(selection){
    // selection
      // .attr('transform', d => 'translate('+d.x+','+d.y+')')
      // .each(function (d){
      //   this.oldData = d.data.nodeData;
      // })
    selection.append('rect')
      .attr('x'       ,-nodeSetting.width/2)
      .attr('y'       ,-nodeSetting.height/2)
      .attr('width'   ,nodeSetting.width)
      .attr('height'  ,nodeSetting.height)
      // .attr('fill','white')
      // .attr('stroke',color)

    let text = selection.append('text')
      .attr('dominant-baseline','middle')
      .attr('text-anchor','middle')
    ;
    nodeDataProperty.forEach((property,index)=>{
      text.append('tspan').attr('class',property)
      .text(d=>d.data.nodeData[property])
      .attr('y',(index - (nodeDataProperty.length-1)/2)*0.9 +'em')
      .attr('x',0);
    });
  }
  /**
     * @param { d3.Selection<SVGGElement , d3.HierarchyNode<TreeLink<any,any>>, SVGGElement, any> } selection 
     * @param {string} colorModiefied 
     * @param {string} colorStable 
     */
  function updateNode(selection,colorModiefied,colorStable){
    selection
      .attr('transform', d => 'translate('+d.x+','+d.y+')')
      .each(function (d){
        this.modified = 0;
        let newNodeData = d.data.nodeData;
        if(this.oldData !== undefined){
          for(let v of Object.entries(this.oldData)){
            if( v[1] === newNodeData[v[0]]) continue;
            this.modified = 1;
            break;
          }
        }
        this.oldData = newNodeData;
      })
    ;
    selection.select('rect')
      .attr('stroke',function (){
        if(this.parentNode.modified)return colorModiefied;
        else return colorStable;
      })
      // .attr('fill',d=>nodeSetting.fill[d.data.nodeData.accept])
      .attr('fill',d=>nodeSetting.fill[d.data.nodeData.accept])
    ;
    let text = selection.select('text');
    nodeDataProperty.forEach((property)=>{
      text.select('tspan.'+property)
      .text(d=>d.data.nodeData[property])
    });
   
    
  }

  //first time creating node G and its element
  //#region 
  let nodeElements = svg_g.append('g').attr('class','nodes')
    .selectAll('nodeElement')
    .data(treeNodes).enter()
    .append('g').attr('class','node')
    .call(initNode,'black')
    .call(updateNode,'black','black')
  //#endregion

  //protocol to responding to new data;  
  

  //#endregion


  //appending function to the button
  const jsonDataLength = jsondata.length;
  function nextGraph(increment){
    currentGraph += increment;
    if(currentGraph >= jsonDataLength-1) nextButton.style('visibility','hidden');
    else nextButton.style('visibility','visible');
    if(currentGraph <= 0) backButton.style('visibility','hidden');
    else backButton.style('visibility','visible');
    
    getGraphData();
    resizeSVG();
    //normal: black, enter:green
    linkElements = linkElements.data(treeLinks, d=>d.source.id+','+d.target.id ).join(
      enter => enter
        .append('g').attr('class','link') 
        .call(initLink)
        .call(updateLink,'lime')
      ,
      update => update
        .call(updateLink,'black')
      ,
      exit => exit.remove()
    );

    //normal:black, new:lime, updated:aqua
    nodeElements = nodeElements.data(treeNodes,d=>d.id).join(
      enter => enter
        .append('g').attr('class','node')
        .call(initNode)
        .call(updateNode,'lime','lime')
      ,
      update => update
        .call(updateNode,'aqua','black')
      ,
      exit => exit.remove()
    )
  }
  nextButton.on('click',()=>nextGraph(1));
  backButton.on('click',()=>nextGraph(-1));
  backButton.style('visibility','hidden');

}




