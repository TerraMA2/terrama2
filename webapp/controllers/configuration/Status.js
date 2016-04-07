module.exports = function(app) {
  return function(request, response) {
    return response.render("configuration/status");
  }
};