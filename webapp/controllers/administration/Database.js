module.exports = function(app) {
  return {
    get: function (request, response) {
      return response.render('administration/databases');
    },

    new: function (request, response) {
      return response.render('administration/database');
    }
  };
};