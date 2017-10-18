"use strict";

/**
 * Controller responsible for retrieving cached views data.
 * @class PredefinedStyles
 *
 * @author Francisco Vinhas [francisco.neto@funcate.org.br]
 *
 */
var PredefinedStyles = function(app){
  var fs = require("fs");
  var path = require("path");

  var getPredefinedStyles = function(request, response){
    var styleFiles = fs.readdirSync(path.join(__dirname, "../../public/json_styles"));
    var styleArray = [];
    styleFiles.forEach(function(styleFile){
      var styleFileContent = JSON.parse(fs.readFileSync(path.join(__dirname, "../../public/json_styles/" + styleFile)));
      styleArray.push(styleFileContent);
    });

    response.json({ styles: styleArray });
  }

  return {
    getPredefinedStyles: getPredefinedStyles
  }
};

module.exports = PredefinedStyles;
