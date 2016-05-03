$(document).ready(function(){
  var socket = io(window.location.origin);

  socket.on('checkPythonScriptResponse', function(result) {
    console.log(result);
  });

  $('#checkAnalysisScript').on('click', function() {
    socket.emit('checkPythonScriptRequest', { script: $('textarea[name="script"]').val() });
  });
});
