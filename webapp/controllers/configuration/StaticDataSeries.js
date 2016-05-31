var DataManager = require('./../../core/DataManager');


module.exports = function(app) {

  return {
    get: function(request, response) {
      response.render('configuration/staticData');
    },

    new: function(request, response) {
      response.render('configuration/dataset', {type: "static"});
    },

    edit: function(request, response) {
      var dataSeriesId = request.params.id;

      DataManager.getDataSeries({id: dataSeriesId}).then(function(dataSeriesResult) {
        response.render('configuration/dataset');
      }).catch(function(err) {
        response.render('base/404');
      })
    }
  };

};
