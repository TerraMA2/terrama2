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
 * @property {object} memberExec - Exec function.
 * @property {object} memberExecSync - Exec function sync.
 * @property {object} memberSpawn - Spawn function.
 * @property {object} memberUtils - 'Utils' model.
 */
var ImportStaticFiles = function(app) {

  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');
  // 'Exportation' model
  var memberExportation = new (require('../../core/Exportation.js'))();
  // 'DataManager' module
  var memberDataManager = require('../../core/DataManager.js');
  // 'unzip' module
  var memberUnzip = require('unzip');
  // Exec function
  var memberExec = require('child_process').exec;
  // Exec function sync
  var memberExecSync = require('child_process').execSync;
  // Spawn function
  var memberSpawn = require('child_process').spawn;
  // 'Utils' model
  var memberUtils = require('../../core/Utils.js');

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

          memberFs.writeFile(path, data, function(err) {
            if(err)
              return sendResponse(err.toString(), folderPath);
            else {
              try {
                memberFs.createReadStream(path).pipe(memberUnzip.Extract({ path: folderPath })).on('close', function() {
                  var files = memberFs.readdirSync(folderPath);
                  var shpName = null;
                  var shpCount = 0;
                  var shpError = null;

                  for(var i = 0, filesLength = files.length; i < filesLength; i++) {
                    var filename = memberPath.join(folderPath, files[i]);
                    var stat = memberFs.lstatSync(filename);

                    if(!stat.isDirectory() && memberPath.extname(filename) !== ".zip" && (stat.size / 1048576) > 300) {
                      shpError = "File is too large!";
                      break;
                    }

                    if(!stat.isDirectory() && memberPath.extname(filename)  === ".shp") {
                      shpName = filename;
                      shpCount++;
                    };
                  }

                  if(shpError !== null) {
                    return sendResponse(shpError, folderPath);
                  } else if(shpName !== null) {
                    if(shpCount === 1) {
                      if(request.body.semantics === 'STATIC_DATA-postgis') {
                        memberExportation.getPsqlString(request.body.dataProviderId).then(function(connectionString) {
                          memberExportation.tableExists(request.body.tableName, request.body.dataProviderId).then(function(resultTable) {
                            if(resultTable.rowCount > 0 && resultTable.rows[0].table_name == request.body.tableName) {
                              return sendResponse("Table already exists!", folderPath);
                            } else {
                              memberExec(connectionString.exportPassword + memberExportation.shp2pgsql() + " -I -s " + request.body.srid + " -W \"" + request.body.encoding + "\" " + shpName + " " + request.body.tableName + " | " + connectionString.connectionString, function(commandErr, commandOut, commandCode) {
                                if(commandErr)
                                  return sendResponse(commandErr.toString(), folderPath);

                                memberExportation.tableExists(request.body.tableName, request.body.dataProviderId).then(function(resultTable) {
                                  if(resultTable.rowCount > 0 && resultTable.rows[0].table_name == request.body.tableName)
                                    return sendResponse(null, folderPath);
                                  else
                                    return sendResponse(commandCode, folderPath);
                                }).catch(function(err) {
                                  return sendResponse(err.toString(), folderPath);
                                });
                              });
                            }
                          }).catch(function(err) {
                            return sendResponse(err.toString(), folderPath);
                          });
                        }).catch(function(err) {
                          return sendResponse(err.toString(), folderPath);
                        });
                      } else {
                        var mask = request.body.mask.split("\\").join("/");

                        if(memberPath.extname(mask) === ".shp") {
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

                                  memberExportation.copyShpFiles(folderPath, createdPath, newFilename);

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
                      }
                    } else {
                      return sendResponse("More than one shapefile found!", folderPath);
                    }
                  } else {
                    return sendResponse("No shapefile found!", folderPath);
                  }
                });
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