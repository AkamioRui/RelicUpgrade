
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


fetch("result/STATE.json").then(
  response=>{
    if(response.ok) return response.json();
    else return Promise.reject(
      new Error("generate the result file first using the code provided above")
    );
  }
).then(
  theJson => drawGraphD3_STATE(theJson)
)






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
    width : 370,
    height : 25,
    padding : 10,
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
  const linkGenerator = d3.linkHorizontal();
    linkGenerator  
    .source(d=>d.source)
    .target(d=>d.target)
    .x(d=>d.y*.680)
    .y(d=>d.x)
  ;
  /**
   * @param { d3.Selection<SVGPathElement, d3.HierarchyPointLink<TreeLink<STATELINK,STATENODE>>, SVGGElement, any>} myselection 
   */
  function initLink(myselection){
    myselection.each(function (d){
      let sourceClass = "from_"+d.source.id;
      let targetClass = "to_"+d.target.id;
      d3.select(this)
        .classed(sourceClass,true)
        .classed(targetClass,true);
    });

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
      .attr('text-anchor','start')
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
      .attr('transform',(d)=>
        'translate('
        + (Math.max(d.source.y,d.target.y)*(1-.680)
        -  Math.abs(d.source.y-d.target.y)*(1-.680)/2)
        +',0)'
      )
      .attr('stroke',color)
    ;
    let text = myselection.select('text')
      .attr('transform',function(d){
        
        if(d.target.data.nodeData.detail.indexOf('Good3')!=-1) {
          //Good3-%d:(%.6lf)
          let tgtIdx = d.target.data.nodeData.detail.match(/\d+/g)[1];
          //C%2d,%2d:%s-%s-%s
          let srcIdx = d.source.data.nodeData.detail.match(/\d+/)[0];
          

          
          if(srcIdx==tgtIdx) return 'translate('
          + (d.source.y+nodeSetting.width/2+5) +','
          + (d.source.x - 8) +')'
          else return 'translate('
          + (d.source.y+nodeSetting.width/2+5) +','
          + (d.source.x + 8) +')'
        }
      
        else if(d.source.data.nodeData.detail.indexOf('up')!=-1){
          return 'translate('
          + (d.source.y+d.target.y)/2 +','
          + (d.source.x+d.target.x)/2 +')'
        }
        
        else return 'translate('
        + (d.target.y-nodeSetting.width/2-70) +','
        + (d.target.x) +')'
      })
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
    selection.each(function (d){
      let idClass = "id_"+d.id;
      d3.select(this)
        .classed(idClass,true)
    });
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
      +(d.data.nodeData.price).toFixed(0)+ '/'
      +(d.data.nodeData.succesR).toFixed(4) + '='
      +(d.data.nodeData.succesR!=0?
        (d.data.nodeData.price/d.data.nodeData.succesR).toFixed(2) : "NAN")
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





