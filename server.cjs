const http = require('node:http');
const path = require('node:path');
const childProcess = require('node:child_process');
const fs = require('node:fs');

//custom
function myExecfile(file,args=[]){
    return new Promise((resolve,reject)=>{
        childProcess.execFile(file,args,(err,stdout,stderr)=>{
            if(err)reject({err,stdout,stderr});
            else resolve({stdout,stderr});
        })    
    })
}

// import * as http from 'node:http';
// import * as path from 'node:path';
// import * as childProcess from 'node:child_process';
// import * as fs from 'node:fs';
// import * as url from 'node:url';
// const __filename = url.fileURLToPath(import.meta.url);
// const __dirname = path.dirname(__filename);

// {
//     '.html': 'text/html',
//     '.js': 'text/javascript',
//     '.css': 'text/css',
//     '.json': 'application/json'
//   }

//param
const port = 8000;



//server
const server = http.createServer((req,res)=>{
    console.log(`---${req.url}`);

    if(req.url == '/'){
        console.log('   return index.html');
        
        let file = path.join(__dirname,'relicUpgradeGUI.html');
        let body = fs.readFileSync(file).toString();
        res.writeHead(200,{'content-type':'text/html'});
        res.end(body);


    } else if(req.url.match(/optionGUI/)){
        console.log('   served option');
        let file = path.join(__dirname,req.url);
        try{
            res.writeHead(200,{'content-type':'text/javascript'});
            fs.createReadStream(file).pipe(res);
        } catch(e){
            console.log(`${req.url} doesnt exist`);
        }


    } else if(req.url.match(/resultGUI/)){
        console.log('   served result');
        let file = path.join(__dirname,req.url);
        try{
            res.writeHead(200,{'content-type':'text/javascript'});
            fs.createReadStream(file).pipe(res);
        
        } catch(e){
            console.log(`${req.url} doesnt exist`);
        }


    } else if(req.url.match(/VarPlus/)){
        console.log('   served VarPlus');
        let file = path.join(__dirname,req.url);
        try{
            res.writeHead(200,{'content-type':'text/javascript'});
            fs.createReadStream(file).pipe(res);
        } catch(e){
            console.log(`${req.url} doesnt exist`);
        }

    } else if(req.url.match(/calculate/)){

        let program = path.join(__dirname,'cFile','relicUpgrade.exe');
        let args = [path.join(__dirname,'result','mySTATE.json'), '2', '7', '5', '8', '4']
        callRelicUpgrade(1);

        function callRelicUpgrade(threshold){
            if(threshold > 9 ) {
                res.writeHead(200,{'content-type':'text/plain'});
                res.end('calculation done\n');
                return;
            }
            args[0] = path.join(__dirname,'result',`${threshold}.json`);
            args[3] = threshold.toString();

            myExecfile(program,args)
            .then(
                ({stdout,stderr})=>{
                    console.log('calculation complete');
                    console.log(stderr??'');

                    callRelicUpgrade(threshold+1);
                }
            ).catch(
                ({err,stdout,stderr})=>{
                    console.log('calculation error',err);
                    console.log(stdout??'');
                    console.log(stderr??'');
                    res.writeHead(500,{'content-type':'text/plain'});
                    res.end('calculation error\n');
                }
            )
        }


        console.log('calculating relic');
        
          
        //TODO, add destination as params to relicUpgrade.exe

    } else {
        console.log(`${req.url} doesnt exist`);
        
        
    }

    /* 
curl http://localhost:8000/    
curl http://localhost:8000/calculate
    
    */
});
server.listen(port,()=>console.log(`listening on ${port}`))