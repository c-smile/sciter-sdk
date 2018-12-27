
const assert = require("assert");
const Sciter = require(__dirname + "/../../bin/64/sciter"); // loading bin/64/sciter/sciter.node DLL 

assert(Sciter);

Sciter.run = function() {
  var imd;
  var pumpMessages = Sciter.pumpMessages;
  function messagePump() {
    if(!pumpMessages())
      clearImmediate(imd);
    else
      setImmediate(messagePump);
  }
  imd = setImmediate(messagePump);
};

module.exports = Sciter;


