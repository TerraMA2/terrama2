"use strict";

/**
 * Controller responsible for export data.
 * @class Exportation
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberFs - 'fs' module.
 * @property {object} memberPath - 'path' module.
 * @property {object} memberUtils - 'Utils' model.
 * @property {object} memberExportation - 'Exportation' model.
 */
var Exportation = function(app) {

  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');
  // 'Utils' model
  var memberUtils = require('../../core/Utils.js');
  // 'Exportation' model
  var memberExportation = new (require('../../core/Exportation.js'))();

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function exportData
   * @memberof Exportation
   * @inner
   */
  var exportData = function(request, response) {
    var finalPath = memberPath.join(__dirname, '../../tmp/' + request.query.folder) + "/" + request.query.file;

    response.download(finalPath, request.query.file, function(err) {
      if(err) return console.error(err);

      memberUtils.deleteFolderRecursively(memberPath.join(__dirname, '../../tmp/' + request.query.folder), function() {});
    });

    memberExportation.deleteInvalidFolders();
  };

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function exportGridFile
   * @memberof Exportation
   * @inner
   */
  var exportGridFile = function(request, response) {
    memberExportation.getGridFilePath(request.query.dpi, request.query.mask, request.query.date).then(function(gridFilePath) {
      var fileName = request.query.file + memberPath.extname(gridFilePath);

      response.download(gridFilePath, fileName);
    }).catch(function(err) {
      return console.error(err);
    });

    memberExportation.deleteInvalidFolders();
  };

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function checkGridFile
   * @memberof Exportation
   * @inner
   */
  var checkGridFile = function(request, response) {
    memberExportation.getGridFilePath(request.query.dpi, request.query.mask, request.query.date).then(function(gridFilePath) {
      memberFs.stat(gridFilePath, function(err, stat) {
        response.json({ result: err === null });
      });
    }).catch(function(err) {
      return console.error(err);
    });
  };

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function checkGridFolder
   * @memberof Exportation
   * @inner
   */
  var checkGridFolder = function(request, response) {
    memberExportation.getGridFolderPath(request.query.dpi).then(function(gridFolderPath) {
      memberFs.stat(gridFolderPath, function(err, stat) {
        response.json({ result: err === null });
      });
    }).catch(function(err) {
      return console.error(err);
    });
  };

  return {
    exportData: exportData,
    exportGridFile: exportGridFile,
    checkGridFile: checkGridFile,
    checkGridFolder: checkGridFolder
  };
};

module.exports = Exportation;
