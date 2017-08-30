"use strict";

/**
 * Controller responsible for importing shapefiles to the database.
 * @class ImportShapefile
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
var ImportShapefile = function(app) {

  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');
  // 'Exportation' model
  var memberExportation = new (require('../../core/Exportation.js'))();
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

      return memberExportation.getPsqlString(request.body.dataProviderId);
    }).then(function(connectionString) {
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

                    if(!stat.isDirectory() && filename.substr(filename.length - 4) !== ".zip" && (stat.size / 1048576) > 300) {
                      shpError = "File is too large!";
                      break;
                    }

                    if(!stat.isDirectory() && filename.substr(filename.length - 4) === ".shp") {
                      shpName = filename;
                      shpCount++;
                    };
                  }

                  if(shpError !== null) {
                    return sendResponse(shpError, folderPath);
                  } else if(shpName !== null) {
                    if(shpCount === 1) {
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

  return importShapefile;
};

module.exports = ImportShapefile;