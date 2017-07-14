'use strict';

define(['components/Utils'],
  function(Utils) {
    var loadSocketsListeners = function() {
      Utils.getWebAppSocket().on('generateFileResponse', function(result) {
        if(result.progress !== undefined && result.progress >= 100) {
          console.log('Quase lá! O arquivo está sendo preparado para o download<span>...</span>');
        } else if(result.progress !== undefined) {
          console.log(result.progress + '% Completo');
        } else {
          console.log("folder=" + result.folder + "&file=" + result.file);
        }
      });
    };

    var loadEvents = function() {
      $('#export').on('click', function() {
				Utils.getWebAppSocket().emit('generateFileRequest', {
					format: 'all',
					dateTimeFrom: '2017-07-10 00:00:00',
					dateTimeTo: '2017-07-12 23:59:59',
					schema: 'public',
					table: 'focos_saida',
					dateTimeField: 'data_pas',
					dataProviderId: 2
				});
			});
    };

    var init = function() {
      loadSocketsListeners();
      loadEvents();
    };

    return {
      init: init
    };
  }
)