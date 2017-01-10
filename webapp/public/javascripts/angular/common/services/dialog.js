define([],
  function() {
    function FileDialog() {
      var callDialog = function(dialog, callback) {
        dialog.addEventListener('change', function(event) {
          var result = dialog.value;

          if (dialog.files && dialog.files.length > 0) {
            // reader.readAsText(dialog.files[0]);
            return callback(null, dialog);
          } else {
            return callback(new Error("No file"));
          }
        }, false);
        dialog.click();
      };

      var dialogs = {};

      dialogs.saveAs = function(callback, defaultFilename, acceptTypes) {
        var dialog = document.createElement('input');
        dialog.type = 'file';
        dialog.nwsaveas = defaultFilename || '';
        if (angular.isArray(acceptTypes)) {
          dialog.accept = acceptTypes.join(',');
        } else if (angular.isString(acceptTypes)) {
          dialog.accept = acceptTypes;
        }
        callDialog(dialog, callback);
      };

      dialogs.openFile = function(callback, multiple, acceptTypes) {
        var dialog = document.createElement('input');
        dialog.type = 'file';
        if (multiple === true) {
          dialog.multiple = 'multiple';
        }
        if (angular.isArray(acceptTypes)) {
          dialog.accept = acceptTypes.join(',');
        } else if (angular.isString(acceptTypes)) {
          dialog.accept = acceptTypes;
        }
        return callDialog(dialog, callback);
      };

      dialogs.openDir = function(callback) {
        var dialog = document.createElement('input');
        dialog.type = 'file';
        dialog.nwdirectory = 'nwdirectory';
        callDialog(dialog, callback);
      };

      dialogs.readAsJSON = function(fileBlob, callback) {
        if (!fileBlob) {
          return callback(new Error("Invalid file"));
        }
        var reader = new FileReader();
        reader.onload = function(file) {
          try {
            callback(null, JSON.parse(reader.result));
          } catch (e) {
            callback(new Error("Invalid configuration file: " + e.toString()));
          }
        };

        reader.readAsText(fileBlob);
      };
      /**
       * It reads XML file loaded from dialog
       *
       * @param {Blob} fileBlob - A javascript blob with file result
       * @param {Function<Error|string>} callback - A callback function to handle reader async.
       */
      dialogs.readAsXML = function(fileBlob, callback) {
        if (!fileBlob) {
          return callback(new Error("Invalid file"));
        }

        var reader = new FileReader();
        reader.onload = function(file) {
          // detecting dom parser
          if (window.DOMParser) {
            parser = new DOMParser();
            var xmlDoc = parser.parseFromString(reader.result, "text/xml");
          } else {
            var xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
            xmlDoc.async = false;
            xmlDoc.loadXML(reader.result);
          }
        };
      };

      return dialogs;
    }

    return FileDialog;
  }
)