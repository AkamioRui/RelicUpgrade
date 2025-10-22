const http = require('node:http');
const path = require('node:path');
const childProcess = require('node:child_process');
const fs = require('node:fs');


// import * as http from 'node:http';
// import * as path from 'node:path';
// import * as childProcess from 'node:child_process';
// import * as fs from 'node:fs';


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

    } else if(req.url.match(/option/)){
        console.log('served option');
        let file = path.join(__dirname,req.url);
        try{
            let body = fs.readFileSync(file).toString();
            res.writeHead(200,{'content-type':'application/javascript'});
            res.end(body);
        } catch(e){
            console.log(`${req.url} doesnt exist`);
        }

    } else {
        console.log(`${req.url} doesnt exist`);
        
        
    }

    /* 
curl http://localhost:8000/    
    
    */
});
server.listen(port,()=>console.log(`listening on ${port}`))