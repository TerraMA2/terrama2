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
 * @property {object} memberFs - 'fs' module.
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
  // 'fs' module
  var memberFs = require('fs');
  // 'path' module
  var memberPath = require('path');
  // Exec function
  var memberExec = require('child_process').exec;
  // Exec function sync
  var memberExecSync = require('child_process').execSync;
  // Spawn function
  var memberSpawn = require('child_process').spawn;

  // Socket connection event
  memberSockets.on('connection', function(client) {
    // Generate file request event
    client.on('generateFileRequest', function(json) {
      var requestFormats = json.format.split(',');

      if(memberUtils.stringInArray(requestFormats, 'all'))
        requestFormats = ['csv', 'geojson', 'kml', 'shapefile'];

      var options = {};

      options.format = requestFormats;

      var dataTimeFrom = json.dateTimeFrom.split(' ');
      var dataTimeTo = json.dateTimeTo.split(' ');
      var fileName = 'Dados.' + dataTimeFrom[0] + '.' + dataTimeTo[0];

      require('crypto').randomBytes(24, function(err, buffer) {
        var today = new Date();

        var dd = today.getDate();
        var mm = today.getMonth() + 1;
        var yyyy = today.getFullYear();

        if(dd < 10) dd = '0' + dd;
        if(mm < 10) mm = '0' + mm;

        var todayString = yyyy + '-' + mm + '-' + dd;
        var filesFolder = buffer.toString('hex') + '_--_' + todayString;

        var separator = (options.fieldSeparator !== undefined && options.fieldSeparator == "semicolon" ? "SEMICOLON" : "COMMA");
        var folderPath = memberPath.join(__dirname, '../tmp/' + filesFolder);

        try {
          memberFs.mkdirSync(folderPath);
        } catch(e) {
          if(e.code != 'EEXIST')
            console.error(e);
        }

        var processedFormats = 0;
        var progress = null;
        var progressStep = 2.5 / requestFormats.length;

        memberExportation.getPgConnectionString(json.dataProviderId).then(function(connectionString) {
          for(var i = 0, formatsLength = requestFormats.length; i < formatsLength; i++) {
            switch(requestFormats[i]) {
              case 'csv':
                var fileExtention = '.csv';
                var ogr2ogrFormat = 'CSV';
                break;
              case 'shapefile':
                var fileExtention = '.shp';
                var ogr2ogrFormat = 'ESRI Shapefile';
                break;
              case 'kml':
                var fileExtention = '.kml';
                var ogr2ogrFormat = 'KML';
                break;
              default:
                var fileExtention = '.json';
                var ogr2ogrFormat = 'GeoJSON';
            }

            if(requestFormats[i] == 'shapefile') {
              try {
                memberFs.mkdirSync(folderPath + "/shapefile");
              } catch(e) {
                if(e.code != 'EEXIST')
                  console.error(e);
              }
            }

            options.Schema = json.schema;
            options.TableName = json.table;
            options.DateTimeFieldName = json.dateTimeField;

            var ogr2ogr = memberExportation.ogr2ogr();
            var filePath = memberPath.join(__dirname, '../tmp/' + filesFolder + (requestFormats[i] == 'shapefile' ? '/shapefile/' : '/') + fileName + fileExtention);

            var args = ['-progress', '-F', ogr2ogrFormat, filePath, connectionString, '-sql', memberExportation.getQuery(json.dateTimeFrom, json.dateTimeTo, options), '-skipfailures'];

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
              console.error(err);
            });

            spawnCommand.on('error', function(err) {
              console.error(err);
            });

            spawnCommand.on('exit', function(code) {
              processedFormats++;

              if(processedFormats == formatsLength) {
                var finalizeProcess = function() {
                  if(requestFormats.length == 1) {
                    switch(requestFormats[0]) {
                      case 'csv':
                        var fileExtention = '.csv';
                        break;
                      case 'shapefile':
                        var fileExtention = '.shp';
                        break;
                      case 'kml':
                        var fileExtention = '.kml';
                        break;
                      default:
                        var fileExtention = '.json';
                    }

                    var finalPath = memberPath.join(__dirname, '../tmp/' + filesFolder) + "/" + fileName + fileExtention + (requestFormats[0] == 'shapefile' ? '.zip' : '');
                    var finalFileName = fileName + fileExtention + (requestFormats[0] == 'shapefile' ? '.zip' : '');

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
        });
      });
    });
  });
};

module.exports = Exportation;
