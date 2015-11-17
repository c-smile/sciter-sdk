/**
 * CSS patterns
 *
 * @author Craig Campbell
 * @version 1.0.9
 */
Rainbow.extend("css", [
    {
        "name": "comment",
        //"pattern": /\/\*(?:.|\r|\n)*?\*\//g
        "pattern": /\/\*((?:.|\r|\n)*?)\*\/|(\/\/).*?$/g // sciter supports single line comments in CSS
    },
    {
        "name": "constant.hex-color",
        "pattern": /#([a-f0-9]{3}|[a-f0-9]{6})(?=;|\s|,|\))/gi
    },
    {
        "matches": {
            1: "constant.numeric",
            2: "constant.unit"
        },
        "pattern": /(\d+)(dip|px|em|cm|mm|s|%|\*)?/g
    },
    {
        "matches": {
            1: "constant.func",
            2: "constant.url",
            3: "constant.func",
        },
        "pattern": /(url\()([^)]+)(\))/g
    },    
    {
        "name": "string",
        "pattern": /('|")(.*?)\1/g
    },
    {
        "name": "css.css-property",
        "matches": {
            1: "support.vendor-prefix"
        },
        "pattern": /(-o-|-moz-|-webkit-|-ms-)?[\w-]+(?=\s?:)(?!.*\{)/g
    },
    {
        "name": "at-rule",
        "pattern": /@[\w-]+/g
    },    
    {
        "matches": {
            1: [
                {
                    "name": "entity.name.sass",
                    "pattern": /&amp;/g
                },
                {
                    "name": "direct-descendant",
                    "pattern": /&gt;/g
                },
                {
                    "name": "entity.name.class",
                    "pattern": /\.[\w\-_]+/g
                },
                {
                    "name": "entity.name.id",
                    "pattern": /\#[\w\-_]+/g
                },
                {
                    "name": "entity.name.pseudo",
                    "pattern": /:[\w\-_]+/g
                },
                {
                    "name": "entity.name.tag",
                    "pattern": /\w+/g
                }
            ]
        },
        "pattern": /([\w\ ,\r\n:\.\#\&\;\-_]+)(?=.*\{)/g
    },
    {
        "matches": {
            2: "css.vendor-prefix",
            3: "css.css-value"
        },
        "pattern": /(:|,)\s*(-o-|-moz-|-webkit-|-ms-)?([a-zA-Z-]*)(?=\b)(?!.*\{)/g
    }
], true);
