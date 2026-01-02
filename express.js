const express = require('express');
const path = require('node:path');
const { exec, execFile } = require('node:child_process');

function asyncExec(program,args){
    return new Promise((resolve,reject)=>{

    })
}

//------------------------app----------------------

const app = express();
app.use(express.static(path.join(__dirname,'UI')))
app.get('/',(req,res)=>{
    res.sendFile(path.join(__dirname,'UI/relicUpgradeGUI.html'))
})
app.get('/calculate/:piece/:mainstat/_/*substat',(req,res)=>{
    const {piece,mainstat,substat}= req.params;
    
    const cProgram = path.join(__dirname,'cFile/relicUpgrade.exe');
    const args = ['destination', piece,mainstat,'threshold',...substat];

    //make it a promise
    for(let threshold = 1; threshold<=9; threshold++){
        args[0] = path.join(__dirname,'result',
            threshold+'.json')
        args[3] = threshold;
        
        execFile(cProgram,args,(err,stdout,stderr)=>{
            console.log({err,stdout,stderr});
        })
    }

    
    
    res.end(JSON.stringify(args));
})

//---------------------------------server
const http = require('node:http');



const server = http.createServer(app);
const port = 3000;
server.listen(port,()=>{console.log(`listening to ${port}`);});
