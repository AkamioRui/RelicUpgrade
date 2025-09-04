
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