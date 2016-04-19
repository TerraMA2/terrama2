var AbstractData = function(params) {
  if (this.constructor === AbstractData) {
    throw new Error("Cannot instantiate abstract class");
  }
  this.class = params.class;
};

AbstractData.prototype.toObject = function() {
  return {
    "class": this.class
  };
};

AbstractData.prototype.toJSON = function() {
  return JSON.stringify(this.toObject());
};

module.exports = AbstractData;