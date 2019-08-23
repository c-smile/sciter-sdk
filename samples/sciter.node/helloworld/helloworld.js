"use strict";

const assert = require("assert");
const Sciter = require("../sciter.js"); 

assert(Sciter);

const view = new Sciter.View(); // Create Sciter window

// load the html in it
view.loadHtml("<html style='background:gold'><body>Hello World!</body></html>");

// show the window
view.expand();

// start message pump loop
Sciter.run();

