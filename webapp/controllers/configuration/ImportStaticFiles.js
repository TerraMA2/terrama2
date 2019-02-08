"use strict";

/**
 * Controller responsible for importing static files.
 * @class ImportStaticFiles
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberFs - 'fs' module.
 * @property {object} memberPath - 'path' module.
 * @property {object} memberExportation - 'Exportation' model.
 * @property {object} memberUnzip - 'unzip' module.
 * @property {object} memberUtils - 'Utils' model.
 */
const ImportStaticFiles = (/*app*/) => {
  // 'fs' module
  const memberFs = require('fs');
  // 'path' module
  const memberPath = require('path');
  // 'DataManager' module
  const memberDataManager = require('../../core/DataManager.js');
  // 'Exportation' model
  const memberExportation = new (require('../../core/Exportation.js'))();
  // 'Utils' model
  const memberUtils = require('../../core/Utils.js');

  const { ShapeImporter } = require('./../../core/ShapeImporter');

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function importShapefile
   * @memberof ImportStaticFiles
   * @inner
   */
  var importShapefile = function(request, response) {
    var sendResponse = function(error, folder) {
      if(folder !== null) memberUtils.deleteFolderRecursively(folder, function() {});
      memberExportation.deleteInvalidFolders();
      return response.json({ error: error });
    };

    var filesFolder = null;
    var folderPath = null;

    memberExportation.generateRandomFolder().then(function(randomFolderResult) {
      filesFolder = randomFolderResult.filesFolder;
      folderPath = randomFolderResult.folderPath;

      memberFs.readFile(request.files.file.path, function(err, data) {
        if(err)
          return sendResponse(err.toString(), folderPath);
        else {
          var path = memberPath.join(__dirname, '../../tmp/' + filesFolder + '/' + request.files.file.name);

          memberFs.writeFile(path, data, async err => {
            if(err)
              return sendResponse(err.toString(), folderPath);
            else {
              try {
                // Retrieve file information such size, permissions, etc.
                const fileProperties = memberFs.lstatSync(path);

                // Check file size. TODO: Remove it, since the it should be validated on header parsing
                if(!fileProperties.isDirectory() && (fileProperties.size / 1048576) > 300) {
                  return sendResponse('File is too large', path);
                }

                const importer = new ShapeImporter(folderPath, request.body.srid, request.body.encoding);
                importer.unzip(path);

                if(request.body.semantics === 'STATIC_DATA-postgis') {
                  await importer.toDatabase(request.body.tableName, request.body.dataProviderId);
                } else {
                  const mask = request.body.mask.split("\\").join("/");

                  if(memberPath.extname(mask) !== ".shp") {
                    return sendResponse("Invalid file name!", folderPath);
                  }

                  await importer.toDataProvider(request.body.dataProviderId, mask);
                }

                return sendResponse(null, folderPath);
              } catch(err) {
                return sendResponse(err.toString(), folderPath);
              }
            }
          });
        }
      });
    }).catch(function(err) {
      return sendResponse(err.toString(), folderPath);
    });
  };

  /**
   * Processes the request and returns a response.
   * @param {json} request - JSON containing the request data
   * @param {json} response - JSON containing the response data
   *
   * @function importGeoTIFF
   * @memberof ImportStaticFiles
   * @inner
   */
  var importGeoTIFF = function(request, response) {
    var sendResponse = function(error, folder) {
      if(folder !== null) memberUtils.deleteFolderRecursively(folder, function() {});
      memberExportation.deleteInvalidFolders();
      return response.json({ error: error });
    };

    var filesFolder = null;
    var folderPath = null;

    memberExportation.generateRandomFolder().then(function(randomFolderResult) {
      filesFolder = randomFolderResult.filesFolder;
      folderPath = randomFolderResult.folderPath;

      memberFs.readFile(request.files.file.path, function(err, data) {
        if(err)
          return sendResponse(err.toString(), folderPath);
        else {
          var path = memberPath.join(__dirname, '../../tmp/' + filesFolder + '/' + request.files.file.name);

          memberFs.writeFile(path, data, function(err) {
            if(err)
              return sendResponse(err.toString(), folderPath);
            else {
              try {
                if(memberPath.extname(path) === ".tif") {
                  var mask = request.body.mask.split("\\").join("/");

                  if(memberPath.extname(mask) === ".tif") {
                    memberDataManager.getDataProvider({ id: request.body.dataProviderId }).then(function(dataProvider) {
                      var dataProviderPath = dataProvider.uri.replace("file://", "");

                      if(memberFs.existsSync(dataProviderPath)) {
                        var finalFilePath = (dataProviderPath + "/" + mask).split("//").join("/");

                        if(!memberFs.existsSync(finalFilePath)) {
                          var maskArray = mask.split("/");

                          if(maskArray.length > 1)
                            var pathCreationResult = memberExportation.createPathToFile(dataProviderPath, maskArray);
                          else
                            var pathCreationResult = {
                              error: null,
                              createdPath: dataProviderPath
                            };

                          if(!pathCreationResult.error) {
                            var createdPath = pathCreationResult.createdPath;
                            var newFilename = maskArray[maskArray.length - 1].replace(memberPath.extname(maskArray[maskArray.length - 1]), "");

                            memberExportation.copyFileSync(path, createdPath, newFilename);

                            return sendResponse(null, folderPath);
                          } else {
                            return sendResponse(pathCreationResult.error, folderPath);
                          }
                        } else {
                          return sendResponse("File already exists!", folderPath);
                        }
                      } else {
                        return sendResponse("Invalid data provider path!", folderPath);
                      }
                    });
                  } else {
                    return sendResponse("Invalid file name!", folderPath);
                  }
                } else {
                  return sendResponse("Invalid file extension!", folderPath);
                }
              } catch(err) {
                return sendResponse(err.toString(), folderPath);
              }
            }
          });
        }
      });
    }).catch(function(err) {
      return sendResponse(err.toString(), folderPath);
    });
  };

  return {
    importShapefile: importShapefile,
    importGeoTIFF: importGeoTIFF
  };
};

module.exports = ImportStaticFiles;