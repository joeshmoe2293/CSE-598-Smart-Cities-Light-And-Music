const http = require('http');

const requestListener = function(req, res) {
    if (req.url === '/nowHome') {
        res.writeHead(200);
        res.end('Welcome home! Turning on your lights now!\n');
    } else if (req.url === '/leftHome') {
        res.writeHead(200);
        res.end('We see you have left home, goodbye for now...\n')
    } else {
        res.writeHead(400);
        res.end('Unrecognized\n')
    }
}

const server = http.createServer(requestListener);
server.listen(8080);
