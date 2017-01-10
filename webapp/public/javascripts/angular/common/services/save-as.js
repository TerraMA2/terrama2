define(
  function() {
    function SaveAs() {
      var a = document.createElement("a");
      document.body.appendChild(a);
      a.style = "display: none";
      return function (data, fileName) {
        var json = JSON.stringify(data, null, 2);

        var blob = new Blob([json], {type: "application/json"});
        var url  = URL.createObjectURL(blob);

        a.download    = fileName;
        a.href        = url;
        a.click();
      };
    }

    return SaveAs;
  }
)