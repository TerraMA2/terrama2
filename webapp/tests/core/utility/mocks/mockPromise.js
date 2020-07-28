function mockPromise(promise) {
  // Don't modify any promise that has been already modified.
  if (promise.isResolved)
    return promise;

  // Set initial state
  let isPending = true;
  let isRejected = false;
  let isFulfilled = false;

  // Observe the promise, saving the fulfillment in a closure scope.
  const result = promise.then(
      function(v) {
          isFulfilled = true;
          isPending = false;
          return v;
      },
      function(e) {
          isRejected = true;
          isPending = false;
          throw e;
      }
  );

  result.isFulfilled = function() { return isFulfilled; };
  result.isPending = function() { return isPending; };
  result.isRejected = function() { return isRejected; };
  return result;
}

module.exports = mockPromise;