/**
* tiscript patterns
*
* @author Andrew Fedoniouk
* @version 1.0.0
*/
Rainbow.extend( "tis", [

   /**
     * matches $. or $(
     */
  /*{
        "name": "selector",
        "pattern": /\$\w*\(.*\)/g
  },*/
  
  {
      "matches": {
          1: "entity.function",
          2: "selector"
      },
      "pattern": /(\$\w*)\(([^)]*)\)/g
  },  
  
  {
    name: "constant",
    pattern: /\b(false|null|true|undefined)\b/g
  },
  {
    name: "keyword",
    pattern: /\b(function|var|if|else|while|return|for|break|continue|do|switch|case|default|null|super|new|try|catch|finally|throw|typeof|instanceof|in|property|const|get|set|include|like|class|namespace|this|assert|delete|otherwise|with|__FILE__|__LINE__|__TRACE__|debug)\b/g
  },
  {
    name: "string",
    pattern: /(".*?")/g
  },
  {
    name: "char",
    pattern: /(')(.|\\.|\\u[\dA-Fa-f]{4})\1/g
  },
  {
    "matches": {
        1: "constant.numeric",
        2: "constant.unit"
    },
    pattern: /(\d+)(px|em|cm|dip|s|ms%)?/g
  },  
  {
    name: "integer",
    pattern: /\b(0x[\da-f]+|\d+)L?\b/g
  },
  {
    name: "comment",
    pattern: /\/\*((?:.|\r|\n)*?)\*\/|(\/\/).*?$/g
  },
  {
    name: "support.annotation",
    pattern: /@\w+/g
  },
 /**
   * matches constructor call
   */
  /*{
      "matches": {
          1: "keyword",
          2: "entity.function"
      },
      "pattern": /(new)\s+(.*)(?=\()/g
  }, */  
  
  /*{
    matches: {
      1: "entity.function"
    },
    pattern: /([^@\.\s]+)\(/g
  },*/
  /*{
    name: "entity.class",
    pattern: /\b([A-Z]\w*)\b/g
  }*/
], true );
