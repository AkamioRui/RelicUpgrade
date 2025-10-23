const http = require('node:http');
const path = require('node:path');
const childProcess = require('node:child_process');
const fs = require('node:fs');


// import * as http from 'node:http';
// import * as path from 'node:path';
// import * as childProcess from 'node:child_process';
// import * as fs from 'node:fs';
// import * as url from 'node:url';
// const __filename = url.fileURLToPath(import.meta.url);
// const __dirname = path.dirname(__filename);


//param
const port = 8000;



//server
const server = http.createServer((req,res)=>{

    if(req.url == '/'){
        console.log('served html');
        
        let file = path.join(__dirname,'relicUpgradeGUI.html');
        let body = fs.readFileSync(file).toString();
        res.writeHead(200,{'content-type':'text/html'});
        res.end(body);

    } else if(req.url.match(/optionGUI/)){
        console.log('served option');
        let file = path.join(__dirname,req.url);
        try{
            // let body = fs.readFileSync(file).toString();
            res.writeHead(200,{'content-type':'text/javascript'});
            fs.createReadStream(file).pipe(res);
            // {
            //     '.html': 'text/html',
            //     '.js': 'text/javascript',
            //     '.css': 'text/css',
            //     '.json': 'application/json'
            //   }
            
            // res.end(body);
        } catch(e){
            console.log(`${req.url} doesnt exist`);
        }

    } else if(req.url.match(/calculate/)){
        console.log('calculating relic');

    } else {
        console.log(`${req.url} doesnt exist`);
        
        
    }

    /* 
curl http://localhost:8000/    
    
    */
});
server.listen(port,()=>console.log(`listening on ${port}`))