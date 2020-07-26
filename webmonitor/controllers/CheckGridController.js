"use strict";

/**
 * Controller responsible for checking if a grid file / folder exists.
 * @class CheckGridController
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberHttp - 'http' module.
 * @property {object} memberHttps - 'https' module.
 * @property {object} memberAdminHostInfo - WebAdmin host info.
 */
var CheckGridController = function(app) {

  // 'http' module
  var memberHttp = require('http');
  
  var Application = require('./../core/Application');
  // WebAdmin host info
  var memberAdminHostInfo = Application.getContextConfig().webadmin;

  var isSSL = Application.getContextConfig().ssl;

  // 'http' module
  var memberHttp = require('http');
  // 'https' module
  var memberHttps = require('https');

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function checkGridFile
   * @memberof CheckGridController
   * @inner
   */
  var checkGridFile = function(request, response) {
    const http = isSSL ? memberHttps : memberHttp;
    var url = memberAdminHostInfo.protocol + memberAdminHostInfo.host + ":" + memberAdminHostInfo.port + memberAdminHostInfo.basePath + "check-grid?dpi=" + request.body.dpi + "&mask=" + request.body.mask + "&file=" + request.body.file;

    if(request.body.date !== undefined)
      url += "." + request.body.date + "&date=" + request.body.date;

    http.get({url, rejectUnauthorized: false}, function(resp) {
      var body = '';

      resp.on('data', function(chunk) {
        body += chunk;
      });

      resp.on('end', function() {
        try {
          body = JSON.parse(body);
        } catch(ex) {
          body = { result: false };
        }

        // JSON response
        response.json(body);
      });
    }).on("error", function(e) {
      console.error(e.message);
      response.json({ result: false });
    });
  };

    /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function checkGridFolder
   * @memberof CheckGridController
   * @inner
   */
  var checkGridFolder = function(request, response) {
    const http = isSSL ? memberHttps : memberHttp;
    var url = memberAdminHostInfo.protocol + memberAdminHostInfo.host + ":" + memberAdminHostInfo.port + memberAdminHostInfo.basePath + "check-grid-folder?dpi=" + request.body.dpi;

    http.get({url: url, rejectUnauthorized: false}, function(resp) {
      var body = '';

      resp.on('data', function(chunk) {
        body += chunk;
      });

      resp.on('end', function() {
        try {
          body = JSON.parse(body);
        } catch(ex) {
          body = { result: false };
        }

        // JSON response
        response.json(body);
      });
    }).on("error", function(e) {
      console.error(e.message);
      response.json({ result: false });
    });
  };

  return {
    checkGridFile: checkGridFile,
    checkGridFolder: checkGridFolder
  };
};

module.exports = CheckGridController;