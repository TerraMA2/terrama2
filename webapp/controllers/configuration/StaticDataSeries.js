module.exports = function(app) {

  return {
    get: function(request, response) {
      response.render('configuration/staticData');
    },

    new: function(request, response) {
      response.render('configuration/dataset');
    },

    edit: function(request, response) {
      response.render('configuration/dataset');
    }
  };

};