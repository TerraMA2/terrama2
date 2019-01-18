"use strict";

/**
 * Socket responsible for generating files for exportation.
 * @class Exportation
 * @variation 2
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberExportation - 'Exportation' model.
 * @property {object} memberUtils - 'Utils' model.
 * @property {object} memberPath - 'path' module.
 * @property {function} memberExec - Exec function.
 * @property {function} memberExecSync - Exec function sync.
 * @property {function} memberSpawn - Spawn function.
 */
var Exportation = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // 'Exportation' model
  var memberExportation = new (require('../core/Exportation.js'))();
  // 'Utils' model
  var memberUtils = require('../core/Utils.js');
  // 'path' module
  var memberPath = require('path');
  // Exec function
  var memberExec = require('child_process').exec;
  // Exec function sync
  var memberExecSync = require('child_process').execSync;
  // Spawn function
  var memberSpawn = require('child_process').spawn;
  // 'DataManager' module
  var memberDataManager = require('../core/DataManager');

  // Socket connection event
  memberSockets.on('connection', function(client) {
    // Generate file request event
    client.on('generateFileRequest', function(json) {
      var requestFormats = json.format.split(',');

      if(memberUtils.stringInArray(requestFormats, 'all'))
        requestFormats = ['csv', 'geojson', 'kml', 'shapefile'];

      var options = {};

      options.format = requestFormats;
      options.Schema = json.schema;
      options.TableName = json.table;

      // Remove special chars
      const normalizedFileName = json.fileName.replace(/[^a-zA-Z0-9]/g,'_');

      if(json.dateTimeField !== undefined && json.dateTimeFrom !== undefined && json.dateTimeTo !== undefined) {
        options.dateTimeField = json.dateTimeField;
        options.dateTimeFrom = json.dateTimeFrom;
        options.dateTimeTo = json.dateTimeTo;

        var dataTimeFrom = json.dateTimeFrom.split(' ');
        var dataTimeTo = json.dateTimeTo.split(' ');
        var fileName = normalizedFileName + '.' + dataTimeFrom[0] + '.' + dataTimeTo[0];
      } else if(json.dateTimeField !== undefined && json.date !== undefined) {
        options.dateTimeField = json.dateTimeField;
        options.date = json.date;

        var fileName = normalizedFileName + '.' + json.date;
      } else {
        var fileName = normalizedFileName;
      }

      var separator = (options.fieldSeparator !== undefined && options.fieldSeparator == "semicolon" ? "SEMICOLON" : "COMMA");
      var filesFolder = null;
      var folderPath = null;

      memberExportation.generateRandomFolder().then(function(randomFolderResult) {
        filesFolder = randomFolderResult.filesFolder;
        folderPath = randomFolderResult.folderPath;

        return memberExportation.getPgConnectionString(json.dataProviderId);
      }).then(function(connectionString) {
        var startProcess = function(innerJoinTable, innerJoinAttribute) {
          var processedFormats = 0;
          var progress = null;
          var progressStep = 2.5 / requestFormats.length;

          options.innerJoinTable = innerJoinTable;
          options.innerJoinAttribute = innerJoinAttribute;

          for(var i = 0, formatsLength = requestFormats.length; i < formatsLength; i++) {
            if(requestFormats[i] == 'shapefile') {
              var folderResult = memberExportation.createFolder(folderPath + "/shapefile");

              if(folderResult)
                console.error(folderResult);
            }

            var format = memberExportation.getFormatStrings(requestFormats[i]);
            var ogr2ogr = memberExportation.ogr2ogr();
            var filePath = memberPath.join(__dirname, '../tmp/' + filesFolder + (requestFormats[i] == 'shapefile' ? '/shapefile/' : '/') + fileName + format.fileExtention);

            var args = ['-progress', '-F', format.ogr2ogrFormat, filePath, connectionString, '-fieldTypeToString', 'Date,Time,DateTime', '-sql', memberExportation.getQuery(options), '-skipfailures'];

            if(requestFormats[i] == "csv")
              args.push('-lco', 'LINEFORMAT=CRLF', '-lco', 'SEPARATOR=' + separator);

            var spawnCommand = memberSpawn(ogr2ogr, args);

            spawnCommand.stdout.on('data', function(data) {
              if(progress === null)
                progress = 0;
              else
                progress += progressStep;

              client.emit('generateFileResponse', { progress: progress });
            });

            spawnCommand.stderr.on('data', function(data) {
              console.error(data.toString());
            });

            spawnCommand.on('error', function(err) {
              console.error(err);
            });

            spawnCommand.on('exit', function(code) {
              processedFormats++;

              if(processedFormats == formatsLength) {
                var finalizeProcess = function() {
                  if(requestFormats.length == 1) {
                    var format = memberExportation.getFormatStrings(requestFormats[0]);
                    var finalPath = memberPath.join(__dirname, '../tmp/' + filesFolder) + "/" + fileName + format.fileExtention + (requestFormats[0] == 'shapefile' ? '.zip' : '');
                    var finalFileName = fileName + format.fileExtention + (requestFormats[0] == 'shapefile' ? '.zip' : '');

                    client.emit('generateFileResponse', {
                      folder: filesFolder,
                      file: finalFileName
                    });
                  } else {
                    var finalPath = memberPath.join(__dirname, '../tmp/' + filesFolder) + "/" + fileName + ".zip";
                    var finalFileName = fileName + ".zip";

                    var zipGenerationCommand = "zip -r -j " + finalPath + " " + folderPath;

                    memberExec(zipGenerationCommand, function(zipGenerationCommandErr, zipGenerationCommandOut, zipGenerationCommandCode) {
                      if(zipGenerationCommandErr) return console.error(zipGenerationCommandErr);

                      client.emit('generateFileResponse', {
                        folder: filesFolder,
                        file: finalFileName
                      });
                    });
                  }
                };

                if(memberUtils.stringInArray(requestFormats, 'shapefile')) {
                  var zipPath = memberPath.join(__dirname, '../tmp/' + filesFolder) + "/" + fileName + ".shp.zip";
                  var zipGenerationCommand = "zip -r -j " + zipPath + " " + folderPath + "/shapefile";

                  try {
                    var zipGenerationCommandResult = memberExecSync(zipGenerationCommand);
                    memberUtils.deleteFolderRecursively(folderPath + "/shapefile", finalizeProcess);
                  } catch(e) {
                    console.error(e);
                  }
                } else {
                  finalizeProcess();
                }
              }
            });
          }
        };

        if(json.monitoredObjectId !== undefined && json.monitoredObjectPk !== undefined) {
          memberDataManager.getDataSeries({ id: json.monitoredObjectId }).then(function(dataSeries) {
            memberExportation.getPrimaryKeyColumn(dataSeries.dataSets[0].format.table_name, json.dataProviderId).then(function(primaryKeyColumnResult) {
              var primaryKeyColumn = (primaryKeyColumnResult.rows.length > 0 && primaryKeyColumnResult.rows[0].column_name ? primaryKeyColumnResult.rows[0].column_name : json.monitoredObjectPk);
              startProcess(dataSeries.dataSets[0].format.table_name, primaryKeyColumn);
            }).catch(function(err) {
              return console.error(err);
            });
          });
        } else {
          startProcess();
        }
      }).catch(function(err) {
        return console.error(err);
      });
    });
  });
};

module.exports = Exportation;
