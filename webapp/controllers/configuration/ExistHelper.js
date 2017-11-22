"use strict";

/**
 * Controller responsible for verifying if a given helper file exists.
 * @class ExistHelper
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberFs - 'fs' module.
 * @property {object} memberPath - 'path' module.
 */
var ExistHelper = function(app) {

  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function existHelper
   * @memberof ExistHelper
   * @inner
   */
  var existHelper = function(request, response) {
    if(request.body.file) {
      var fileNameArray = request.body.file.split("/");
      var file = memberPath.join(__dirname, "./../../../" + fileNameArray[fileNameArray.length - 2] + "/" + fileNameArray[fileNameArray.length - 1]);

      memberFs.stat(file, function(err, stat) {
        response.json({ result: err === null });
      });
    } else {
      response.json({ result: false });
    }
  };

  return {
    existHelper: existHelper
  };
};

module.exports = ExistHelper;