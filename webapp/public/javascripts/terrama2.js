$(document).ready(function(){
  var socket = io(window.location.origin);

  socket.on('checkPythonScriptResponse', function(result) {
    if(result.hasError) {
      $('#analysisSaveButton').attr('disabled', 'disabled');
    } else {
      $('#analysisSaveButton').removeAttr('disabled');
    }

    $('#scriptCheckResult').html(result.messages);
  });

  $('#checkAnalysisScript').on('click', function() {
    socket.emit('checkPythonScriptRequest', { script: $('textarea[name="script"]').val() });
  });
});
