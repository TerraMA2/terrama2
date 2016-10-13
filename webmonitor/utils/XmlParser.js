"use strict";

var xml2js = require('xml2js');
var parse = new xml2js.Parser({explicitRoot: false});

var XmlParser = function(xml){
    var parsed;
    parse.parseString(xml, function (err, result){
        parsed = result;
    })
    return parsed;
}
module.exports = XmlParser;