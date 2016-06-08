var DataManager = require('./../../core/DataManager');
var Enums = require('./../../core/Enums');


module.exports = function(app) {

  return {
    get: function(request, response) {
      response.render('configuration/dynamicDataSeries');
    },

    new: function(request, response) {
      response.render('configuration/dataset', {type: "dynamic", "Enums": Enums});
    },

    edit: function(request, response) {
      var dataSeriesId = request.params.id;

      DataManager.getCollector({
        output: {
          id: dataSeriesId
        }
      }).then(function(collectorResult) {
        var promises = [];
        promises.push(DataManager.getDataSeries({id: collectorResult.input_data_series}));
        promises.push(DataManager.getDataSeries({id: collectorResult.output_data_series}));

        Promise.all(promises).then(function(dataSeriesResults) {
          console.log(dataSeriesResults);
          console.log(dataSeriesResults[0].dataSets[0].format);
          response.render('configuration/dataset', {
            state: "dynamic",
            type: "dynamic",
            "Enums": Enums,
            dataSeries: {
              input: dataSeriesResults[0].rawObject(),
              output: dataSeriesResults[1].rawObject(),
            },
            collector: collectorResult.rawObject()
          });
        }).catch(function(err) {
          console.log(err);
        });
      }).catch(function(err) {
        response.render('base/404');
      })
    }
  }

};
