const express = require('express');
const path = require('node:path');
const { exec, execFile } = require('node:child_process');
const fs =  require('node:fs')


function asyncExecFile(program,args){
    return new Promise((resolve,reject)=>{
        execFile(program,args,(err,stdout,stderr)=>{
            if(err) reject({err,stderr,stdout})
            else resolve({stderr,stdout})
        })
    })
}

//------------------------app----------------------

const app = express();
app.use((req,res,next)=>{
    console.log('@@@@@@'+req.url);
    next();
})
app.use(express.static(path.join(__dirname,'UI')))
app.get('/',(req,res)=>{
    res.sendFile(path.join(__dirname,'UI/relicUpgradeGUI.html'))
})
app.get('/calculate/:piece/:mainstat/_/*substat', async(req,res)=>{
    const {piece,mainstat,substat}= req.params;
    
    const cProgram = path.join(__dirname,'cFile/relicUpgrade.exe');
    const args = ['destination', piece,mainstat,'threshold',...substat];

    res.header('Content-Type','application/json')
    res.write('[')
    for(let threshold = 1; threshold<=9; threshold++){
        args[0] = path.join(__dirname,'result',
            threshold+'.json')
        args[3] = threshold;

        // await asyncExecFile(cProgram,args)

        await new Promise((done,err)=>{
            fs.createReadStream(args[0])
                .on('end',done)
                .on('error',err)
                .pipe(res,{end:false})
        })
        
        
        if(threshold<9) res.write(',')
        
    }
    res.write(']')
    
    
    
    
    res.end();
})

//---------------------------------server
const http = require('node:http');



const server = http.createServer(app);
const port = 3000;
server.listen(port,()=>{console.log(`listening to ${port}`);});
