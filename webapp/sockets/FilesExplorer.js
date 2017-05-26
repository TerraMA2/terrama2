"use strict";

/**
 * Socket responsible for checking the smtp connection with a given host.
 * @class FilesExplorer
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 */
var FilesExplorer = function(io) {

  // Sockets object
  var memberSockets = io.sockets;

  var memberFtp = require('ftp');

  var memberSftp = require('ssh2').Client;

  // Socket connection event
  memberSockets.on('connection', function(client) {

    // List files request event
    client.on('listFilesRequest', function(json) {
      var ftpsClient = new memberSftp();

      ftpsClient.on('ready', function() {
        ftpsClient.sftp(function(err, sftp) {
          if(err) {
            client.emit('listFilesResponse', { error: err.toString() });
            ftpsClient.end();
          } else {
            sftp.readdir(json.path, function(err, list) {
              if(err) {
                client.emit('listFilesResponse', { error: err.toString() });
                ftpsClient.end();
              } else {
                var items = [];

                for(var i = 0, listLength = list.length; i < listLength; i++) {
                  if(list[i].longname.charAt(0) == 'd' && list[i].filename.charAt(0) != '.')
                    items.push({
                      name: list[i].filename,
                      fullPath: (json.path != '/' ? json.path + '/' + list[i].filename : json.path + list[i].filename),
                      children: [],
                      childrenVisible: false
                    });
                }

                items.sort(function(a, b) {
                  if(a.name < b.name) return -1;
                  if(a.name > b.name) return 1;
                  return 0;
                });

                var result = {
                  list: items
                };

                if(json.parent) {
                  result.parent = json.parent;
                }

                client.emit('listFilesResponse', result);
                ftpsClient.end();
              }
            });
          }
        });
      });

      ftpsClient.on('error', function(err) {
        client.emit('listFilesResponse', { error: err.toString() });
        ftpsClient.end();
      });

      try {
        ftpsClient.connect({
          host: json.host,
          port: json.port,
          username: json.username,
          password: json.password
        });
      } catch(err) {
        client.emit('listFilesResponse', { error: err.toString() });
        ftpsClient.end();
      }

      /*var ftpClient = new memberFtp();

      var config = {
        user: json.username,
        password: json.password,
        host: json.host,
        port: json.port
      };

      ftpClient.on('error', function(err) {
        client.emit('listFilesResponse', { list: err });
        ftpClient.end();
      });

      ftpClient.on('ready', function() {
        ftpClient.list(json.path, function(err, list) {
          if(err) {
            client.emit('listFilesResponse', { list: err });
            ftpClient.end();
          } else {
            client.emit('listFilesResponse', { list: list });
            ftpClient.end();
          }
        });
      });

      ftpClient.connect(config);*/
    });
  });
};

module.exports = FilesExplorer;
