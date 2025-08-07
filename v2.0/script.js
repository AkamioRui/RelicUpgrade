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

/* 
json type is updated
also create a 
*/
drawGraphD3();
async function drawGraphD3(){

  //common element
  const all = d3.select('body').append('div');
  const backButton = all.append('button').text('back');
  const nextButton = all.append('button').text('next');
  all.append('br');
  const svg = all.append('svg');
  const svg_g = svg.append('g');



  //type definition  
  /** @typedef {{chance:number,price:number,succesR:number,detail:string,accept:number}} treeLinkData */
  /** @typedef {{id:number,parentId:any,data:treeLinkData}} treeLink */
  /** @typedef {{id:number,parentId:any,data:{chance:number}}} outerLink */

  


  //create some constant variable
  //#region 
  /** @type {{treeLinks:treeLink[],outerLinks:outerLink[]}[]} */
  const jsondata = await (await fetch('mydata.json')).json();
  const jsonDataLength = jsondata.length;
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
  //#endregion
  



  
  //resizeSVG
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
  //#region 
  /**
   * @param {d3.Selection<SVGPathElement, HierarchyPointLink<treeLink>, SVGGElement, any>} myselection 
   */
  /** @type {d3.Link< any, d3.HierarchyPointLink<treeLink>, d3.HierarchyNode<treeLink> >} */
  const linkGenerator = d3.linkVertical()
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
  function reRenderLinks(){
    linkElements.data(treeLinks, d=>d.source.id+','+d.target.id ).join(
        enter => enter
          .append('path')
          .call(adjustLink,'lime')  
        ,
        update => update.call(adjustLink,'black'),
        exit => exit.remove()
      );
  }
  //#endregion








  //draw node;
  //#region 
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

  //first time creating node G and its element
  //#region 
  const nodeElements = svg_g
    .append('g').attr('class','nodes')
    .selectAll('nodeElement')
    .data(treeNodes).enter()
    .append('g').attr('class','node')
    .call(initNode,'black')
  //#endregion

  //protocol to responding to new data;  
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
          this.oldData = d.data.data;
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
      // enter => enter.append('g').attr('class','node')
      // .call(initNode,'lime'),
      enter => enter.append('g').attr('class','node')
      .call(initNode,'lime'),
      update => update.call(updateNode,'aqua','black'),
      exit => exit.remove()
    )

  }
  //#endregion


  //appending function to the button
  function nextGraph(){
    currentGraph++;
    if(currentGraph >= jsonDataLength-1) nextButton.style('visibility','hidden');
    else nextButton.style('visibility','visible');
    if(currentGraph <= 0) backButton.style('visibility','hidden');
    else backButton.style('visibility','visible');
    
    getGraphData();
    resizeSVG();
    reRenderLinks();
    reRenderNodes();
  }
  nextButton.on('click',nextGraph);
  function previousGraph(){
    currentGraph--;
    if(currentGraph >= jsonDataLength-1) nextButton.style('visibility','hidden');
    else nextButton.style('visibility','visible');
    if(currentGraph <= 0) backButton.style('visibility','hidden');
    else backButton.style('visibility','visible');
    

    getGraphData();
    resizeSVG();
    reRenderLinks();
    reRenderNodes();
  }
  backButton.style('visibility','hidden');
  backButton.on('click',previousGraph);

}





