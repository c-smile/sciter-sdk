/*

  WebSocket echo server for the demo purposes

  Prerequisites:

    1. Install node.js and npm
    2. npm install ws

  See also,

    http://einaros.github.com/ws/

  To run,

    node server.js
*/

"use strict"; 
var WebSocketServer = require('ws').Server;
var http = require('http');

var server = http.createServer();
var wss = new WebSocketServer({server: server, path: '/echo'});
    wss.on('connection', function(ws) {
        console.log('/echo connected');
        ws.on('message', function(data, flags) {
            if (flags.binary) return; 
            
            if( data == "." ) { 
              ws.close(); 
              console.log('CLOSE rq received, closing');
              return; 
            } 
            
            console.log('received "' + data + '"');

            // changing data to indicate that it is processed by the server            
            data = "(((" + data + ")))";
            
            // sending it back
            console.log('sending "' + data + '"');
            ws.send(data);
        });
        ws.on('close', function() { console.log('Connection closed!');});
        ws.on('error', function(e) { });
    });
server.listen(8126);
console.log('Listening on port 8126...');
