
class LiveReload
{
  function this(on_reload) {
    this.enabled = false;
    this.urls = {};
    this.watchers = {};
    this.onRequestResponse = null;
    this.onReload = on_reload;
    this.delayReload = () => this.reload();
  }

  // PUBLIC:

  function stop() {
    this.enabled = false;
    self.timer(0, this.delayReload);
  }

  function start() {
    this.enabled = true;
    this.monitorRequests();
  }

  function reset() {
    // clear already tracked URLs
    // and wait for the new ones.
    this.urls = {};
  }

  // PRIVATE:

  function reload() {
    view.root.postEvent("change-processed");
    if(!this.enabled)
      return;
    // reload the current file
    //debug(live: reload);
    this.reset();
    if(this.onReload)
      this.onReload();
  }

  function scheduleReload() {
    view.root.postEvent("change-detected");
    self.timer(3s, this.delayReload);
  }

  function addFile(url) {
    if(this.urls[url])
      return;

    var path = URL.toPath(url /~ "/").toLowerCase();

    this.urls[url] = path;
    //debug(live: add {url} as {path});

    if(!this.watchers[path]) {
      this.watchers[path] =

        System.watch(path) << event change (filename) {
          var path = this.path + "/" + filename;
          //debug(live: change in {path});
          (this super).scheduleReload(path);
        };
    }
  }

  function monitorRequests() {
    //debug(live: start);

    // save the original handler
    if(!this.onRequestResponse)
      this.onRequestResponse = view.onRequestResponse;

    // install a new hook

    view.onRequestResponse = function(rq) {
      var liveReload = (this super);

      function dataTypeFrom(uri) {
        switch(uri.ext.toLowerCase())
        {
          case "htm": case "html": return Request.DATA_HTML;
          case "css": return Request.DATA_STYLE;
          case "png": case "jpg": case "bmp": case "gif": case "svg": return Request.DATA_IMAGE;
        }
        return Request.DATA_RAW_DATA;
      }

      function isTracked(rq, url) {
        if(!liveReload.enabled)
          return false;

        var uri = URL.parse(url);
        if(uri.protocol != "file")
          return false;

        var rt = rq.requestDataType;
        if(rt == Request.DATA_RAW_DATA)
          rt = dataTypeFrom(uri);

        if(rt == Request.DATA_HTML || rt == Request.DATA_STYLE || rt == Request.DATA_IMAGE)
          return true;

        return false;
      }

      // check this resource
      var url = rq.responseUrl || rq.requestUrl;
      var reloadable = isTracked(rq, url);
      // debug({url} rq {rq.status} {rq.isConsumed});

      // add a new URL to track
      if(reloadable) {
        liveReload.addFile(url);
      }

      // call the original handler
      if(liveReload.onRequestResponse)
        liveReload.onRequestResponse(rq);
    }
  }
}
