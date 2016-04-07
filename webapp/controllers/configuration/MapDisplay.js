module.exports = function(app) {

  return {
    get: function(request, response) {
      response.render("configuration/mapDisplay");
    }
  };
};