"use strict";
const assert = require("assert");
const Sciter = require("../sciter.js"); 

assert(Sciter);

const view = new Sciter.View();
view.loadFile(__dirname + "/main.htm");

view.expand();

Sciter.run();


