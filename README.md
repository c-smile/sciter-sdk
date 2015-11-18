# sciter-sdk

[Sciter Engine](http://sciter.com) SDK distribution by [Terra Informatica Software](http://terrainformatica.com) - multiplatform embedeable HTML/CSS/script UI engine.

![Sciter Clock demo on Mac,Win,Lin](http://sciter.com/images/sciter-clock.png)

Sciter uses Plain C and C++ API for embedding it into native desktop applications on various desktop OSes. 

Other programming language bindings and runtimes:

*   [Sciter Delphi 7 wrapper on GitHub](https://github.com/da-baranov/SciDe) by Mr. Baranov D.A.
*   [D language Sciter SDK port](https://github.com/midiway/sciter-dport) on GitHub by [Mr. Ramon F. Mendes](http://misoftware.rs/)
*   [Go language Sciter SDK binding](https://github.com/datagoes/sciter) on GitHub by Oskca
*   [.NET and C# Sciter bindings](https://github.com/midiway/SciterSharp) on GitHub by  [Mr. Ramon F. Mendes](http://misoftware.rs/)


## Sciter SDK contains the following folders:

*   **bin**, **bin.osx** and **bin.gtk** - folders with the compiled Sciter engine: sciter32/64.dll (Windows), sciter-osx-64.dylib (OS X), sciter-gtk-64.so (Linux) and sciter.exe variations - demo "browser" with builtin DOM inspector, script debugger and Sciter documentation browser (see screenshot above). There are compiled versions of samples from the demo folder.
*   **include** - C and C++ include files that define the public Sciter engine API: window level functions, DOM access methods and utilities.
*   **demos**, **demos.osx**, **demos.gtk** - demo projects/applications demonstrating various aspects of Sciter embedding.
*   **doc** - documentation in HTML format, viewable in conventional browsers and the built-in help viewer.
*   **samples** - HTML/CSS/script ready to use code snippets and samples demonstrating various Sciter features.

### Libraries and samples included in the public Sciter SDK

*   **samples/+plus** - that is _AngularJS_ alike data-binding library. Small (480 LOC) and non-intrusive Model-View-Whatever library.
*   **samples/+lib** - _underscore.js_ alike primitives.
*   **samples/+promise** - _Promises/A+_ specification implementation.
*   **samples/+query** - port of basic _jQuery/Zepto_ features. Most of the basic jQuery features are implemented natively in Sciter, so this library is quite compact - 700 LOC.
*   **samples/+lang** - i18n primitives.
*   **samples/+vlist** - _virtual list,_ _grid_ library, and samples. Use it when you need to browse large sets of records. +vlist uses a live data binding mechanism. Just provide an array[] of records and a AngularJS alike repeatable template.
*   **samples/animations** - library and demos of animation framework, similar to GreenSock.js animation platform (GSAP).
*   **samples/animated-png** - [Animated PNG](https://en.wikipedia.org/wiki/APNG) demos.
*   **samples/animations-transitions-css** - CSS based transitions. Historically, Sciter uses slightly different syntax of defining CSS transitions, but the feature set is similar to the CSS3 Transitions module.
*   **samples/basics** - basic CSS samples, including the CSS3 _transform_ property.
*   **samples/communication** - AJAX/JSON client, WebSockets and DataSockets duplex inter/intra-net communication.
*   **samples/css++** - demonstration of various CSS extnesions introduced in Sciter.
*   **samples/dialogs+windows** - demonstation of View.window, View.dialog and View.msgbox features: desktop windows defined by HTML/CSS/script means.
*   **samples/drag-n-drop-manager** - drag-n-drop manager.
*   **samples/effects.css++** - transition:blend and transition: slide-xxx demos - Sciter specific transition extensions.
*   **samples/font-@-awesome** - CSS3 @font-face feature demo using FontAwesome integration.
*   **samples/forms** - demonstration of Sciter extended set of `<input>` widgets including `<select type=tree>`, `<input type=number>`, `<input type=masked>` and many others.
*   **samples/goodies** - Sciter extras, including `behavior:file-icon` - shell icons rendering.
*   **samples/graphics** - use of _Graphics_ class - immediate and buffered drawing primitives including "render-element-to-bitmap" and "dynamic-CSS-background-image" features, Graphics in Sciter is a superset of <canvas> functionality in browsers.
*   **samples/ideas/** - bunch of implementation ideas including:![Virtual list](http://www.terrainformatica.com/w3/virtual-list.png)

*   callout - dynamic callout
*   carousel   
*   [KiTE - {{mustache}} alike template engine](https://code.google.com/p/kite).
*   lightbox-dialog - lightbox in-window modal dialogs.
*   moveable-windows - Sciter supports so called _airborn DOM elements_ - elements rendered in separate windows. The sample demonstrates this feature.
*   tray-notifications - shell tray notifications defined by HTML/CSS means.
*   virtual-list - yet another virtual list with kinetic scroll that supports unlimited number of items of variable height (see image on the right).

*   **samples/image-map** - "image catalogs for humans" and DPI-aware images.![Sciter tooltips](http://terrainformatica.com/w3/sciter-tooltip.png)
*   **samples/image-transformations.css++** - yet another Sciter specific CSS feature: image filters.
*   **samples/menu** - real menus defined in HTML and styled by CSS.
*   **samples/popup** - yet another demo of "airborn" DOM elements - popup windows.
*   **samples/replace-animator** - demo of animated transitions between different layouts.
*   **samples/richtext** - demonstration of the <richtext> widget - WYSIWYG HTML editor,
*   **samples/scrollbars-n-scrolling** - scrollbar and scroll manner styling.
*   **samples/selection** - demo of textual/range selections on DOM tree.
*   **samples/sqlite** - demo of _SQLite_ integration (tiscript-sqlite.dll).
*   **samples/svg** - demonstration of extended _SVG_ support in Sciter.
*   **samples/tooltips++** -  demo of tooltips/calltips definitions & styling (see image on the right).
*   **samples/video** - <video> playback in Sciter.
*   **samples/xml** - XML handling using builtin XMLParser based on the [XML tokenizer](http://www.codeproject.com/Articles/14076/Fast-and-Compact-HTML-XML-Scanner-Tokenizer).
