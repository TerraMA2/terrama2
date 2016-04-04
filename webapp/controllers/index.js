module.exports = function(app) {

  function indexController(request, response) {
    return response.redirect("/login");
  }

  return indexController;
};
