module.exports = function(app) {

  return {
    get: function(request, response) {
      response.render('configuration/dynamicDataSeries');
    },

    new: function(request, response) {
      response.render('configuration/dataset');
    },

    edit: function(request, response) {
      response.render('configuration/dataset');
    }
  }

};