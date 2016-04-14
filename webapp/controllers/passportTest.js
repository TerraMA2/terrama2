module.exports = function(app) {
  function passportTest(request, response) {
    response.render("passportTest");
  }

  return passportTest;
};
