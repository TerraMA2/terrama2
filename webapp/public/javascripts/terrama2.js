$(document).ready(function(){
  var socket = io(window.location.origin);

  socket.on('checkPythonScriptResponse', function(result) {
    if(result.hasError || result.hasPythonError) {
      if(result.hasError) {
        $('#systemError').text(result.systemError);
        $('#systemError').css('display', '');
      } else {
        $('#systemError').css('display', 'none');
      }

      $('#analysisSaveButton').attr('disabled', 'disabled');
    } else {
      $('#systemError').css('display', 'none');
      $('#analysisSaveButton').removeAttr('disabled');
    }

    $('#scriptCheckResult').text(result.messages);
  });

  $('#checkAnalysisScript').on('click', function() {
    socket.emit('checkPythonScriptRequest', { script: $('textarea[name="script"]').val() });
  });
});
