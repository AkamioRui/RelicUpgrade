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

const knownFile=[
    {name:'optionGUI.js','head':'text/javascript'},
    {name:'resultGUI.js','head':'text/javascript'},
    {name:'script.js','head':'text/javascript'},
    {name:'VarPlus.js','head':'text/javascript'},
]

//server
const server = http.createServer(async (req,res)=>{
    console.log(`---${req.url}`);

    if(req.url == '/'){
        console.log('   return index.html');
        
        let file = path.join(__dirname,'relicUpgradeGUI.html');
        let body = fs.readFileSync(file).toString();
        res.writeHead(200,{'content-type':'text/html'});
        res.end(body);
        
        console.log('\n');return;

    } 
    
    let found = knownFile.some(v=>{
        if(req.url.match(v.name)){
            console.log(`served ${v.name}`);

            let file = path.join(__dirname,req.url);
            try{
                res.writeHead(200,{'content-type':v.head});
                fs.createReadStream(file).pipe(res);
            } catch(e){
                console.log(`${req.url} doesnt exist`);
                console.log(e);
                
            }
            
            return true;
        } else return false;
        
    });
    if(found)return;


    if(req.url.match(/calculate/)){

        let program = path.join(__dirname,'cFile','relicUpgrade.exe');
        // let args = [path.join(__dirname,'result','mySTATE.json'), '2', '7', '5', '8', '4'];

        let [empty, reqName, ...params] = req.url.split('/');
        let args = [path.join(__dirname,'result','mySTATE.json'), ...params];
        
        console.log(args);

        async function sendAllFile(endIndex){
            res.writeHead(200,{'content-type':'application/json'});
            res.write('[\n');
            for(let i = 1; i<=endIndex; i++){
                
                
                await new Promise((resolve,reject)=>{
                    fs.createReadStream(path.join(__dirname,'result',`${i}.json`)) 
                    .on('end',resolve)
                    .on('error',reject)
                    .pipe(res,{end:false})
                })
    
                if(i<endIndex) res.write(',\n');
            }
            res.write(']\n');
            res.end();

        }

        callRelicUpgrade(1);
        function callRelicUpgrade(threshold){
            if(threshold > 9 ) {
                sendAllFile(9);
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


    } else console.log(`${req.url} doesnt exist`);
        
        


    /* 
curl http://localhost:8000/    
curl http://localhost:8000/calculate
    
    */
});
server.listen(port,()=>console.log(`listening on ${port}`))