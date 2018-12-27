"use strict";
const assert = require("assert");
const Sciter = require("../sciter.js"); 

assert(Sciter);

const view = new Sciter.View();
view.loadFile(__dirname + "/node-methods.htm");

view.getGreeting = function(param) {
  return "Node: Hi " + param + "!";
}

view.expand();
Sciter.run();


