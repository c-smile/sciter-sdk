
// aspect, uses attribute accept-drop like: filter="*.png *.jpg *.gif" 
// generates events:
//   "file-enter"/files
//   "file-leave"/files
//   "file-drop"/files
   
function DropZone(filter = "") 
{
  var extensions = (this.attributes["accept-drop"] || filter || "*").split(";");

  var files = []; // filtered files

  function checkFiles(list) {
    if(typeof list != #array )
      list = [list];
    function flt(fn) {
      for(var x in extensions)
        if( fn like x ) return true;
      return false;
    }
    files = list.filter(flt);
    return files.length > 0;
  }

  this.onExchange = function(evt)
  {
     if( (evt.type == Event.X_WILL_ACCEPT_DROP) && (evt.draggingDataType == #file)) 
     {
       return checkFiles(evt.dragging); 
     }
     else if( evt.type == Event.X_DRAG_ENTER)
     {
       this.@.addClass("allow-drop");
       this.postEvent("file-enter", files);
       return true;
     }
     else if( evt.type == Event.X_DRAG_LEAVE )
     {
       this.@.removeClass("allow-drop");
       this.postEvent("file-leave", files);
       return true;
     }
     else if( evt.type == Event.X_DRAG)
     {
       //stdout.printf("dragging %V\n", evt.dragging);      
       return true;
     }
     else if( evt.type == Event.X_DROP)
     {
       this.@.removeClass("allow-drop");
       if( files.length )
         this.postEvent("file-drop", files);
       return true;
     }
  }
} 


