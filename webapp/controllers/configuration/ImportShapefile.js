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

    var filesFolder = null;
    var folderPath = null;

    memberExportation.generateRandomFolder().then(function(randomFolderResult) {
      filesFolder = randomFolderResult.filesFolder;
      folderPath = randomFolderResult.folderPath;

      return memberExportation.getPsqlString(request.body.dataProviderId);
    }).then(function(connectionString) {
      memberFs.readFile(request.files.file.path, function(err, data) {
        if(err)
          return response.json({ error: err.toString() });
        else {
          var path = memberPath.join(__dirname, '../../tmp/' + filesFolder + '/' + request.files.file.name);

          memberFs.writeFile(path, data, function(err) {
            if(err)
              return response.json({ error: err.toString() });
            else {
              try {
                memberFs.createReadStream(path).pipe(memberUnzip.Extract({ path: folderPath })).on('close', function() {
                  var files = memberFs.readdirSync(folderPath);
                  var shpName = null;
                  var shpCount = 0;

                  for(var i = 0, filesLength = files.length; i < filesLength; i++) {
                    var filename = memberPath.join(folderPath, files[i]);
                    var stat = memberFs.lstatSync(filename);

                    if(!stat.isDirectory() && filename.indexOf(".shp") >= 0) {
                      shpName = filename;
                      shpCount++;
                    };
                  }

                  if(shpName !== null) {
                    if(shpCount === 1) {
                      var progress = null;
                      var progressStep = 1;//2.5 / requestFormats.length;

                      var shp2pgsql = memberExportation.shp2pgsql();

                      memberExportation.tableExists(request.body.tableName, request.body.dataProviderId).then(function(resultTable) {
                        if(resultTable.rowCount > 0 && resultTable.rows[0].table_name == request.body.tableName) {
                          return response.json({ error: "Table already exists!" });
                        } else {
                          memberExec(connectionString.exportPassword + shp2pgsql + " -I -s " + request.body.srid + " -W \"" + request.body.encoding + "\" " + shpName + " " + request.body.tableName + " | " + connectionString.connectionString, function(commandErr, commandOut, commandCode) {
                            if(commandErr)
                              return response.json({ error: commandErr });

                            memberExportation.tableExists(request.body.tableName, request.body.dataProviderId).then(function(resultTable) {
                              if(resultTable.rowCount > 0 && resultTable.rows[0].table_name == request.body.tableName) {
                                memberUtils.deleteFolderRecursively(folderPath, function() {});

                                return response.json({ error: null, message: "Sucess: " + shpName });
                              } else {
                                return response.json({ error: commandCode });
                              }
                            }).catch(function(err) {
                              return response.json({ error: err.toString() });
                            });
                          });
                        }
                      }).catch(function(err) {
                        return response.json({ error: err.toString() });
                      });
                    } else {
                      return response.json({ error: "Error: More than one shapefile found!" });
                    }
                  } else {
                    return response.json({ error: "Error: No shapefile found!" });
                  }
                });
              } catch(err) {
                return response.json({ error: err.toString() });
              }
            }
          });
        }
      });
    }).catch(function(err) {
      return response.json({ error: err.toString() });
    });
  };

  return importShapefile;
};

module.exports = ImportShapefile;