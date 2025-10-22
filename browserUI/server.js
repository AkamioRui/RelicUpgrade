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
    let reqpath = req.url;

    if(reqpath == '/'){
        let file = path.join(__dirname,'relicUpgradeGUI.html');
        let body = fs.readFileSync(file).toString();
        res.end(body);
    }

    //curl http://localhost:8000/
});
server.listen(port,()=>console.log(`listening on ${port}`))