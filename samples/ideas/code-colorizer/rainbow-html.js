/**
 * HTML patterns
 *
 * @author Craig Campbell
 * @version 1.0.9
 */
Rainbow.extend("html", [
/*    {
        "name": "source.php.embedded",
        "matches": {
            2: {
                "language": "php"
            }
        },
        "pattern": /&lt;\?=?(?!xml)(php)?([\s\S]*?)(\?&gt;)/gm
    }, */
    {
        "name": "source.css.embedded",
        "matches": {
            1: "html.tag.style.start",
            2: {
                "language": "css"
            },
            3: "html.tag.style.end",
        },
        "pattern": /(&lt;style.*&gt;)((?:.|\r|\n)*?)(&lt;\/style&gt;)/gi
    },
    {
        "name": "source.tis.embedded",
        "matches": {
            1: "html.tag.script.start",
            2: {
                "language": "tis"
               },
            3: "html.tag.script.end"
        },
        "pattern": /(&lt;script.*&gt;)((?:.|\r|\n)*?)(&lt;\/script&gt;)/gi
    },
    
    {
        "name": "html.comment",
        "pattern": /&lt;\!--((?:.|\r|\n)*?)--&gt;/gm
    },
    {
        "matches": {
            1: "html.tag.open",
            2: "html.tag.close"
        },
        "pattern": /(&lt;)|(\/?\??&gt;)/g
    },
    {
        "name": "html.tag",
        "matches": {
            1: "html.tag",
            2: "html.tag.special",
            3: "html.tag-name",
            4: "html.tag.attributes",
            5: "html.tag.special"
        },
        "pattern": /(&lt;\??)([a-z0-9-?\/]+)([^&]*)(&gt;)/gi
    },
], true);
