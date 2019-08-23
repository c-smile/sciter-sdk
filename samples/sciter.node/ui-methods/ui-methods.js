"use strict";
const assert = require("assert");
const Sciter = require("../sciter.js"); 

assert(Sciter);


const view = new Sciter.View();
view.loadFile(__dirname + "/ui-methods.htm");

view.call("sayHello","NodeJs"); // call function sayHello(toWhom) in ui-methods/test.htm

view.expand();

Sciter.run();


