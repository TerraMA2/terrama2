"use strict";

var path = require('path'),
    dir = require('fs').readdirSync(__dirname + path.sep);

dir.forEach(function(filename){
  if (path.extname(filename) === '.js' && filename !== 'index.js' && filename !== 'Filter.js'){
    var exportAsName = path.basename(filename);
    module.exports[exportAsName.substr(0, exportAsName.length-3)] = require(path.join( __dirname, filename));
  }
});